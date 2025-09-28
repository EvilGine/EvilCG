#include <ecg_api.h>

#include <core/ecg_cl_programs.h>
#include <core/ecg_host_ctrl.h>
#include <core/ecg_internal.h>
#include <core/ecg_program.h>

#include <help/ecg_allocate.h>
#include <help/ecg_logger.h>
#include <help/ecg_helper.h>
#include <help/ecg_checks.h>
#include <help/ecg_math.h>
#include <help/ecg_geom.h>

namespace ecg::hulls {
	const float g_convex_epsilon = 1E-06F;

	struct convex_face_t {
		uint64_t v0;
		uint64_t v1;
		uint64_t v2;
		vec3_base normal;
		vec3_base center;
		bool valid = true;
		std::set<uint64_t> outer_vertexes;
	};

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

			result_obb = hulls::bb_to_full_bb(&bb);
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

	void add_face(std::span<vec3_base>& global_vertexes, std::list<convex_face_t>& convex_hull_faces,
		uint64_t ind0, uint64_t ind1, uint64_t ind2, const vec3_base& global_center, float eps = g_convex_epsilon)
	{
		vec3_base a = global_vertexes[ind0];
		vec3_base b = global_vertexes[ind1];
		vec3_base c = global_vertexes[ind2];

		vec3_base ab = b - a;
		vec3_base ac = c - a;
		vec3_base n = cross(ab, ac);
		float nlen2 = dot(n, n);
		if (nlen2 < eps) return;

		vec3_base normal = normalize(n);
		vec3_base center = (a + b + c) / 3.0f;
		if (dot(normal, center - global_center) < 0.0f) normal = -normal;

		convex_face_t face;
		face.v0 = ind0; face.v1 = ind1; face.v2 = ind2;
		face.normal = normal;
		face.center = center;
		face.valid = true;

		for (size_t id = 0; id < global_vertexes.size(); ++id) {
			if (id == face.v0 || id == face.v1 || id == face.v2) continue;
			float d = dot(face.normal, global_vertexes[id] - global_vertexes[face.v0]);
			if (d > 1e-6f) face.outer_vertexes.insert(id);
		}

		convex_hull_faces.push_back(face);
	}

	std::list<convex_face_t> get_initial_tetrahedron(std::span<vec3_base>& global_vertexes, vec3_base& global_center) {
		std::vector<uint64_t> initial_convex;
		vec3_base initial_center_vertex;
		initial_convex.resize(4);

		uint64_t x_max_vrt = 0;
		uint64_t x_min_vrt = 0;

		// 1. Find the farthest points on the X-axis
		size_t vrt_id = 0;
		for (auto& vrt : global_vertexes) {
			if (global_vertexes[x_max_vrt].x < vrt.x) x_max_vrt = vrt_id;
			if (global_vertexes[x_min_vrt].x > vrt.x) x_min_vrt = vrt_id;
			++vrt_id;
		}

		initial_convex[0] = x_min_vrt;
		initial_convex[1] = x_max_vrt;

		// 2. Find third farthest from line point
		float third_vertex_distance = 0.0f;
		uint64_t third_vertex = 0;

		vrt_id = 0;
		for (auto& vrt : global_vertexes) {
			vec3_base ic_0 = global_vertexes[initial_convex[0]];
			vec3_base ic_1 = global_vertexes[initial_convex[1]];

			float current_distance = distance(vrt, ic_0, ic_1);
			if (current_distance > third_vertex_distance) {
				third_vertex_distance = current_distance;
				third_vertex = vrt_id;
			}
			++vrt_id;
		}
		initial_convex[2] = third_vertex;

		// 3. Find third farthest from surface point
		float fourth_vertex_distance = 0.0f;
		uint64_t fourth_vertex = 0;

		vrt_id = 0;
		for (auto& vrt : global_vertexes) {
			vec3_base ic_0 = global_vertexes[initial_convex[0]];
			vec3_base ic_1 = global_vertexes[initial_convex[1]];
			vec3_base ic_2 = global_vertexes[initial_convex[2]];

			float current_distance = distance(vrt, ic_0, ic_1, ic_2);
			if (current_distance > fourth_vertex_distance) {
				fourth_vertex_distance = current_distance;
				fourth_vertex = vrt_id;
			}
			++vrt_id;
		}

		initial_convex[3] = fourth_vertex;
		for (auto& vrt_id : initial_convex) { global_center += global_vertexes[vrt_id]; }
		global_center /= initial_convex.size();

		std::list<convex_face_t> convex_hull_faces;
		add_face(global_vertexes, convex_hull_faces, initial_convex[0], initial_convex[1], initial_convex[2], global_center);
		add_face(global_vertexes, convex_hull_faces, initial_convex[0], initial_convex[1], initial_convex[3], global_center);
		add_face(global_vertexes, convex_hull_faces, initial_convex[0], initial_convex[2], initial_convex[3], global_center);
		add_face(global_vertexes, convex_hull_faces, initial_convex[1], initial_convex[2], initial_convex[3], global_center);

		return convex_hull_faces;
	}

	std::list<convex_face_t> expand_hull(std::span<vec3_base>& global_vertexes, std::list<convex_face_t>& convex_hull_faces, vec3_base global_center) {
		std::list<convex_face_t> new_convex_hull = convex_hull_faces;
		bool convex_hull_changed = true;

		while (convex_hull_changed) {
			convex_hull_faces = new_convex_hull;
			convex_hull_changed = false;
			new_convex_hull.clear();

			for (auto& face : convex_hull_faces) {
				if (!face.valid) continue;
				
				auto& candidate_indexes = face.outer_vertexes;
				uint64_t new_candidate_index = UINT64_MAX;
				bool candidate_was_found = false;
				float max_dist = 0.0f;

				for (uint64_t idx : candidate_indexes) {
					if (idx == face.v0 || idx == face.v1 || idx == face.v2) continue;
					float dist = dot(face.normal, global_vertexes[idx] - global_vertexes[face.v0]);
					if (dist > max_dist) {
						max_dist = dist;
						new_candidate_index = idx;
						candidate_was_found = true;
					}
				}

				if (candidate_was_found && max_dist > g_convex_epsilon) {
					std::unordered_set<uint64_t> affected_points;
					vec3_base new_candidate = global_vertexes[new_candidate_index];
					std::unordered_map<edge_t, uint64_t, ecg_hash_func, ecg_compare_func> edge_counter;

					for (auto& inner_face : convex_hull_faces) {
						if (!inner_face.valid) continue;

						float d = dot(inner_face.normal, new_candidate - global_vertexes[inner_face.v0]);
						if (d > g_convex_epsilon) {
							inner_face.valid = false;
							affected_points.insert(inner_face.outer_vertexes.begin(), inner_face.outer_vertexes.end());

							++edge_counter[make_edge_struct(inner_face.v0, inner_face.v1)];
							++edge_counter[make_edge_struct(inner_face.v1, inner_face.v2)];
							++edge_counter[make_edge_struct(inner_face.v2, inner_face.v0)];
							inner_face.outer_vertexes.clear();
						}
					}

					for (auto& [edge, cnt] : edge_counter) {
						if (cnt == 1) {
							convex_face_t new_face;
							new_face.v0 = edge.a;
							new_face.v1 = edge.b;
							new_face.v2 = new_candidate_index;

							vec3_base a = global_vertexes[edge.a];
							vec3_base b = global_vertexes[edge.b];
							vec3_base c = global_vertexes[new_candidate_index];

							vec3_base ab = b - a;
							vec3_base ac = c - a;
							vec3_base n = cross(ab, ac);
							float nlen2 = dot(n, n);
							if (nlen2 < g_convex_epsilon) continue;

							new_face.normal = normalize(n);
							new_face.center = (a + b + c) / 3.0f;
							if (dot(new_face.normal, new_face.center - global_center) < 0.0f)
								new_face.normal = -new_face.normal;

							new_face.valid = true;

							for (uint64_t idx : affected_points) {
								if (idx == new_face.v0 || idx == new_face.v1 || idx == new_face.v2) continue;
								if (dot(new_face.normal, global_vertexes[idx] - a) > g_convex_epsilon)
									new_face.outer_vertexes.insert(idx);
							}

							new_convex_hull.push_back(std::move(new_face));
						}
					}

					convex_hull_changed = true;
				}
				else {
					new_convex_hull.push_back(face);
				}
			}
		}

		return new_convex_hull;
	}

	ecg_internal_mesh_t create_convex_hull(const ecg_array_t vrt_arr, ecg_status* status) {
		auto& mem_inst = ecg_mem::get_instance();
		ecg_internal_mesh_t result;
		ecg_status_handler op_res;

		if (vrt_arr.arr_ptr == nullptr || vrt_arr.arr_size == 0 || vrt_arr.arr_size < 4) {
			if (status != nullptr) *status = ecg_status_code::INVALID_ARG;
			return result;
		}

		try {
			std::span<vec3_base> global_vertexes(static_cast<vec3_base*>(vrt_arr.arr_ptr), vrt_arr.arr_size);
			vec3_base global_center = vec3_base{ 0.0f, 0.0f, 0.0f };

			auto convex_hull_faces = get_initial_tetrahedron(global_vertexes, global_center);
			auto new_convex_hull = expand_hull(global_vertexes, convex_hull_faces, global_center);

			{
				std::vector<uint32_t> ch_indexes(new_convex_hull.size() * 3);
				size_t face_id = 0;
				
				for (auto it = new_convex_hull.begin(); it != new_convex_hull.end(); ++it, ++face_id) {
					ch_indexes[face_id * 3 + 0] = it->v0;
					ch_indexes[face_id * 3 + 1] = it->v1;
					ch_indexes[face_id * 3 + 2] = it->v2;
				}

				result.vertexes = allocate_array<vec3_base>(global_vertexes.size());
				result.indexes = allocate_array<uint32_t>(ch_indexes.size());

				std::memcpy(result.vertexes.arr_ptr, global_vertexes.data(), global_vertexes.size() * sizeof(vec3_base));
				std::memcpy(result.indexes.arr_ptr, ch_indexes.data(), ch_indexes.size() * sizeof(uint32_t));
			}
		}
		catch (...) {
			on_unknown_exception(op_res, status);
			mem_inst.delete_memory(result.vertexes.handler);
			mem_inst.delete_memory(result.indexes.handler);
			result = ecg_internal_mesh_t{};
		}

		return result;
	}
}