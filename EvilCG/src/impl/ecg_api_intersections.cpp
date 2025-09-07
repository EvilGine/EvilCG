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
	using fp = std::pair<uint32_t, uint32_t>;
	template <typename T, typename Hasher> using uset = std::unordered_set<T, Hasher>;
	template <typename T1, typename T2, typename Hasher> using umap = std::unordered_map<T1, T2, Hasher>;

	const int min_votes_for_inner_vertex = 2;

	intersection_set_t get_intersection_points(const ecg_mesh_t* m1, const ecg_mesh_t* m2, ecg_status* status) {
		auto& mem_inst = ecg_mem::get_instance();
		auto& ctrl = ecg_cl::get_instance();
		auto queue = ctrl.get_cmd_queue();
		auto context = ctrl.get_context();
		auto device = ctrl.get_device();
		ecg_status_handler op_res;

		intersection_set_t empty_res;
		intersection_set_t res;
		ecg_array_t empty;

		empty.arr_ptr = nullptr;
		empty.arr_size = 0;

		empty_res.vrt = empty;
		empty_res.ind = empty;
		res = empty_res;

		try {
			default_mesh_check(m1, op_res, status);
			default_mesh_check(m2, op_res, status);

			cl::Program::Sources sources = { intersect_two_meshes_code };
			ecg_program program(context, device, sources);

			cl_uint m1_faces_cnt = m1->indexes_size / 3;
			std::vector<uint32_t> vrt_offsets;
			vrt_offsets.resize(m1_faces_cnt);

			cl_uint m1_vertexes_buffer_size = m1->vertexes_size * sizeof(vec3_base);
			cl_uint m2_vertexes_buffer_size = m2->vertexes_size * sizeof(vec3_base);
			cl::Buffer m1_vertexes_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, m1_vertexes_buffer_size);
			cl::Buffer m2_vertexes_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, m2_vertexes_buffer_size);

			cl_uint m1_indexes_buffer_size = m1->indexes_size * sizeof(uint32_t);
			cl_uint m2_indexes_buffer_size = m2->indexes_size * sizeof(uint32_t);
			cl::Buffer m1_indexes_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, m1_indexes_buffer_size);
			cl::Buffer m2_indexes_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, m2_indexes_buffer_size);

			cl_uint vrt_offsets_buffer_size = m1_faces_cnt * sizeof(uint32_t);
			cl::Buffer vrt_offsets_buffer = cl::Buffer(context, CL_MEM_READ_WRITE, vrt_offsets_buffer_size);

			cl::NDRange global = m1_faces_cnt;
			cl::NDRange local = cl::NullRange;
			cl_long long_null_value = 0;
			cl_int null_value = 0;
			cl_int pattern = 0;

			cl_int m1_vrt_size = m1->vertexes_size;
			cl_int m2_vrt_size = m2->vertexes_size;
			cl_int m1_ind_size = m1->indexes_size;
			cl_int m2_ind_size = m2->indexes_size;
			cl_int vrt_off_size = m1_faces_cnt;

			op_res = queue.enqueueWriteBuffer(m1_vertexes_buffer, CL_FALSE, 0, m1_vertexes_buffer_size, m1->vertexes);
			op_res = queue.enqueueWriteBuffer(m2_vertexes_buffer, CL_FALSE, 0, m2_vertexes_buffer_size, m2->vertexes);

			op_res = queue.enqueueWriteBuffer(m1_indexes_buffer, CL_FALSE, 0, m1_indexes_buffer_size, m1->indexes);
			op_res = queue.enqueueWriteBuffer(m2_indexes_buffer, CL_FALSE, 0, m2_indexes_buffer_size, m2->indexes);

			op_res = queue.enqueueFillBuffer(vrt_offsets_buffer, pattern, 0, vrt_offsets_buffer_size);
			op_res = queue.finish();

			op_res = program.execute(
				// in
				queue, intersect_two_meshes_name, global, local,
				m1_vertexes_buffer, m1_vrt_size,
				m2_vertexes_buffer, m2_vrt_size,
				m1_indexes_buffer, m1_ind_size,
				m2_indexes_buffer, m2_ind_size,
				vrt_offsets_buffer, vrt_off_size,
				// out (first iterate calculate size of out)
				nullptr, long_null_value,
				nullptr, long_null_value
			);

			op_res = queue.enqueueReadBuffer(vrt_offsets_buffer, CL_FALSE, 0, vrt_offsets_buffer_size, vrt_offsets.data());
			queue.finish();

			auto item = std::find_if(
				vrt_offsets.begin(), vrt_offsets.end(),
				[](uint32_t value) { return value != 0; }
			);
			if (item == vrt_offsets.end())
				return empty_res;

			cl_long number_of_intersections = 0;
			for (size_t id = 0; id < vrt_offsets.size(); ++id) {
				uint32_t temp = vrt_offsets[id];
				vrt_offsets[id] += number_of_intersections;
				number_of_intersections += temp;
			}
			cl_long number_of_faces = number_of_intersections * 2;

			std::vector<vec3_base> intersections;
			intersections.resize(number_of_intersections);

			std::vector<uint32_t> int_faces;
			int_faces.resize(number_of_faces);

			cl_long faces_buffer_size = number_of_faces * sizeof(uint32_t);
			cl::Buffer faces_buffer = cl::Buffer(context, CL_MEM_READ_WRITE, faces_buffer_size);

			cl_long intersections_buffer_size = number_of_intersections * sizeof(vec3_base);
			cl::Buffer intersections_buffer = cl::Buffer(context, CL_MEM_READ_WRITE, intersections_buffer_size);

			op_res = queue.enqueueWriteBuffer(vrt_offsets_buffer, CL_FALSE, 0, vrt_offsets_buffer_size, vrt_offsets.data());
			op_res = queue.finish();

			op_res = program.execute(
				// in
				queue, intersect_two_meshes_name, global, local,
				m1_vertexes_buffer, m1_vrt_size,
				m2_vertexes_buffer, m2_vrt_size,
				m1_indexes_buffer, m1_ind_size,
				m2_indexes_buffer, m2_ind_size,
				vrt_offsets_buffer, vrt_off_size,
				// out
				intersections_buffer, number_of_intersections,
				faces_buffer, number_of_faces
			);

			op_res = queue.enqueueReadBuffer(intersections_buffer, CL_FALSE, 0, intersections_buffer_size, intersections.data());
			queue.finish();

			op_res = queue.enqueueReadBuffer(faces_buffer, CL_FALSE, 0, faces_buffer_size, int_faces.data());
			queue.finish();

			auto [opt_vrt, opt_ind] = optimize_intersection_set(intersections, int_faces);
			intersection_set_t temp_int_set;

			res.vrt = allocate_array<vec3_base>(opt_vrt.size());
			res.ind = allocate_array<uint32_t>(opt_ind.size());

			safe_copy_to_arr(res.vrt, opt_vrt);
			safe_copy_to_arr(res.ind, opt_ind);
		}
		catch (...) {
			on_unknown_exception(op_res, status);
			if (res.vrt.handler != 0) mem_inst.delete_memory(res.vrt.handler);
			if (res.ind.handler != 0) mem_inst.delete_memory(res.ind.handler);
			res = empty_res;
		}

		return res;
	}

	bool check_is_point_in_mesh(const ecg_mesh_t* mesh, const vec3_base vertex) {
		try {
			auto randf = [](float min, float max) {
				return min + (max - min) * (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX));
				};

			auto base = normalize({ randf(-1,1), randf(-1,1), randf(-1,1) });
			std::array<vec3_base, 4> directions = {
				base , // Random vector
				normalize(vec3_base { 1.0f, 0.0f, 0.0f } + base), // X-axis
				normalize(vec3_base { 0.0f, 1.0f, 0.0f } + base), // Y-axis
				normalize(vec3_base { 0.0f, 0.0f, 1.0f } + base)  // Z-axis
			};

			if (mesh == nullptr) return false;
			if (mesh->indexes == nullptr) return false;
			if (mesh->vertexes == nullptr || mesh->vertexes_size == 0) return false;
			if (mesh->indexes_size == 0 || mesh->indexes_size % 3 != 0) return false;

			std::span<vec3_base> vertexes(mesh->vertexes, mesh->vertexes_size);
			std::span<face_t> faces(reinterpret_cast<face_t*>(mesh->indexes), mesh->indexes_size / 3);

			int votes = 0;
			for (auto dir : directions) {
				int intersections = 0;

				for (auto face : faces) {
					vec3_base s0 = vertexes[face.ind_1];
					vec3_base s1 = vertexes[face.ind_2];
					vec3_base s2 = vertexes[face.ind_3];

					if (ray_intersects_triangle(vertex, dir, s0, s1, s2)) {
						++intersections;
					}
				}

				if (intersections % 2 == 1)
					++votes;
			}

			return votes >= min_votes_for_inner_vertex;
		}
		catch (const std::exception& ex) {
			spdlog::error(ex.what());
		}
		catch (...) {
			spdlog::error(g_unknown_error);
		}

		return false;
	}

	bool check_is_point_in_mesh_gpu(const ecg_cl_mesh_t& mesh, const vec3_base vertex) {
		auto& ctrl = ecg_cl::get_instance();
		auto queue = ctrl.get_cmd_queue();
		auto context = ctrl.get_context();
		auto device = ctrl.get_device();
		ecg_status_handler op_res;

		auto randf = [](float min, float max) {
			return min + (max - min) * (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX));
		};

		auto base = normalize({ randf(-1,1), randf(-1,1), randf(-1,1) });
		std::array<vec3_base, 4> directions = {
				base , // Random vector
				normalize(vec3_base { 1.0f, 0.0f, 0.0f } + base), // X-axis
				normalize(vec3_base { 0.0f, 1.0f, 0.0f } + base), // Y-axis
				normalize(vec3_base { 0.0f, 0.0f, 1.0f } + base)  // Z-axis
		};

		try {
			if (mesh.indexes_buffer == cl::Buffer()) return false;
			if (mesh.vertexes_buffer == cl::Buffer()) return false;

			if (mesh.vertexes_size == 0) return false;
			if (mesh.indexes_size == 0 || mesh.indexes_size % 3 != 0) return false;

			cl_uint vertexes_buffer_size = mesh.vertexes_size * sizeof(vec3_base);
			cl_uint indexes_buffer_size = mesh.indexes_size * sizeof(uint32_t);
			cl::Buffer int_buffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uint));

			cl::NDRange local = cl::NullRange;
			cl::NDRange global = mesh.indexes_size / 3;

			cl::Program::Sources sources = { check_is_point_in_mesh_code };
			ecg_program program(context, device, sources);

			int votes = 0;
			for (auto dir : directions) {
				cl_int intersections = 0;
				queue.enqueueWriteBuffer(int_buffer, CL_FALSE, 0, sizeof(intersections), &intersections);
				queue.finish();

				op_res = program.execute(
					queue, check_is_point_in_mesh_name, global, local,
					mesh.vertexes_buffer, (cl_uint)mesh.vertexes_size,
					mesh.indexes_buffer, (cl_uint)mesh.indexes_size,
					(cl_float)dir.x, (cl_float)dir.y, (cl_float)dir.z,
					(cl_float)vertex.x, (cl_float)vertex.y, (cl_float)vertex.z,
					int_buffer
				);
				queue.finish();

				queue.enqueueReadBuffer(int_buffer, CL_FALSE, 0, sizeof(intersections), &intersections);
				queue.finish();

				if (intersections % 2 == 1)
					++votes;
			}

			return votes >= min_votes_for_inner_vertex;
		}
		catch (const std::exception& ex) {
			spdlog::error(ex.what());
		}
		catch (...) {
			spdlog::error(g_unknown_error);
		}

		return false;
	}

	void add_inner_vertexes(
		const std::span<vec3_base>& vertexes, const std::span<face_t>& faces, const ecg_cl_mesh_t& cl_mesh,
		const umap<fp, uset<uint32_t, std::hash<uint32_t>>, ecg_hash_func>& edge_to_faces,
		const umap<uint32_t, face_t, std::hash<uint32_t>>& faces_from_mesh,
		uset<uint32_t, std::hash<uint32_t>>& new_vertexes
	) {
		bool find_new_vertex = false;
		size_t current_iteration = 0;
		const size_t max_iterations = 1024;

		umap<uint32_t, face_t, std::hash<uint32_t>> processed_faces;
		auto to_process = faces_from_mesh;

		// Try to get nearest points | faces
		auto get_nearest_faces = [](
			std::span<face_t> faces,
			const umap<fp, uset<uint32_t, std::hash<uint32_t>>, ecg_hash_func>& edge_to_faces,
			const umap<uint32_t, face_t, std::hash<uint32_t>>& to_process,
			const umap<uint32_t, face_t, std::hash<uint32_t>>& processed
			) -> umap<uint32_t, face_t, std::hash<uint32_t>>
			{
				umap<uint32_t, face_t, std::hash<uint32_t>> nearest;

				for (auto [id, face] : to_process) {
					fp edges[] = {
						make_edge(face.ind_1, face.ind_2),
						make_edge(face.ind_2, face.ind_3),
						make_edge(face.ind_3, face.ind_1)
					};

					for (auto edge : edges) {
						auto it = edge_to_faces.find(edge);
						if (it != edge_to_faces.end()) {
							for (auto neighbor_id : it->second) {
								if (!processed.contains(neighbor_id) && !nearest.contains(neighbor_id)) {
									nearest.insert({ neighbor_id, faces[neighbor_id] });
								}
							}
						}
					}
				}

				return nearest;
			};

		do {
			find_new_vertex = false;
			auto nearest_faces = get_nearest_faces(
				faces, edge_to_faces, to_process, processed_faces
			);

			bool add_new_vertex = false;
			for (auto [id, face] : nearest_faces) {
				vec3_base v0 = vertexes[face.ind_1];
				vec3_base v1 = vertexes[face.ind_2];
				vec3_base v2 = vertexes[face.ind_3];

				if (check_is_point_in_mesh_gpu(cl_mesh, v0)) {
					new_vertexes.insert(face.ind_1);
					add_new_vertex = true;
				}

				if (check_is_point_in_mesh_gpu(cl_mesh, v1)) {
					new_vertexes.insert(face.ind_2);
					add_new_vertex = true;
				}

				if (check_is_point_in_mesh_gpu(cl_mesh, v2)) {
					new_vertexes.insert(face.ind_3);
					add_new_vertex = true;
				}
			}

			processed_faces.insert(nearest_faces.begin(), nearest_faces.end());
			to_process = std::move(nearest_faces);
			find_new_vertex = add_new_vertex;
			++current_iteration;
		} while (find_new_vertex && current_iteration < max_iterations);
	}

	ecg_array_t add_interior_intersection_points(
		const ecg_mesh_t* m1, const ecg_mesh_t* m2, const intersection_set_t* int_set, ecg_status* status
	) {
		auto& mem_inst = ecg_mem::get_instance();
		std::list<void*> allocated_memory;
		ecg_status_handler op_res;

		ecg_array_t result;
		ecg_array_t empty;

		empty.arr_ptr = nullptr;
		empty.arr_size = 0;
		result = empty;

		try {
			default_mesh_check(m1, op_res, status);
			default_mesh_check(m2, op_res, status);

			std::span<vec3_base> intersection_points(static_cast<vec3_base*>(int_set->vrt.arr_ptr), int_set->vrt.arr_size);
			auto ind_arr = static_cast<uint32_t*>(int_set->ind.arr_ptr);
			auto ind_arr_size = int_set->ind.arr_size;
			auto vrt_arr_size = int_set->vrt.arr_size;

			// Main logic for searching interior points
			{
				ecg_cl_mesh_t cl_m1 = allocate_cl_mesh(m1);
				ecg_cl_mesh_t cl_m2 = allocate_cl_mesh(m2);

				// Intersection data
				umap<fp, uset<uint32_t, std::hash<uint32_t>>, ecg_hash_func> edge_to_faces_a;
				umap<fp, uset<uint32_t, std::hash<uint32_t>>, ecg_hash_func> edge_to_faces_b;
				umap<uint32_t, face_t, std::hash<uint32_t>> faces_from_mesh_a;
				umap<uint32_t, face_t, std::hash<uint32_t>> faces_from_mesh_b;

				uset<uint32_t, std::hash<uint32_t>> new_vertexes_a;
				uset<uint32_t, std::hash<uint32_t>> new_vertexes_b;

				// A little dangerous use reinterpret_cast, but easy work and process
				std::span<face_t> b_faces(reinterpret_cast<face_t*>(m2->indexes), m2->indexes_size / 3);
				std::span<face_t> a_faces(reinterpret_cast<face_t*>(m1->indexes), m1->indexes_size / 3);
				std::span<vec3_base> b_vertexes(m2->vertexes, m2->vertexes_size);
				std::span<vec3_base> a_vertexes(m1->vertexes, m1->vertexes_size);

				for (size_t id = 0; id < a_faces.size(); ++id) {
					auto face = a_faces[id];
					edge_to_faces_a[make_edge(face.ind_1, face.ind_2)].insert(id);
					edge_to_faces_a[make_edge(face.ind_2, face.ind_3)].insert(id);
					edge_to_faces_a[make_edge(face.ind_3, face.ind_1)].insert(id);
				}

				for (size_t id = 0; id < b_faces.size(); ++id) {
					auto face = b_faces[id];
					edge_to_faces_b[make_edge(face.ind_1, face.ind_2)].insert(id);
					edge_to_faces_b[make_edge(face.ind_2, face.ind_3)].insert(id);
					edge_to_faces_b[make_edge(face.ind_3, face.ind_1)].insert(id);
				}

				for (size_t id = 0; id < vrt_arr_size; ++id) {
					auto face_a = ind_arr[id * 2 + 0];
					auto face_b = ind_arr[id * 2 + 1];
					faces_from_mesh_a.insert({ face_a, a_faces[face_a] });
					faces_from_mesh_b.insert({ face_b, b_faces[face_b] });
				}

				// Collect inner points of mesh B in mesh A
				for (auto [id, face] : faces_from_mesh_b) {
					vec3_base v0 = b_vertexes[face.ind_1];
					vec3_base v1 = b_vertexes[face.ind_2];
					vec3_base v2 = b_vertexes[face.ind_3];

					if (check_is_point_in_mesh_gpu(cl_m1, v0)) new_vertexes_b.insert(face.ind_1);
					if (check_is_point_in_mesh_gpu(cl_m1, v1)) new_vertexes_b.insert(face.ind_2);
					if (check_is_point_in_mesh_gpu(cl_m1, v2)) new_vertexes_b.insert(face.ind_3);
				}

				// Collect inner points of mesh A in mesh B
				for (auto [id, face] : faces_from_mesh_a) {
					vec3_base v0 = a_vertexes[face.ind_1];
					vec3_base v1 = a_vertexes[face.ind_2];
					vec3_base v2 = a_vertexes[face.ind_3];

					if (check_is_point_in_mesh_gpu(cl_m2, v0)) new_vertexes_a.insert(face.ind_1);
					if (check_is_point_in_mesh_gpu(cl_m2, v1)) new_vertexes_a.insert(face.ind_2);
					if (check_is_point_in_mesh_gpu(cl_m2, v2)) new_vertexes_a.insert(face.ind_3);
				}

				// Search other nearest points
				add_inner_vertexes(b_vertexes, b_faces, cl_m1, edge_to_faces_b, faces_from_mesh_b, new_vertexes_b);
				add_inner_vertexes(a_vertexes, a_faces, cl_m2, edge_to_faces_a, faces_from_mesh_a, new_vertexes_a);

				// Fill data
				{
					uset<vec3_base, ecg_hash_func> all_vertexes;
					for (auto id : new_vertexes_a) all_vertexes.insert(a_vertexes[id]);
					for (auto id : new_vertexes_b) all_vertexes.insert(b_vertexes[id]);
					for (auto int_vertex : intersection_points) all_vertexes.insert(int_vertex);
					std::vector<vec3_base> temp_all_vertexes(all_vertexes.begin(), all_vertexes.end());

					result = allocate_array<vec3_base>(all_vertexes.size());
					safe_copy_to_arr(result, temp_all_vertexes);
				}
			}
		}
		catch (const std::exception& ex) {
			spdlog::error(ex.what());
			result = empty;
		}
		catch (...) {
			spdlog::error(g_unknown_error);
			result = empty;
		}

		return result;
	}

	ecg_internal_mesh_t compute_intersection(const ecg_mesh_t* m1, const ecg_mesh_t* m2, ecg_status* status) {
		auto& mem_inst = ecg_mem::get_instance();
		ecg_internal_mesh_t result;
		ecg_array_t empty;
		empty.arr_ptr = nullptr;
		empty.arr_size = 0;
		empty.handler = 0;

		auto int_set_v1 = get_intersection_points(m1, m2, status);
		auto vrt = add_interior_intersection_points(m1, m2, &int_set_v1, status);

		mem_inst.delete_memory(int_set_v1.ind.handler);
		mem_inst.delete_memory(int_set_v1.vrt.handler);
		
		result.indexes = empty;
		result.vertexes = vrt;
		return result;
	}
}