#include <ecg_api.h>

#include <core/ecg_subprograms.h>
#include <core/ecg_host_ctrl.h>
#include <core/ecg_internal.h>
#include <core/ecg_program.h>

#include <help/ecg_allocate.h>
#include <help/ecg_logger.h>
#include <help/ecg_helper.h>
#include <help/ecg_checks.h>
#include <help/ecg_math.h>
#include <help/ecg_geom.h>

namespace ecg {
	void set_logger(std::shared_ptr<spdlog::logger> ptr) {
		std::scoped_lock lock(g_ecg_logger_mutex);
		g_ecg_logger = ptr;
	}

	void cleanup(uint64_t handler) {
		auto& mem = ecg_mem::get_instance();
		mem.delete_memory(handler);
	}
	
	void cleanup_all() {
		auto& mem = ecg_mem::get_instance();
		mem.delete_all_memory();
	}

	vec3_base get_center(const ecg_mesh_t* mesh, ecg_status* status) {
		ecg_status_handler op_res;

		try {
			default_mesh_check(mesh, op_res, status);
			vec3_base acc = sum_vertexes(mesh, status); 
			if (status != nullptr) op_res = *status;
			return acc / mesh->vertexes_size;
		}
		catch (...) {
			on_unknown_exception(op_res, status);
		}

		return vec3_base();
	}

	vec3_base sum_vertexes(const ecg_mesh_t* mesh, ecg_status* status) {
		ecg_status_handler op_res;
		vec3_base result;

		try {
			default_mesh_check(mesh, op_res, status);

			auto& ctrl = ecg_cl::get_instance();
			auto& queue = ctrl.get_cmd_queue();
			auto& context = ctrl.get_context();
			auto& dev = ctrl.get_device();

			const size_t max_work_group_size = ctrl.get_max_work_group_size();
			cl::Program::Sources sources = { summ_vertexes_code };
			auto program = ecg_program::get_program(context, dev, sources, summ_vertexes_name);

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
				cl::Buffer acc_buffer  = cl::Buffer(context, CL_MEM_READ_WRITE, accumulator_buffer_size, nullptr, &err_create_buffer); op_res = err_create_buffer;
				cl::Buffer vert_buffer = cl::Buffer(context, CL_MEM_READ_ONLY,  vertexes_buffer_size,    nullptr, &err_create_buffer); op_res = err_create_buffer;
				cl::Buffer res_buffer  = cl::Buffer(context, CL_MEM_WRITE_ONLY, result_buffer_size,      nullptr, &err_create_buffer); op_res = err_create_buffer;

				cl::NDRange local = max_work_group_size;
				cl::NDRange global = work_groups * max_work_group_size;

				op_res = queue.enqueueFillBuffer(res_buffer, (cl_int(0)), 0, result_buffer_size);
				op_res = queue.enqueueFillBuffer(acc_buffer, (cl_int(0)), 0, accumulator_buffer_size);
				op_res = queue.enqueueWriteBuffer(vert_buffer, CL_FALSE, 0, vertexes_buffer_size, data);
				op_res = queue.finish();

				op_res = program->execute(
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
			on_unknown_exception(op_res, status);
		}

		return result;
	}

	void internal_compute_aabb(
		cl::Context& context, cl::CommandQueue& queue, 
		cl::Buffer& aabb_result, cl::Buffer& vertexes_buffer,
		cl_int vert_size, std::shared_ptr<ecg_program>& program, ecg_status_handler& op_res,
		cl::NDRange global, cl::NDRange local, 
		bounding_box& result_bb
	) {
		op_res = queue.enqueueWriteBuffer(aabb_result, CL_FALSE, 0, sizeof(bounding_box), &default_bb);
		op_res = queue.finish();

		op_res = program->execute(
			queue, compute_aabb_name, global, local,
			vertexes_buffer, vert_size,
			aabb_result);

		op_res = queue.enqueueReadBuffer(aabb_result, CL_FALSE, 0, sizeof(bounding_box), &result_bb);
		op_res = queue.finish();
	}

	bounding_box compute_aabb(const ecg_mesh_t* mesh, ecg_status* status) {
		bounding_box result_bb = default_bb;
		ecg_status_handler op_res;

		try {
			default_mesh_check(mesh, op_res, status);
			
			auto& ctrl = ecg_cl::get_instance();
			auto& queue = ctrl.get_cmd_queue();
			auto& context = ctrl.get_context();
			auto& dev = ctrl.get_device();

			cl::Program::Sources sources = { compute_aabb_code };
			auto program = ecg_program::get_program(context, dev, sources, compute_aabb_name);

			const size_t buffer_size = sizeof(mesh->vertexes[0]) * mesh->vertexes_size;
			cl::Buffer vertexes_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, buffer_size);
			cl::Buffer aabb_result = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(bounding_box));

			cl::NDRange local = cl::NullRange;
			cl::NDRange global = mesh->vertexes_size;
			cl_int vert_size = sizeof(mesh->vertexes[0]) / sizeof(float);

			op_res = queue.enqueueWriteBuffer(vertexes_buffer, CL_FALSE, 0, buffer_size, mesh->vertexes);
			internal_compute_aabb(
				context, queue, aabb_result, vertexes_buffer, vert_size, 
				program, op_res, global, local, result_bb
			);
		}
		catch (...) {
			on_unknown_exception(op_res, status);
		}

		return result_bb;
	}

	full_bounding_box compute_obb(const ecg_mesh_t* mesh, ecg_status* status) {
		full_bounding_box result_obb;
		ecg_status_handler op_res;

		try {
			default_mesh_check(mesh, op_res, status);
			
			auto& ctrl = ecg_cl::get_instance();
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

			auto compute_obb = ecg_program::get_program(context, dev, obb_sources, compute_obb_name);
			cl::NDRange global = mesh->vertexes_size;
			cl::NDRange local = cl::NullRange;

			op_res = queue.enqueueWriteBuffer(vertex_buffer, CL_FALSE, 0, vertex_buffer_size, mesh->vertexes);
			op_res = queue.enqueueWriteBuffer(cov_mat_buffer, CL_FALSE, 0, sizeof(mat3_base), &cov_mat);
			op_res = queue.finish();

			op_res = compute_obb->execute(
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

			op_res = compute_obb->execute(
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
			on_unknown_exception(op_res, status);
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

	float compute_surface_area(const ecg_mesh_t* mesh, ecg_status* status) {
		ecg_status_handler op_res;
		float result = -FLT_MAX;

		try {
			default_mesh_check(mesh, op_res, status);

			auto& ctrl = ecg_cl::get_instance();
			auto& queue = ctrl.get_cmd_queue();
			auto& context = ctrl.get_context();
			auto& dev = ctrl.get_device();

			cl::Program::Sources sources = { compute_surface_area_code };
			auto program = ecg_program::get_program(context, dev, sources, compute_surface_area_name);

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

			op_res = program->execute(
				queue, compute_surface_area_name, global, local,
				vert_buffer, vert_arr_size,
				ind_buffer, ind_arr_size,
				vert_size, surf_area_buff
			);

			op_res = queue.enqueueReadBuffer(surf_area_buff, CL_FALSE, 0, sizeof(float), &result);
			op_res = queue.finish();
		}
		catch (...) {
			on_unknown_exception(op_res, status);
		}

		return result;
	}

	cmp_res compare_meshes(const ecg_mesh_t* m1, const ecg_mesh_t* m2, mat3_base* delta_transform, ecg_status* status) {
		cmp_res result = cmp_res::CMP_UNDEFINED;
		ecg_status_handler op_res;

		try {
			default_mesh_check(m1, op_res, status);
			default_mesh_check(m2, op_res, status);

			const vec3_base m1_center = get_center(m1, status);
			const vec3_base m2_center = get_center(m1, status);
		
			
		}
		catch (...) {
			if (op_res == ecg_status_code::SUCCESS)
				op_res = ecg_status_code::UNKNOWN_EXCEPTION;
			if (status != nullptr)
				*status = op_res.get_status();
		}

		return result;
	}

	mat3_base compute_covariance_matrix(const ecg_mesh_t* mesh, ecg_status* status) {
		mat3_base cov_mat = null_mat3;
		ecg_status_handler op_res;

		try {
			default_mesh_check(mesh, op_res, status);

			auto& ctrl = ecg_cl::get_instance();
			auto& queue = ctrl.get_cmd_queue();
			auto& context = ctrl.get_context();
			auto& dev = ctrl.get_device();

			bounding_box bb = default_bb;
			vec3_base center = get_center(mesh, status);
			cl_float4 center_cl = { center.x, center.y, center.z, 0.0f };
			const cl_int vertex_size = sizeof(vec3_base) / sizeof(float);

			cl_int vertex_buffer_size = mesh->vertexes_size * sizeof(mesh->vertexes[0]);
			cl::Buffer cov_mat_buffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(cov_mat));
			cl::Buffer vertex_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, vertex_buffer_size);

			cl::Program::Sources obb_sources = {
				enable_atomics_def,
				get_vertex,
				compute_cov_mat_code,
				compute_obb_code
			};

			auto compute_obb = ecg_program::get_program(context, dev, obb_sources, compute_cov_mat_name);
			cl::NDRange global = mesh->vertexes_size;
			cl::NDRange local = cl::NullRange;

			op_res = queue.enqueueWriteBuffer(vertex_buffer, CL_FALSE, 0, vertex_buffer_size, mesh->vertexes);
			op_res = queue.enqueueWriteBuffer(cov_mat_buffer, CL_FALSE, 0, sizeof(mat3_base), &cov_mat);
			op_res = queue.finish();

			op_res = compute_obb->execute(
				queue, compute_cov_mat_name, global, local,
				vertex_buffer, vertex_size, center_cl,
				cov_mat_buffer
			);

			op_res = queue.enqueueReadBuffer(cov_mat_buffer, CL_FALSE, 0, sizeof(mat3_base), &cov_mat);
			op_res = queue.finish();
		}
		catch (...) {
			on_unknown_exception(op_res, status);
		}

		return cov_mat;
	}

	bool is_mesh_closed(const ecg_mesh_t* mesh, ecg_status* status) {
		ecg_status_handler op_res;
		bool result = true;

		try {
			default_mesh_check(mesh, op_res, status);

			auto& ctrl = ecg_cl::get_instance();
			auto& queue = ctrl.get_cmd_queue();
			auto& context = ctrl.get_context();
			auto& dev = ctrl.get_device();

			cl_uint vertexes_size = mesh->vertexes_size;
			cl_uint indexes_size = mesh->indexes_size;

			cl_int ind_buffer_size = mesh->indexes_size * sizeof(mesh->indexes[0]);
			cl::Buffer result_buffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(bool));
			cl::Buffer ind_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, ind_buffer_size);

			cl::Program::Sources sources = { is_mesh_closed_code };
			auto is_mesh_close_program = ecg_program::get_program(context, dev, sources, is_mesh_closed_name);
			
			op_res = queue.enqueueWriteBuffer(result_buffer, CL_FALSE, 0, sizeof(bool), &result);
			op_res = queue.enqueueWriteBuffer(ind_buffer, CL_FALSE, 0, ind_buffer_size, mesh->indexes);
			op_res = queue.finish();

			cl::NDRange global = cl::NDRange(mesh->indexes_size);
			cl::NDRange local = cl::NullRange;

			op_res = is_mesh_close_program->execute(
				queue, is_mesh_closed_name, global, local,
				ind_buffer, indexes_size,
				result_buffer
			);

			op_res = queue.enqueueReadBuffer(result_buffer, CL_FALSE, 0, sizeof(bool), &result);
			op_res = queue.finish();
		}
		catch (...) {
			on_unknown_exception(op_res, status);
		}

		return result;
	}

	bool is_mesh_manifold(const ecg_mesh_t* mesh, ecg_status* status) {
		ecg_status_handler op_res;
		bool result = false;

		try {
			default_mesh_check(mesh, op_res, status);
			
			bool is_mesh_self_intersected = false;
			bool all_vertexes_manifold = true;
			bool is_mesh_closed = true;

			auto& ctrl = ecg_cl::get_instance();
			auto& queue = ctrl.get_cmd_queue();
			auto& context = ctrl.get_context();
			auto& dev = ctrl.get_device();

			cl::Program::Sources is_mesh_closed_src = { is_mesh_closed_code };
			cl::Program::Sources is_mesh_vertex_manifold_src = { is_mesh_vertexes_manifold_code };
			cl::Program::Sources is_mesh_self_intersected_src = { is_mesh_self_intersected_code };

			auto is_mesh_closed_prog = ecg_program::get_program(context, dev, is_mesh_closed_src, is_mesh_closed_name);
			auto is_mesh_self_intersected_prog = ecg_program::get_program(context, dev, is_mesh_self_intersected_src, is_mesh_self_intersected_name);
			auto is_mesh_vertexes_manifold_prog = ecg_program::get_program(context, dev, is_mesh_vertex_manifold_src, is_mesh_vertexes_manifold_name);

			cl_int vrt_size = sizeof(mesh->vertexes[0]) / sizeof(float);

			cl_uint indexes_size = mesh->indexes_size;
			cl_uint ind_buffer_size = mesh->indexes_size * sizeof(mesh->indexes[0]);
			cl::Buffer ind_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, ind_buffer_size);

			cl_uint vertexes_size = mesh->vertexes_size;
			cl_uint vert_buffer_size = mesh->vertexes_size * sizeof(mesh->vertexes[0]);
			cl::Buffer vert_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, vert_buffer_size);

			cl::Buffer is_closed_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(bool));
			cl::Buffer is_self_intersected_buffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(bool));
			cl::Buffer all_vertexes_manifold_buffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(bool));

			op_res = queue.enqueueWriteBuffer(all_vertexes_manifold_buffer, CL_FALSE, 0, sizeof(bool), &all_vertexes_manifold);
			op_res = queue.enqueueWriteBuffer(is_closed_buffer, CL_FALSE, 0, sizeof(bool), &is_mesh_closed);
			op_res = queue.enqueueWriteBuffer(vert_buffer, CL_FALSE, 0, vert_buffer_size, mesh->vertexes);
			op_res = queue.enqueueWriteBuffer(ind_buffer, CL_FALSE, 0, ind_buffer_size, mesh->indexes);
			op_res = queue.finish();

			cl::NDRange global = cl::NDRange(mesh->vertexes_size);
			cl::NDRange local = cl::NullRange;

			op_res = is_mesh_closed_prog->execute(
				queue, is_mesh_closed_name, global, local,
				ind_buffer, indexes_size,
				is_closed_buffer
			);

			op_res = is_mesh_vertexes_manifold_prog->execute(
				queue, is_mesh_vertexes_manifold_name, global, local,
				ind_buffer, indexes_size, vertexes_size,
				all_vertexes_manifold_buffer
			);

			global = mesh->indexes_size / 3;
			op_res = is_mesh_self_intersected_prog->execute(
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
			on_unknown_exception(op_res, status);
		}

		return result;
	}

	bool is_mesh_self_intersected(const ecg_mesh_t* mesh, self_intersection_method method, ecg_status* status) {
		ecg_status_handler op_res;
		bool result = false;
		
		try {
			default_mesh_check(mesh, op_res, status);
		
			auto& ctrl = ecg_cl::get_instance();
			auto& queue = ctrl.get_cmd_queue();
			auto& context = ctrl.get_context();
			auto& dev = ctrl.get_device();

			cl_int vrt_size = sizeof(mesh->vertexes[0]) / sizeof(float);
			cl::Program::Sources source = { is_mesh_self_intersected_code };
			auto is_self_intersected_prog = ecg_program::get_program(context, dev, source, is_mesh_self_intersected_name);

			cl_uint indexes_size = mesh->indexes_size;
			cl_uint indexes_buffer_size = sizeof(mesh->indexes[0]) * mesh->indexes_size;
			cl::Buffer indexes_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, indexes_buffer_size);

			cl_uint vertexes_size = mesh->vertexes_size;
			cl_uint vertexes_buffer_size = sizeof(mesh->vertexes[0]) * mesh->vertexes_size;
			cl::Buffer vertexes_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, vertexes_buffer_size);

			cl::Buffer is_self_intersected_buffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(bool));

			if (method == self_intersection_method::SI_BRUTEFORCE) {
				cl::NDRange global = mesh->indexes_size / 3;
				cl::NDRange local = cl::NullRange;

				op_res = queue.enqueueWriteBuffer(indexes_buffer, CL_FALSE, 0, indexes_buffer_size, mesh->indexes);
				op_res = queue.enqueueWriteBuffer(vertexes_buffer, CL_FALSE, 0, vertexes_buffer_size, mesh->vertexes);
				op_res = queue.enqueueWriteBuffer(is_self_intersected_buffer, CL_FALSE, 0, sizeof(bool), &result);
				op_res = queue.finish();

				op_res = is_self_intersected_prog->execute(
					queue, is_mesh_self_intersected_name, global, local,
					vertexes_buffer, vertexes_size, indexes_buffer, indexes_size,
					vrt_size, is_self_intersected_buffer
				);

				op_res = queue.enqueueReadBuffer(is_self_intersected_buffer, CL_FALSE, 0, sizeof(bool), &result);
				op_res = queue.finish();
			}
			else {
				op_res = ecg_status_code::INCORRECT_METHOD;
			}
		}
		catch (...) {
			on_unknown_exception(op_res, status);
		}

		return result;
	}

	ecg_array_t triangulate_mesh(const ecg_mesh_t* mesh, int base_num_vert, ecg_status* status) {
		ecg_status_handler op_res;
		ecg_array_t result_indexes;

		try {
			if (status != nullptr) *status = ecg_status_code::SUCCESS;
			if (mesh == nullptr || base_num_vert <= 0) op_res = ecg_status_code::INVALID_ARG;
			if (mesh->indexes == nullptr || mesh->indexes_size <= 0) op_res = ecg_status_code::EMPTY_INDEX_ARR;
			if (mesh->indexes_size % base_num_vert != 0) op_res = ecg_status_code::INCORRECT_VERTEX_COUNT_IN_FACE;

			auto& ctrl = ecg_cl::get_instance();
			auto& queue = ctrl.get_cmd_queue();
			auto& context = ctrl.get_context();
			auto& dev = ctrl.get_device();

			const size_t triangle_size = 3;
			const cl_uint default_index_value = 0;

			cl_uint old_indexes_size = mesh->indexes_size;
			cl_uint old_faces_cnt = mesh->indexes_size / base_num_vert;
			cl_uint new_faces_cnt = (base_num_vert - 2) * old_faces_cnt;
			cl_uint new_indexes_size = new_faces_cnt * triangle_size;
			cl_uint curr_vertexes_in_face = base_num_vert;

			cl_int err_create_buffer = CL_SUCCESS;
			size_t new_indexes_buffer_size = new_indexes_size * sizeof(uint32_t);
			size_t old_indexes_buffer_size = mesh->indexes_size * sizeof(uint32_t);
			
			cl::Buffer new_indexes_buffer = cl::Buffer(context, CL_MEM_READ_WRITE, new_indexes_buffer_size, nullptr, &err_create_buffer); op_res = err_create_buffer;
			cl::Buffer old_indexes_buffer = cl::Buffer(context, CL_MEM_READ_ONLY,  old_indexes_buffer_size, nullptr, &err_create_buffer); op_res = err_create_buffer;

			cl::Program::Sources sources = { triangulate_mesh_code };
			auto program = ecg_program::get_program(context, dev, sources, triangulate_mesh_name);

			cl::NDRange global = old_faces_cnt;
			cl::NDRange local  = cl::NullRange;

			op_res = queue.enqueueFillBuffer(new_indexes_buffer, default_index_value, 0, new_indexes_buffer_size);
			op_res = queue.enqueueWriteBuffer(old_indexes_buffer, CL_FALSE, 0, old_indexes_buffer_size, mesh->indexes);
			op_res = queue.finish();

			op_res = program->execute(
				queue, triangulate_mesh_name, global, local,
				old_indexes_buffer, old_indexes_size,
				new_indexes_buffer, new_indexes_size,
				old_faces_cnt, curr_vertexes_in_face
			);

			result_indexes = allocate_array<uint32_t>(new_indexes_size);
			op_res = queue.enqueueReadBuffer(new_indexes_buffer, CL_FALSE, 0, new_indexes_buffer_size, result_indexes.arr_ptr);
			op_res = queue.finish();
		}
		catch (...) {
			on_unknown_exception(op_res, status);
		}

		return result_indexes;
	}

	float compute_volume(const ecg_mesh_t* mesh, ecg_status* status) {
		ecg_status_handler op_res;
		float result_volume = -1.0f;

		try {
			default_mesh_check(mesh, op_res, status);
			bool is_manifold = is_mesh_manifold(mesh, status);
			if (!is_manifold) op_res = ecg_status_code::NON_MANIFOLD_MESH;

			auto& ctrl = ecg_cl::get_instance();
			auto& queue = ctrl.get_cmd_queue();
			auto& context = ctrl.get_context();
			auto& dev = ctrl.get_device();

			cl::Program::Sources sources = { compute_volume_code };
			auto program = ecg_program::get_program(context, dev, sources, compute_volume_name);

			cl_uint indexes_size = mesh->indexes_size;
			cl_uint faces_cnt = mesh->indexes_size / 3;
			cl_uint vertexes_size = mesh->vertexes_size;
			size_t vertexes_buffer_size = sizeof(vec3_base) * vertexes_size;
			size_t indexes_buffer_size  = sizeof(uint32_t) * indexes_size;
			size_t volume_buffer_size   = sizeof(cl_float) * faces_cnt;

			cl_float pattern = 0.0f;
			cl_int err_create_buffer = CL_SUCCESS;
			cl::Buffer vertexes_buffer = cl::Buffer(context, CL_MEM_READ_ONLY,  vertexes_buffer_size, nullptr, &err_create_buffer); op_res = err_create_buffer;
			cl::Buffer indexes_buffer  = cl::Buffer(context, CL_MEM_READ_ONLY,  indexes_buffer_size , nullptr, &err_create_buffer); op_res = err_create_buffer;
			cl::Buffer volume_buffer   = cl::Buffer(context, CL_MEM_READ_WRITE, volume_buffer_size  , nullptr, &err_create_buffer); op_res = err_create_buffer;

			cl::NDRange global = faces_cnt;
			cl::NDRange local = cl::NullRange;

			op_res = queue.enqueueWriteBuffer(vertexes_buffer, CL_FALSE, 0, vertexes_buffer_size, mesh->vertexes);
			op_res = queue.enqueueWriteBuffer(indexes_buffer,  CL_FALSE, 0, indexes_buffer_size,  mesh->indexes);
			op_res = queue.enqueueFillBuffer(volume_buffer, pattern, 0, volume_buffer_size);
			op_res = queue.finish();

			op_res = program->execute(
				queue, compute_volume_name, global, local,
				vertexes_buffer, vertexes_size,
				indexes_buffer, indexes_size,
				volume_buffer, faces_cnt 
			);

			// TODO: summ on GPU using volume_buffer
			std::vector<float> volumes; 
			volumes.resize(faces_cnt);
			
			op_res = queue.enqueueReadBuffer(volume_buffer, CL_FALSE, 0, volume_buffer_size, volumes.data());
			queue.finish();

			result_volume = std::accumulate(volumes.begin(), volumes.end(), 0.0f);
		}
		catch (...) {
			on_unknown_exception(op_res, status);
		}

		return result_volume;
	}

	ecg_array_t compute_faces_normals(const ecg_mesh_t* mesh, ecg_status* status) {
		ecg_array_t result_normals;
		ecg_status_handler op_res;

		try {
			default_mesh_check(mesh, op_res, status);
			auto& ctrl = ecg_cl::get_instance();
			auto& queue = ctrl.get_cmd_queue();
			auto& context = ctrl.get_context();
			auto& dev = ctrl.get_device();

			cl::Program::Sources sources = { compute_faces_normals_code };
			auto program = ecg_program::get_program(context, dev, sources, compute_faces_normals_name);

			cl_uint indexes_size = mesh->indexes_size;
			cl_uint faces_cnt = mesh->indexes_size / 3;
			cl_uint vertexes_size = mesh->vertexes_size;
			size_t vertexes_buffer_size = sizeof(vec3_base) * vertexes_size;
			size_t indexes_buffer_size = sizeof(uint32_t) * indexes_size;
			size_t normals_buffer_size = sizeof(vec3_base) * faces_cnt;

			cl_float pattern = 0.0f;
			cl_int err_create_buffer = CL_SUCCESS;
			cl::Buffer vertexes_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, vertexes_buffer_size, nullptr, &err_create_buffer); op_res = err_create_buffer;
			cl::Buffer indexes_buffer  = cl::Buffer(context, CL_MEM_READ_ONLY, indexes_buffer_size,  nullptr, &err_create_buffer); op_res = err_create_buffer;
			cl::Buffer normals_buffer  = cl::Buffer(context, CL_MEM_READ_WRITE, normals_buffer_size, nullptr, &err_create_buffer); op_res = err_create_buffer;

			cl::NDRange global = faces_cnt;
			cl::NDRange local = cl::NullRange;

			op_res = queue.enqueueWriteBuffer(vertexes_buffer, CL_FALSE, 0, vertexes_buffer_size, mesh->vertexes);
			op_res = queue.enqueueWriteBuffer(indexes_buffer, CL_FALSE, 0, indexes_buffer_size, mesh->indexes);
			op_res = queue.enqueueFillBuffer(normals_buffer, pattern, 0, normals_buffer_size);
			op_res = queue.finish();

			op_res = program->execute(
				queue, compute_faces_normals_name, global, local,
				vertexes_buffer, vertexes_size,
				indexes_buffer, indexes_size,
				normals_buffer, faces_cnt
			);

			result_normals = allocate_array<vec3_base>(faces_cnt);
			op_res = queue.enqueueReadBuffer(normals_buffer, CL_FALSE, 0, normals_buffer_size, result_normals.arr_ptr);
			op_res = queue.finish();
		}
		catch (...) {
			on_unknown_exception(op_res, status);
		}

		return result_normals;
	}

	ecg_array_t compute_vertex_normals(const ecg_mesh_t* mesh, ecg_status* status) {
		ecg_status_handler op_res;
		ecg_array_t result;

		try {
			default_mesh_check(mesh, op_res, status);
			auto& ctrl = ecg_cl::get_instance();
			auto& queue = ctrl.get_cmd_queue();
			auto& context = ctrl.get_context();
			auto& dev = ctrl.get_device();

			cl::Program::Sources sources = { compute_vertex_normals_code };
			auto program = ecg_program::get_program(context, dev, sources, compute_vertex_normals_name);

			cl_uint indexes_size = mesh->indexes_size;
			cl_uint vertexes_size = mesh->vertexes_size;
			size_t indexes_buffer_size = sizeof(uint32_t) * indexes_size;
			size_t vertexes_buffer_size = sizeof(vec3_base) * vertexes_size;

			cl_float pattern = 0.0f;
			cl_int err_create_buffer = CL_SUCCESS;
			cl_int vrt_size = sizeof(vec3_base) / sizeof(float);
			cl::Buffer vertexes_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, vertexes_buffer_size, nullptr, &err_create_buffer); op_res = err_create_buffer;
			cl::Buffer normals_buffer = cl::Buffer(context, CL_MEM_READ_WRITE, vertexes_buffer_size, nullptr, &err_create_buffer); op_res = err_create_buffer;
			cl::Buffer indexes_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, indexes_buffer_size, nullptr, &err_create_buffer); op_res = err_create_buffer;

			cl::NDRange global = mesh->vertexes_size;
			cl::NDRange local = cl::NullRange;

			op_res = queue.enqueueWriteBuffer(vertexes_buffer, CL_FALSE, 0, vertexes_buffer_size, mesh->vertexes);
			op_res = queue.enqueueWriteBuffer(indexes_buffer, CL_FALSE, 0, indexes_buffer_size, mesh->indexes);
			op_res = queue.enqueueFillBuffer(normals_buffer, pattern, 0, vertexes_buffer_size);
			op_res = queue.finish();

			op_res = program->execute(
				queue, compute_vertex_normals_name, global, local,
				vertexes_buffer, vertexes_size,
				indexes_buffer, indexes_size,
				vrt_size, normals_buffer
			);

			result = allocate_array<vec3_base>(mesh->vertexes_size);
			op_res = queue.enqueueReadBuffer(normals_buffer, CL_FALSE, 0, vertexes_buffer_size, result.arr_ptr);
			op_res = queue.finish();
		}
		catch (...) {
			on_unknown_exception(op_res, status);
		}

		return result;
	}

	ecg_internal_mesh_t create_convex_hull(const ecg_array_t vrt_arr, ecg_status* status) {
		ecg_internal_mesh_t result;
		ecg_status_handler op_res;

		if (vrt_arr.arr_ptr == nullptr || vrt_arr.arr_size == 0 || vrt_arr.arr_size < 4) {
			if (status != nullptr) *status = ecg_status_code::INVALID_ARG;
			return result;
		}

		try {
			std::span<vec3_base> global_vertexes(static_cast<vec3_base*>(vrt_arr.arr_ptr), vrt_arr.arr_size);
			std::set<vec3_base, ecg_less_func> global_unique_vertexes{ global_vertexes.begin(), global_vertexes.end() };
			std::vector<vec3_base> initial_convex;
			vec3_base initial_center_vertex;
			initial_convex.resize(4);

			vec3_base x_max_vrt = global_vertexes[0];
			vec3_base x_min_vrt = global_vertexes[0];
		
			// Initial tetrahedron
			{
				// 1. Find the farthest points on the X-axis
				for (auto& vrt : global_vertexes) {
					if (x_max_vrt.x < vrt.x) x_max_vrt = vrt;
					if (x_min_vrt.x > vrt.x) x_min_vrt = vrt;
				}

				initial_convex[0] = x_min_vrt;
				initial_convex[1] = x_max_vrt;

				// 2. Find third farthest from line point
				vec3_base third_vertex = global_vertexes[0];
				float third_vertex_distance = 0.0f;

				for (auto& vrt : global_vertexes) {
					float current_distance = distance(vrt, initial_convex[0], initial_convex[1]);
					if (current_distance > third_vertex_distance) {
						third_vertex_distance = current_distance;
						third_vertex = vrt;
					}
				}

				initial_convex[2] = third_vertex;

				// 3. Find third farthest from surface point
				vec3_base fourth_vertex = global_vertexes[0];
				float fourth_vertex_distance = 0.0f;
			
				for (auto& vrt : global_vertexes) {
					float current_distance = distance(vrt, initial_convex[0], initial_convex[1], initial_convex[2]);
					if (current_distance > fourth_vertex_distance) {
						fourth_vertex_distance = current_distance;
						fourth_vertex = vrt;
					}
				}

				initial_convex[3] = fourth_vertex;
				for (auto& vrt : initial_convex) { initial_center_vertex += vrt; }
				initial_center_vertex /= initial_convex.size();
			}

			struct convex_face_t {
				vec3_base v0;
				vec3_base v1;
				vec3_base v2;
				vec3_base normal;
			};

			std::list<convex_face_t> convex_hull_faces;
			auto add_face = [&](vec3_base a, vec3_base b, vec3_base c, vec3_base center) {
				convex_hull_faces.push_back({ a, b, c, normalize((a + b + c) / 3.0f - center) });
			};

			add_face(initial_convex[0], initial_convex[1], initial_convex[2], initial_center_vertex);
			add_face(initial_convex[0], initial_convex[1], initial_convex[3], initial_center_vertex);
			add_face(initial_convex[0], initial_convex[2], initial_convex[3], initial_center_vertex);
			add_face(initial_convex[1], initial_convex[2], initial_convex[3], initial_center_vertex);

			bool convex_hull_changed = true;
			auto uvertexes_copy = global_unique_vertexes;
			std::list<convex_face_t> new_convex_hull = convex_hull_faces;

			// TODO: ‘актически вышло, что в данном алгоритме мы собираем самые отдаленные точки
			// ѕо уму дальше от них нужно построить корректную топологию модели выпуклой оболочки
			// Ќа данный момент топологи€ не верна
			// 0. ѕока добавл€ли хот€ бы одного кандидата
			while (convex_hull_changed) {
				// 1. »терироватьс€ по полигонам
				convex_hull_faces = new_convex_hull;
				convex_hull_changed = false;
				new_convex_hull.clear();
				
				for (auto& face : convex_hull_faces) {
					bool candidate_was_found = false;
					vec3_base new_candidate;
					float max_dist = 0.0f;

					// 1.1. ƒл€ каждого полигона искать самую отдаленную точку вдоль нормали
					for (auto& vertex : uvertexes_copy) {
						float dist = dot(face.normal, vertex - face.v0);
						if (dist > max_dist) {
							max_dist = dist;
							new_candidate = vertex;
							candidate_was_found = true;
						}
					}

					// 1.2. ≈сли нашли такую точку, то замен€ем первую плоскость и добавл€ем новые
					if (candidate_was_found) {
						// 1.3.1. ƒобавить новые сформированные полигоны
						// ”брать точку из просмотра (она теперь часть оболочки)
						vec3_base new_center = face.v0 + face.v1 + face.v2 + new_candidate;
						new_center /= 4;

						add_face(face.v0, face.v1, new_candidate, new_center);
						add_face(face.v1, face.v2, new_candidate, new_center);
						add_face(face.v2, face.v0, new_candidate, new_center);
						uvertexes_copy.erase(new_candidate);
						convex_hull_changed = true;
					}
					else {
						// 1.3.2. ѕросто добавить текущий полигон. 
						// —остав просмотра точек мен€тьс€ не должен (нужно проверить)
						new_convex_hull.push_back(face);
					}
				}
			}
			
			// —формировать данные меша из данных выпуклой оболочки
			std::vector<uint32_t> ch_indexes(new_convex_hull.size() * 3);
			std::vector<vec3_base> ch_vertexes(new_convex_hull.size() * 3);

			size_t id = 0;
			for (auto it = new_convex_hull.begin(); it != new_convex_hull.end(); ++it, ++id) {
				ch_vertexes[id * 3 + 0] = it->v0;
				ch_vertexes[id * 3 + 1] = it->v1;
				ch_vertexes[id * 3 + 2] = it->v2;

				ch_indexes[id * 3 + 0] = id * 3 + 0;
				ch_indexes[id * 3 + 1] = id * 3 + 1;
				ch_indexes[id * 3 + 2] = id * 3 + 2;
			}

			result.vertexes = allocate_array<vec3_base>(ch_vertexes.size() * 3);
			result.indexes = allocate_array<uint32_t>(ch_indexes.size() * 3);

			std::memcpy(result.vertexes.arr_ptr, ch_vertexes.data(), ch_vertexes.size() * sizeof(vec3_base));
			std::memcpy(result.indexes.arr_ptr, ch_indexes.data(), ch_indexes.size() * sizeof(uint32_t));
		}
		catch (...) {
			on_unknown_exception(op_res, status);
			result = ecg_internal_mesh_t{};
		}

		return result;
	}
}