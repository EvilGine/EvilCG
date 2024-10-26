#include <ecg_api.h>

#include <cl/ecg_subprograms.h>
#include <cl/ecg_host_ctrl.h>
#include <cl/ecg_program.h>

#include <help/ecg_helper.h>
#include <help/ecg_geom.h>

namespace ecg {
	// TODO: write CL kernel for get_center
	vec3_base get_center(const mesh_t* mesh, ecg_status* status) {
		ecg_status_handler op_res;

		try {
			if (status != nullptr) *status = status_code::SUCCESS;
			if (mesh == nullptr) op_res = status_code::INVALID_ARG;
			if (mesh->vertexes == nullptr || mesh->vertexes_size <= 0) op_res = status_code::EMPTY_VERTEX_ARR;

			vec3_base acc;
			for (uint32_t id = 0; id < mesh->vertexes_size; ++id)
				acc += mesh->vertexes[id];
			return acc / mesh->vertexes_size;
		}
		catch (...) {
			if (op_res == status_code::SUCCESS)
				op_res = status_code::UNKNOWN_EXCEPTION;
			if (status != nullptr)
				*status = op_res.get_status();
		}

		return vec3_base();
	}

	vec3_base summ_vertexes(const mesh_t* mesh, ecg_status* status) {
		return vec3_base();
	}

	bounding_box compute_aabb(const mesh_t* mesh, ecg_status* status) {
		bounding_box result_bb = default_bb;
		ecg_status_handler op_res;

		try {
			if (status != nullptr) *status = status_code::SUCCESS;
			auto& ctrl = ecg_host_ctrl::get_instance();
			auto& queue = ctrl.get_cmd_queue();
			auto& context = ctrl.get_context();
			auto& dev = ctrl.get_device();

			if (mesh == nullptr) op_res = status_code::INVALID_ARG;
			if (mesh->vertexes == nullptr || mesh->vertexes_size <= 0) op_res = status_code::EMPTY_VERTEX_ARR;

			cl::Program::Sources sources = { compute_aabb_code };
			ecg_program program(context, dev, sources);

			const size_t buffer_size = sizeof(mesh->vertexes[0]) * mesh->vertexes_size;
			cl::Buffer vertexes_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, buffer_size);
			cl::Buffer aabb_result = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(bounding_box));

			cl::NDRange local = cl::NullRange;
			cl::NDRange global = mesh->vertexes_size;
			cl_int vert_size = sizeof(mesh->vertexes[0]) / sizeof(float);

			op_res = queue.enqueueWriteBuffer(aabb_result, CL_FALSE, 0, sizeof(bounding_box), &default_bb);
			op_res = queue.enqueueWriteBuffer(vertexes_buffer, CL_FALSE, 0, buffer_size, mesh->vertexes);
			queue.finish();

			op_res = program.execute(
				queue, compute_aabb_name, global, local,
				vertexes_buffer, vert_size,
				aabb_result);

			op_res = queue.enqueueReadBuffer(aabb_result, CL_FALSE, 0, sizeof(bounding_box), &result_bb);
			queue.finish();
		}
		catch (...) {
			if (op_res == status_code::SUCCESS)
				op_res = status_code::UNKNOWN_EXCEPTION;
			if (status != nullptr)
				*status = op_res.get_status();
		}

		return result_bb;
	}

	full_bounding_box compute_obb(const mesh_t* mesh, ecg_status* status) {
		full_bounding_box result_obb;
		ecg_status_handler op_res;

		try {
			if (mesh == nullptr) op_res = status_code::INVALID_ARG;
			if (mesh->vertexes == nullptr || mesh->vertexes_size <= 0) op_res = status_code::EMPTY_VERTEX_ARR;
			
			if (status != nullptr) *status = status_code::SUCCESS;
			auto& ctrl = ecg_host_ctrl::get_instance();
			auto& queue = ctrl.get_cmd_queue();
			auto& context = ctrl.get_context();
			auto& dev = ctrl.get_device();

			bounding_box bb = default_bb;
			mat3_base cov_mat = null_mat3;
			vec3_base center = get_center(mesh, status);
			cl_float4 center_cl = { center.x, center.y, center.z, 0.0f };
			const cl_int vertex_size = sizeof(vec3_base) / sizeof(float);

			cl_int vertex_buffer_size = mesh->vertexes_size * sizeof(mesh->vertexes[0]);
			cl::Buffer cov_mat_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(cov_mat));
			cl::Buffer vertex_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, vertex_buffer_size);

			cl::Program::Sources obb_sources = {
				enable_atomics_def,
				get_vertex,
				compute_cov_mat_code,
				compute_obb_code 
			};

			ecg_program compute_obb(context, dev, obb_sources);
			cl::NDRange global = mesh->vertexes_size;
			cl::NDRange local = cl::NullRange;

			op_res = queue.enqueueWriteBuffer(vertex_buffer, CL_FALSE, 0, vertex_buffer_size, mesh->vertexes);
			op_res = queue.enqueueWriteBuffer(cov_mat_buffer, CL_FALSE, 0, sizeof(mat3_base), &cov_mat);
			queue.finish();

			op_res = compute_obb.execute(
				queue, compute_cov_mat_name, global, local,
				vertex_buffer, vertex_size, center_cl,
				cov_mat_buffer
			);

			op_res = queue.enqueueReadBuffer(cov_mat_buffer, CL_FALSE, 0, sizeof(mat3_base), &cov_mat);
			queue.finish();

			cov_mat = cov_mat / static_cast<float>(mesh->vertexes_size);
			svd_t svd_mat = compute_svd(cov_mat);
			vec3_base x_axis = { svd_mat.u.m00, svd_mat.u.m10, svd_mat.u.m20 };
			vec3_base y_axis = { svd_mat.u.m01, svd_mat.u.m11, svd_mat.u.m21 };
			vec3_base z_axis = { svd_mat.u.m02, svd_mat.u.m12, svd_mat.u.m22 };

			mat3_base transf = make_transform(z_axis, y_axis);
			mat3_base inv_transf = invert(transf);

			cl::Buffer inv_transf_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(inv_transf));
			cl::Buffer res_bb_buffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(bounding_box));

			op_res = queue.enqueueWriteBuffer(inv_transf_buffer, CL_FALSE, 0, sizeof(inv_transf), &inv_transf);
			op_res = queue.enqueueWriteBuffer(res_bb_buffer, CL_FALSE, 0, sizeof(bounding_box), &bb);
			queue.finish();

			op_res = compute_obb.execute(
				queue, compute_obb_name, global, local,
				vertex_buffer, vertex_size,
				inv_transf_buffer, center_cl,
				res_bb_buffer
			);

			op_res = queue.enqueueReadBuffer(res_bb_buffer, CL_FALSE, 0, sizeof(bounding_box), &bb);
			queue.finish();

			result_obb = bb_to_full_bb(&bb);
			result_obb.p0 = center + transf * result_obb.p0;
			result_obb.p1 = center + transf * result_obb.p1;
			result_obb.p2 = center + transf * result_obb.p2;
			result_obb.p3 = center + transf * result_obb.p3;

			result_obb.p4 = center + transf * result_obb.p4;
			result_obb.p5 = center + transf * result_obb.p5;
			result_obb.p6 = center + transf * result_obb.p6;
			result_obb.p7 = center + transf * result_obb.p7;
		}
		catch (...) {
			if (op_res == status_code::SUCCESS)
				op_res = status_code::UNKNOWN_EXCEPTION;
			if (status != nullptr)
				*status = op_res.get_status();
		}

		return result_obb;
	}

	std::vector<vec3_base> find_nearest_vertices(const mesh_t* mesh, const vec3_base* point, int k, ecg_status* status) {
		std::vector<vec3_base> result;
		ecg_status_handler op_res;

		try {
			if (status != nullptr) *status = status_code::SUCCESS;
			if (mesh == nullptr || k <= 0 || point == nullptr) op_res = status_code::INVALID_ARG;
			if (mesh->vertexes == nullptr || mesh->vertexes_size <= 0) op_res = status_code::EMPTY_VERTEX_ARR; 


		}
		catch (...) {
			if (op_res == status_code::SUCCESS)
				op_res = status_code::UNKNOWN_EXCEPTION;
			if (status != nullptr)
				*status = op_res.get_status();
		}

		return result;
	}

	full_bounding_box bb_to_full_bb(const bounding_box* bb) {
		full_bounding_box res;

		res.p0 = bb->min;
		res.p1 = { bb->max.x, bb->min.y, bb->min.z };
		res.p2 = { bb->max.x, bb->max.y, bb->min.z };
		res.p3 = { bb->min.x, bb->max.y, bb->min.z };

		res.p4 = { bb->min.x, bb->min.y, bb->max.z };
		res.p5 = { bb->max.x, bb->min.y, bb->max.z };
		res.p6 = bb->max;
		res.p7 = { bb->min.x, bb->max.y, bb->max.z };

		return res;
	}
}