#include <ecg_api.h>

#include <cl/ecg_subprograms.h>
#include <cl/ecg_host_ctrl.h>
#include <cl/ecg_program.h>

#include <help/ecg_helper.h>
#include <help/ecg_geom.h>

namespace ecg {
	void default_mesh_check(const mesh_t* mesh, ecg_status_handler& op_res, ecg_status* status) {
		if (status != nullptr) *status = status_code::SUCCESS;
		if (mesh == nullptr) op_res = status_code::INVALID_ARG;
		if (mesh->vertexes == nullptr || mesh->vertexes_size <= 0) op_res = status_code::EMPTY_VERTEX_ARR;
		if (mesh->indexes == nullptr || mesh->indexes_size <= 0) op_res = status_code::EMPTY_INDEX_ARR;
		if (mesh->indexes_size % 3 != 0) op_res = status_code::NOT_TRIANGULATED_MESH;
	}

	void on_exception(ecg_status_handler& op_res, ecg_status* status) {
		if (op_res == status_code::SUCCESS)
			op_res = status_code::UNKNOWN_EXCEPTION;
		if (status != nullptr)
			*status = op_res.get_status();
	}

	vec3_base get_center(const mesh_t* mesh, ecg_status* status) {
		ecg_status_handler op_res;

		try {
			default_mesh_check(mesh, op_res, status);
			vec3_base acc = summ_vertexes(mesh, status); 
			if (status != nullptr) op_res = *status;
			return acc / mesh->vertexes_size;
		}
		catch (...) {
			on_exception(op_res, status);
		}

		return vec3_base();
	}

	vec3_base summ_vertexes(const mesh_t* mesh, ecg_status* status) {
		ecg_status_handler op_res;
		vec3_base result;

		try {
			default_mesh_check(mesh, op_res, status);

			auto& ctrl = ecg_host_ctrl::get_instance();
			auto& queue = ctrl.get_cmd_queue();
			auto& context = ctrl.get_context();
			auto& dev = ctrl.get_device();

			const size_t max_work_group_size = ctrl.get_max_work_group_size();
			cl::Program::Sources sources = { summ_vertexes_code };
			ecg_program program(context, dev, sources);

			auto internal_summ = [&](const vec3_base* data, cl_int data_size) {
				const float temp_groups = static_cast<float>(data_size) / max_work_group_size;
				const size_t work_groups = std::ceil(temp_groups);

				constexpr cl_int vert_sz = sizeof(vec3_base) / sizeof(float);
				constexpr size_t item_sz = sizeof(data[0]);

				const size_t accumulator_buffer_size = work_groups * max_work_group_size * item_sz;
				const size_t vertexes_buffer_size = data_size * item_sz;
				const size_t result_buffer_size = work_groups * item_sz;
				std::vector<vec3_base> result;
				result.resize(work_groups);

				cl_int err_create_buffer = CL_SUCCESS;
				cl::Buffer acc_buffer = cl::Buffer(context, CL_MEM_READ_WRITE, accumulator_buffer_size, nullptr, &err_create_buffer); op_res = err_create_buffer;
				cl::Buffer vert_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, vertexes_buffer_size, nullptr, &err_create_buffer); op_res = err_create_buffer;
				cl::Buffer res_buffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, result_buffer_size, nullptr, &err_create_buffer); op_res = err_create_buffer;

				cl::NDRange local = max_work_group_size;
				cl::NDRange global = work_groups * max_work_group_size;

				op_res = queue.enqueueFillBuffer(res_buffer, (cl_int(0)), 0, result_buffer_size);
				op_res = queue.enqueueFillBuffer(acc_buffer, (cl_int(0)), 0, accumulator_buffer_size);
				op_res = queue.enqueueWriteBuffer(vert_buffer, CL_FALSE, 0, vertexes_buffer_size, data);
				op_res = queue.finish();

				op_res = program.execute(
					queue, summ_vertexes_name, global, local, 
					data_size, vert_sz,
					vert_buffer, acc_buffer,
					res_buffer
				);

				op_res = queue.enqueueReadBuffer(res_buffer, CL_FALSE, 0, result_buffer_size, result.data());
				queue.finish();
				return result;
			};
			
			std::vector<vec3_base> acc_vector;
			acc_vector = internal_summ(mesh->vertexes, mesh->vertexes_size);
			while (acc_vector.size() > 1) {
				acc_vector = std::move(internal_summ(acc_vector.data(), acc_vector.size()));
			}
			result = acc_vector[0];
		}
		catch (...) {
			on_exception(op_res, status);
		}

		return result;
	}

	bounding_box compute_aabb(const mesh_t* mesh, ecg_status* status) {
		bounding_box result_bb = default_bb;
		ecg_status_handler op_res;

		try {
			default_mesh_check(mesh, op_res, status);
			
			auto& ctrl = ecg_host_ctrl::get_instance();
			auto& queue = ctrl.get_cmd_queue();
			auto& context = ctrl.get_context();
			auto& dev = ctrl.get_device();

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
			op_res = queue.finish();

			op_res = program.execute(
				queue, compute_aabb_name, global, local,
				vertexes_buffer, vert_size,
				aabb_result);

			op_res = queue.enqueueReadBuffer(aabb_result, CL_FALSE, 0, sizeof(bounding_box), &result_bb);
			op_res = queue.finish();
		}
		catch (...) {
			on_exception(op_res, status);
		}

		return result_bb;
	}

	full_bounding_box compute_obb(const mesh_t* mesh, ecg_status* status) {
		full_bounding_box result_obb;
		ecg_status_handler op_res;

		try {
			default_mesh_check(mesh, op_res, status);
			
			auto& ctrl = ecg_host_ctrl::get_instance();
			auto& queue = ctrl.get_cmd_queue();
			auto& context = ctrl.get_context();
			auto& dev = ctrl.get_device();

			bounding_box bb = default_bb;
			mat3_base cov_mat = null_mat3;
			vec3_base center = get_center(mesh, status);
			cl_float4 center_cl = { center.x, center.y, center.z, 0.0f };
			constexpr cl_int vertex_size = sizeof(vec3_base) / sizeof(float);

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
			op_res = queue.finish();

			op_res = compute_obb.execute(
				queue, compute_cov_mat_name, global, local,
				vertex_buffer, vertex_size, center_cl,
				cov_mat_buffer
			);

			op_res = queue.enqueueReadBuffer(cov_mat_buffer, CL_FALSE, 0, sizeof(mat3_base), &cov_mat);
			op_res = queue.finish();

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
			op_res = queue.finish();

			op_res = compute_obb.execute(
				queue, compute_obb_name, global, local,
				vertex_buffer, vertex_size,
				inv_transf_buffer, center_cl,
				res_bb_buffer
			);

			op_res = queue.enqueueReadBuffer(res_bb_buffer, CL_FALSE, 0, sizeof(bounding_box), &bb);
			op_res = queue.finish();

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
			on_exception(op_res, status);
		}

		return result_obb;
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

	ecg_array_t find_nearest_vertices(const mesh_t* mesh, const vec3_base* point, int k, ecg_status* status) {
		ecg_array_t result;
		ecg_status_handler op_res;

		try {
			default_mesh_check(mesh, op_res, status);


		}
		catch (...) {
			on_exception(op_res, status);
		}

		return result;
	}

	float compute_surface_area(const mesh_t* mesh, ecg_status* status) {
		ecg_status_handler op_res;
		float result = -FLT_MAX;

		try {
			default_mesh_check(mesh, op_res, status);

			auto& ctrl = ecg_host_ctrl::get_instance();
			auto& queue = ctrl.get_cmd_queue();
			auto& context = ctrl.get_context();
			auto& dev = ctrl.get_device();

			cl::Program::Sources sources = { compute_surface_area_code };
			ecg_program program(context, dev, sources);

			const cl_int vert_buffer_sz = sizeof(mesh->vertexes[0]) * mesh->vertexes_size;
			const cl_int ind_buffer_sz = sizeof(mesh->indexes[0]) * mesh->indexes_size;
			const cl_int vert_size = sizeof(mesh->vertexes[0]) / sizeof(float);

			const cl_int vert_arr_size = mesh->vertexes_size;
			const cl_int ind_arr_size = mesh->indexes_size;

			cl::Buffer vert_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, vert_buffer_sz);
			cl::Buffer ind_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, ind_buffer_sz);
			cl::Buffer surf_area_buff = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float));

			cl::NDRange local = cl::NullRange;
			cl::NDRange global = mesh->indexes_size / 3;

			op_res = queue.enqueueWriteBuffer(vert_buffer, CL_FALSE, 0, vert_buffer_sz, mesh->vertexes);
			op_res = queue.enqueueWriteBuffer(ind_buffer, CL_FALSE, 0, ind_buffer_sz, mesh->indexes);
			op_res = queue.enqueueFillBuffer(surf_area_buff, 0, 0, sizeof(float));
			op_res = queue.finish();

			op_res = program.execute(
				queue, compute_surface_area_name, global, local,
				vert_buffer, vert_arr_size,
				ind_buffer, ind_arr_size,
				vert_size, surf_area_buff
			);

			op_res = queue.enqueueReadBuffer(surf_area_buff, CL_FALSE, 0, sizeof(float), &result);
			op_res = queue.finish();
		}
		catch (...) {
			on_exception(op_res, status);
		}

		return result;
	}

	cmp_res compare_meshes(const mesh_t* m1, const mesh_t* m2, mat3_base* delta_transform, ecg_status* status) {
		cmp_res result = cmp_res::UNDEFINED;
		ecg_status_handler op_res;

		try {
			default_mesh_check(m1, op_res, status);
			default_mesh_check(m2, op_res, status);

			const vec3_base m1_center = get_center(m1, status);
			const vec3_base m2_center = get_center(m1, status);
		
			
		}
		catch (...) {
			if (op_res == status_code::SUCCESS)
				op_res = status_code::UNKNOWN_EXCEPTION;
			if (status != nullptr)
				*status = op_res.get_status();
		}

		return result;
	}

	mat3_base compute_covariance_matrix(const mesh_t* mesh, ecg_status* status) {
		mat3_base cov_mat = null_mat3;
		ecg_status_handler op_res;

		try {
			default_mesh_check(mesh, op_res, status);

			auto& ctrl = ecg_host_ctrl::get_instance();
			auto& queue = ctrl.get_cmd_queue();
			auto& context = ctrl.get_context();
			auto& dev = ctrl.get_device();

			bounding_box bb = default_bb;
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
			op_res = queue.finish();

			op_res = compute_obb.execute(
				queue, compute_cov_mat_name, global, local,
				vertex_buffer, vertex_size, center_cl,
				cov_mat_buffer
			);

			op_res = queue.enqueueReadBuffer(cov_mat_buffer, CL_FALSE, 0, sizeof(mat3_base), &cov_mat);
			op_res = queue.finish();
		}
		catch (...) {
			on_exception(op_res, status);
		}

		return cov_mat;
	}

	bool is_mesh_closed(const mesh_t* mesh, ecg_status* status) {
		ecg_status_handler op_res;
		bool result = true;

		try {
			default_mesh_check(mesh, op_res, status);

			auto& ctrl = ecg_host_ctrl::get_instance();
			auto& queue = ctrl.get_cmd_queue();
			auto& context = ctrl.get_context();
			auto& dev = ctrl.get_device();

			cl_uint vertexes_size = mesh->vertexes_size;
			cl_uint indexes_size = mesh->indexes_size;

			cl_int ind_buffer_size = mesh->indexes_size * sizeof(mesh->indexes[0]);
			cl::Buffer result_buffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(bool));
			cl::Buffer ind_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, ind_buffer_size);

			cl::Program::Sources sources = { is_mesh_closed_code };
			ecg_program is_mesh_close_program(context, dev, sources);
			
			op_res = queue.enqueueWriteBuffer(result_buffer, CL_FALSE, 0, sizeof(bool), &result);
			op_res = queue.enqueueWriteBuffer(ind_buffer, CL_FALSE, 0, ind_buffer_size, mesh->indexes);
			op_res = queue.finish();

			cl::NDRange global = cl::NDRange(mesh->indexes_size);
			cl::NDRange local = cl::NullRange;

			op_res = is_mesh_close_program.execute(
				queue, is_mesh_closed_name, global, local,
				ind_buffer, indexes_size,
				result_buffer
			);

			op_res = queue.enqueueReadBuffer(result_buffer, CL_FALSE, 0, sizeof(bool), &result);
			op_res = queue.finish();
		}
		catch (...) {
			on_exception(op_res, status);
		}

		return result;
	}

	bool is_mesh_manifold(const mesh_t* mesh, ecg_status* status) {
		ecg_status_handler op_res;
		bool result = false;

		try {
			default_mesh_check(mesh, op_res, status);
			
			bool is_mesh_self_intersected = false;
			bool all_vertexes_manifold = true;
			bool is_mesh_closed = true;

			auto& ctrl = ecg_host_ctrl::get_instance();
			auto& queue = ctrl.get_cmd_queue();
			auto& context = ctrl.get_context();
			auto& dev = ctrl.get_device();

			cl::Program::Sources is_mesh_closed_src = { is_mesh_closed_code };
			cl::Program::Sources is_mesh_vertex_manifold_src = { is_mesh_vertexes_manifold_code };
			cl::Program::Sources is_mesh_self_intersected_src = { is_mesh_self_intersected_code };

			ecg_program is_mesh_closed_prog(context, dev, is_mesh_closed_src);
			ecg_program is_mesh_vertexes_manifold_prog(context, dev, is_mesh_vertex_manifold_src);
			ecg_program is_mesh_self_intersected_prog(context, dev, is_mesh_self_intersected_src);

			cl_int vrt_size = sizeof(mesh->vertexes[0]) / sizeof(float);

			cl_uint indexes_size = mesh->indexes_size;
			cl_uint ind_buffer_size = mesh->indexes_size * sizeof(mesh->indexes[0]);
			cl::Buffer ind_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, ind_buffer_size);

			cl_uint vertexes_size = mesh->vertexes_size;
			cl_uint vert_buffer_size = mesh->vertexes_size * sizeof(mesh->vertexes[0]);
			cl::Buffer vert_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, vert_buffer_size);

			cl::Buffer is_closed_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(bool));
			cl::Buffer is_self_intersected_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(bool));
			cl::Buffer all_vertexes_manifold_buffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(bool));

			op_res = queue.enqueueWriteBuffer(all_vertexes_manifold_buffer, CL_FALSE, 0, sizeof(bool), &all_vertexes_manifold);
			op_res = queue.enqueueWriteBuffer(is_closed_buffer, CL_FALSE, 0, sizeof(bool), &is_mesh_closed);
			op_res = queue.enqueueWriteBuffer(vert_buffer, CL_FALSE, 0, vert_buffer_size, mesh->vertexes);
			op_res = queue.enqueueWriteBuffer(ind_buffer, CL_FALSE, 0, ind_buffer_size, mesh->indexes);
			op_res = queue.finish();

			cl::NDRange global = cl::NDRange(mesh->vertexes_size);
			cl::NDRange local = cl::NullRange;

			op_res = is_mesh_closed_prog.execute(
				queue, is_mesh_closed_name, global, local,
				ind_buffer, indexes_size,
				is_closed_buffer
			);

			op_res = is_mesh_vertexes_manifold_prog.execute(
				queue, is_mesh_vertexes_manifold_name, global, local,
				ind_buffer, indexes_size, vertexes_size,
				all_vertexes_manifold_buffer
			);

			global = mesh->indexes_size / 3;
			op_res = is_mesh_self_intersected_prog.execute(
				queue, is_mesh_self_intersected_name, global, local,
				vert_buffer, vertexes_size, ind_buffer, indexes_size,
				vrt_size, is_self_intersected_buffer
			);

			op_res = queue.enqueueReadBuffer(is_self_intersected_buffer, CL_FALSE, 0, sizeof(bool), &is_mesh_self_intersected);
			op_res = queue.enqueueReadBuffer(all_vertexes_manifold_buffer, CL_FALSE, 0, sizeof(bool), &all_vertexes_manifold);
			op_res = queue.enqueueReadBuffer(is_closed_buffer, CL_FALSE, 0, sizeof(bool), &is_mesh_closed);
			op_res = queue.finish();

			result = is_mesh_closed && all_vertexes_manifold && !is_mesh_self_intersected;
		}
		catch (...) {
			on_exception(op_res, status);
		}

		return result;
	}

	bool is_mesh_self_intersected(const mesh_t* mesh, self_intersection_method method, ecg_status* status) {
		ecg_status_handler op_res;
		bool result = false;
		
		try {
			default_mesh_check(mesh, op_res, status);
		
			auto& ctrl = ecg_host_ctrl::get_instance();
			auto& queue = ctrl.get_cmd_queue();
			auto& context = ctrl.get_context();
			auto& dev = ctrl.get_device();

			cl_int vrt_size = sizeof(mesh->vertexes[0]) / sizeof(float);
			cl::Program::Sources source = { is_mesh_self_intersected_code };
			ecg_program is_self_intersected_prog(context, dev, source);

			cl_uint indexes_size = mesh->indexes_size;
			cl_uint indexes_buffer_size = sizeof(mesh->indexes[0]) * mesh->indexes_size;
			cl::Buffer indexes_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, indexes_buffer_size);

			cl_uint vertexes_size = mesh->vertexes_size;
			cl_uint vertexes_buffer_size = sizeof(mesh->vertexes[0]) * mesh->vertexes_size;
			cl::Buffer vertexes_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, vertexes_buffer_size);

			cl::Buffer is_self_intersected_buffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(bool));

			if (method == self_intersection_method::BRUTEFORCE) {
				cl::NDRange global = mesh->indexes_size / 3;
				cl::NDRange local = cl::NullRange;

				op_res = queue.enqueueWriteBuffer(indexes_buffer, CL_FALSE, 0, indexes_buffer_size, mesh->indexes);
				op_res = queue.enqueueWriteBuffer(vertexes_buffer, CL_FALSE, 0, vertexes_buffer_size, mesh->vertexes);
				op_res = queue.enqueueWriteBuffer(is_self_intersected_buffer, CL_FALSE, 0, sizeof(bool), &result);
				op_res = queue.finish();

				op_res = is_self_intersected_prog.execute(
					queue, is_mesh_self_intersected_name, global, local,
					vertexes_buffer, vertexes_size, indexes_buffer, indexes_size,
					vrt_size, is_self_intersected_buffer
				);

				op_res = queue.enqueueReadBuffer(is_self_intersected_buffer, CL_FALSE, 0, sizeof(bool), &result);
				op_res = queue.finish();
			}
			else {
				op_res = status_code::INCORRECT_METHOD;
			}
		}
		catch (...) {
			on_exception(op_res, status);
		}

		return result;
	}
}