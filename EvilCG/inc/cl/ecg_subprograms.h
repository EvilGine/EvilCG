#ifndef ECG_SUBPROGRAMS_H
#define ECG_SUBPROGRAMS_H
#include <ecg_global.h>

namespace ecg {
	#define NAME_OF(VAR) "\n"#VAR

	/// <summary>
	/// Namespace with structures for OpenCL kernel codes
	/// </summary>
	namespace cl_structs {
		const std::string face_struct =
			NAME_OF(
				struct face_t {
					\n
						uint32_t id0; \n
						uint32_t id1; \n
						uint32_t id2; \n
				}; \n
			);

		const std::string edge_struct =
			NAME_OF(
				struct edge_t { \n
					uint32_t id0; \n
					uint32_t id1; \n
				}; \n
			);

		const std::string init_edge_func =
			NAME_OF(
				void init_edge(struct edge_t* edge) { \n
					edge->id0 = 0; \n
					edge->id1 = 0; \n
				} \n\n
			);

		const std::string init_face_func =
			NAME_OF(
				void init_face(struct face_t* face) { \n
					face->id0 = 0; \n
					face->id1 = 0; \n
					face->id2 = 0; \n
				} \n\n
			);

		const std::string get_face_func =
			NAME_OF(
				struct face_t get_face(__global uint32_t * indexes, uint32_t id) {
					struct face_t result;
					result.id0 = indexes[id * 3 + 0];
					result.id1 = indexes[id * 3 + 1];
					result.id2 = indexes[id * 3 + 2];
					return  result;
				}
			);

		const std::string is_face_null_func =
			NAME_OF(
				bool is_face_null(struct face_t face) { \n
					return face.id0 == 0 && face.id1 == 0 && face.id2 == 0; \n
				} \n\n
			);

		const std::string is_face_contains_vertex_func =
			NAME_OF(
				bool is_face_contains_vertex(struct face_t face, uint32_t vert) { \n
					bool res = \n
						face.id0 == vert || \n
						face.id1 == vert || \n
						face.id2 == vert; \n
					return res; \n
				}
			);

		const std::string is_face_contains_edge_func =
			NAME_OF(
				bool is_face_contains_edge(struct face_t face, struct edge_t edge) { \n
					bool contains_edge = \n
						(edge.id0 == face.id0 && edge.id1 == face.id1) || \n
						(edge.id0 == face.id1 && edge.id1 == face.id2) || \n
						(edge.id0 == face.id2 && edge.id1 == face.id0) || \n
						(edge.id1 == face.id0 && edge.id0 == face.id1) || \n
						(edge.id1 == face.id1 && edge.id0 == face.id2) || \n
						(edge.id1 == face.id2 && edge.id0 == face.id0); \n
					return contains_edge; \n
				} \n\n
			);

		const std::string is_edges_equal_func =
			NAME_OF(
				bool is_edges_equal(struct edge_t lhs, struct edge_t rhs) {
					return 
						(lhs.id0 == rhs.id0 && lhs.id1 == rhs.id1) ||
						(lhs.id0 == rhs.id1 && lhs.id1 == rhs.id0);
				}
			);
	}

	const std::string define_epsilon = "\n#define EPSILON 1e-6\n";
	const std::string typedef_uint32_t = "\ntypedef unsigned int uint32_t;\n";
	const std::string enable_atomics_def = "\n#pragma OPENCL EXTENSION cl_khr_int64_extended_atomics : enable\n";

	const std::string mul_mat_vec =
		NAME_OF(
			inline float3 mul_mat3_vec3(volatile __global float* mat3, float3 vec) {
				float3 result;

				result.x = mat3[0] * vec.x + mat3[1] * vec.y + mat3[2] * vec.z;
				result.y = mat3[3] * vec.x + mat3[4] * vec.y + mat3[5] * vec.z;
				result.z = mat3[6] * vec.x + mat3[7] * vec.y + mat3[8] * vec.z;

				return result;
			}

			inline float3 mul_mat3_vec3_l(volatile __local float* mat3, float3 vec) {
				float3 result;

				result.x = mat3[0] * vec.x + mat3[1] * vec.y + mat3[2] * vec.z;
				result.y = mat3[3] * vec.x + mat3[4] * vec.y + mat3[5] * vec.z;
				result.z = mat3[6] * vec.x + mat3[7] * vec.y + mat3[8] * vec.z;

				return result;
			}
		);

	const std::string atomic_min_f =
		NAME_OF(
			inline void atomic_min_f(volatile __global float* source, const float new_value) {
				union {
					unsigned int int_value;
					float float_value;
				} old_union; 

				union {
					unsigned int int_value;
					float float_value;
				} new_union;

				do {
					old_union.float_value = *source;
					if (new_value >= old_union.float_value)
						return;

					new_union.float_value = new_value;
				} while (atomic_cmpxchg((__global unsigned int*) source, old_union.int_value, new_union.int_value) != old_union.int_value);
			} \n\n

			inline void atomic_min_fl(volatile __local float* source, const float new_value) {
				union {
					unsigned int int_value;
					float float_value;
				} old_union;

				union {
					unsigned int int_value;
					float float_value;
				} new_union;

				do {
					old_union.float_value = *source;
					if (new_value >= old_union.float_value)
						return;

					new_union.float_value = new_value;
				} while (atomic_cmpxchg((__local unsigned int*) source, old_union.int_value, new_union.int_value) != old_union.int_value);
			}
		);

	const std::string atomic_max_f = 
    NAME_OF(
        inline void atomic_max_f(volatile __global float* source, const float new_value) {\n
			union {
				unsigned int int_value;
				float float_value;
			} old_union;

			union {
				unsigned int int_value;
				float float_value;
			} new_union;

            do {\n
                old_union.float_value = *source;\n
                if (new_value <= old_union.float_value)\n
                    return;\n\n

                new_union.float_value = new_value;\n
            } while (atomic_cmpxchg((__global unsigned int*) source, old_union.int_value, new_union.int_value) != old_union.int_value);\n
        }\n\n

        inline void atomic_max_fl(volatile __local float* source, const float new_value) {\n
            union {
				unsigned int int_value;
				float float_value;
			} old_union;

			union {
				unsigned int int_value;
				float float_value;
			} new_union;

            do {\n
                old_union.float_value = *source;\n
                if (new_value <= old_union.float_value)\n
                    return;\n\n

                new_union.float_value = new_value;\n
            } while (atomic_cmpxchg((__local unsigned int*) source, old_union.int_value, new_union.int_value) != old_union.int_value);\n
        }\n
    );

	const std::string atomic_add_f =
		NAME_OF(
			void __attribute__((always_inline)) atomic_add_f(volatile __global float* source, const float val) {
				union {
					uint  u32;
					float f32;
				} next;
				union {
					uint  u32;
					float f32;
				} expected;
				union {
					uint  u32;
					float f32;
				} current;
				current.f32 = *source;
				do {
					next.f32 = (expected.f32 = current.f32) + val;
					current.u32 = atomic_cmpxchg((volatile global uint*)source, expected.u32, next.u32);
				} while (current.u32 != expected.u32);
			}

			void __attribute__((always_inline)) atomic_add_fl(volatile __local float* source, const float val) {
				union {
					uint  u32;
					float f32;
				} next;
				union {
					uint  u32;
					float f32;
				} expected;
				union {
					uint  u32;
					float f32;
				} current;
				current.f32 = *source;
				do {
					next.f32 = (expected.f32 = current.f32) + val;
					current.u32 = atomic_cmpxchg((volatile __local uint*)source, expected.u32, next.u32);
				} while (current.u32 != expected.u32);
			}
		);

	const std::string atomic_sub_f =
		NAME_OF(
			inline void atomic_sub_f(volatile __global float* source, const float sub_value) {
				union {
					unsigned int int_value;
					float float_value;
				} if_union;

				if_union.float_value = *source - add_val;
				atomic_xchg((__global int*) source, if_union.int_value);
			}
		);

	const std::string get_vert_len =
		NAME_OF(
			float get_len_fl3(float3 vert) {
				return sqrt(vert.x * vert.x + vert.y * vert.y + vert.z * vert.z);
			}

			float get_len_fl4(float4 vert) {
				return sqrt(vert.x * vert.x + vert.y * vert.y + vert.z * vert.z + vert.w * vert.w);
			}
		);

	const std::string get_vertex =
		NAME_OF(
			float3 get_vertex(int id, volatile __global float* vertexes, int vert_size) { \n
				return (float3)( \n
					vertexes[id * vert_size + 0], \n
					vertexes[id * vert_size + 1], \n
					vertexes[id * vert_size + 2] \n
				); \n
			} \n
		);

	const std::string get_face_normal =
		NAME_OF(
			float3 get_face_normal(float3 s0, float3 s1, float3 s2) { \n
				return cross_product(s1 - s0, s2 - s0); \n
			} \n\n
		);

	const std::string is_point_in_triangle_func =
		NAME_OF(
			bool is_point_in_triangle(float3 P, float3 S0, float3 S1, float3 S2) {
				float3 v0 = S1 - S0;
				float3 v1 = S2 - S0;
				float3 v2 = P - S0;

				float d00 = dot(v0, v0); float d01 = dot(v0, v1);
				float d11 = dot(v1, v1); float d20 = dot(v2, v0);
				float d21 = dot(v2, v1);

				float denom = d00 * d11 - d01 * d01;
				if (denom == 0.0f) return false;

				float v = (d11 * d20 - d01 * d21) / denom;
				float w = (d00 * d21 - d01 * d20) / denom;
				float u = 1.0f - v - w;

				return (u >= 0.0f && v >= 0.0f && w >= 0.0f);
			}
		);

	const std::string get_intersection_point_func =
		NAME_OF(
			bool get_intersection_point(
				float3 p0, float3 p1, float3 s0, float3 s1, float3 s2,
				float3 * intersection_point, float* t_parameter) {
				bool is_intersected = false;
				float t_param = -1.0f;
				float3 pi_point;

				float3 line_dir = p1 - p0;
				float3 surf_norm = get_face_normal(s0, s1, s2);
				float denom = dot(surf_norm, line_dir);
				float d = -dot(surf_norm, s0);

				if (denom == 0.0f) {
					if (dot(surf_norm, p0) + d == 0 && dot(surf_norm, p1) + d == 0) {
						if (is_point_in_triangle(p0, s0, s1, s2)) {
							is_intersected = true;
							pi_point = p0;
						}
						else if (is_point_in_triangle(p1, s0, s1, s2)) {
							is_intersected = true;
							pi_point = p1;
						}
						else {
							is_intersected = true;
							pi_point = (p0 + p1) / 2.0f;
						}
					}
					else {
						return false;
					}
				}
				else {
					t_param = -(d + dot(surf_norm, p0)) / denom;
					pi_point = p0 + line_dir * t_param;
					is_intersected = is_point_in_triangle(pi_point, s0, s1, s2);
				}

				if (t_parameter != NULL)
					*t_parameter = t_param;
				if (intersection_point != NULL && is_intersected)
					*intersection_point = pi_point;

				return is_intersected;
			}
		);

	const std::string is_vertex_of_triangle_func =
		NAME_OF(
			bool is_vertex_of_triangle(float3 s0, float3 s1, float3 s2, float3 pt) {
				return
					s0.x == pt.x && s0.y == pt.y && s0.z == pt.z ||
					s1.x == pt.x && s1.y == pt.y && s1.z == pt.z ||
					s2.x == pt.x && s2.y == pt.y && s2.z == pt.z;
			}
		);

	const std::string has_shared_vertex_func =
		NAME_OF(
			bool has_shared_vertex(const face_t & f1, const face_t & f2) {
				return f1.id0 == f2.id0 || f1.id0 == f2.id1 || f1.id0 == f2.id2 ||
					f1.id1 == f2.id0 || f1.id1 == f2.id1 || f1.id1 == f2.id2 ||
					f1.id2 == f2.id0 || f1.id2 == f2.id1 || f1.id2 == f2.id2;
			}
		);

	const std::string cross_product =
		NAME_OF(
			float3 cross_product(float3 a, float3 b) { \n
				return (float3)( \n
					a.y * b.z - a.z * b.y, \n
					a.z * b.x - a.x * b.z, \n
					a.x * b.y - a.y * b.x  \n
				); \n\n
			}
		);

	const std::string calculate_surf_area =
		cross_product +
		get_vert_len +
		get_vertex +
		NAME_OF(
			float calculate_surf_area(
				volatile __global float* vertexes, 
				int ind_1, int ind_2, int ind_3,
				int vert_size
			) { \n
				float3 v1 = get_vertex(ind_1, vertexes, vert_size); // A \n
				float3 v2 = get_vertex(ind_2, vertexes, vert_size); // B \n
				float3 v3 = get_vertex(ind_3, vertexes, vert_size); // C \n
				
				float3 ab = v2 - v1;
				float3 ac = v3 - v1;

				float3 cross = cross_product(ab, ac);
				return get_len_fl3(cross) / 2.0f;
			}
		);

	const std::string to_mesh_name = "to_mesh";
	const std::string to_mesh_code =
		enable_atomics_def +
		NAME_OF(
			__kernel void to_mesh(
				__global float* input_vert, int input_vert_size,
				__global float* output_vert, int output_vert_size)
			{
				const int gid = get_global_id(0);
				if (!((input_vert_size == 3 && output_vert_size == 4) ||
					(input_vert_size == 4 && output_vert_size == 3)))
					return;

				output_vert[gid * output_vert_size + 0] = input_vert[gid * input_vert_size + 0];
				output_vert[gid * output_vert_size + 1] = input_vert[gid * input_vert_size + 1];
				output_vert[gid * output_vert_size + 2] = input_vert[gid * input_vert_size + 2];

				if (output_vert_size == 4) output_vert[gid * output_vert_size + 3] = 0.0f;
			}
		);

	const std::string force_compare_name = "force_compare_meshes";
	const std::string force_compare_code = 
		"#define FULL_EQUAL 1\n"
		"#define NOT_EQUAL -1\n" + 
		enable_atomics_def +
		get_vertex + 
		NAME_OF(
			__kernel void force_compare_meshes(
				int vert_size, 
				__global float* m1_v, __global float* m2_v, 
				__global int* is_equal) {
				const int gid = get_global_id(0);

				float3 v1 = get_vertex(gid, m1_v, vert_size);
				float3 v2 = get_vertex(gid, m2_v, vert_size);

				float3 diff = (float3)
					(
						fabs(v1.x - v2.x),
						fabs(v1.y - v2.y),
						fabs(v1.z - v2.z)
					);

				if (diff.x > 1E-06f || diff.y > 1E-06f || diff.z > 1E-06f) {
					atomic_min(&is_equal[0], NOT_EQUAL);
				}
				else {
					atomic_min(&is_equal[0], FULL_EQUAL);
				}
			}
		);

	const std::string compare_name = "compare_meshes";
	const std::string compare_code =
		"#define FULL_EQUAL 1\n"
		"#define NOT_EQUAL -1\n" +
		enable_atomics_def +
		get_vertex + 
		NAME_OF(
			__kernel void compare_meshes(
				int vert_size, float threshold,
				__global float* m1_v, __global float* m2_v,
				__global int* is_equal) {
				const int gid = get_global_id(0);

				float3 v1 = get_vertex(gid, m1_v, vert_size);
				float3 v2 = get_vertex(gid, m2_v, vert_size);

				float3 diff = (float3)
					(
						fabs(v1.x - v2.x),
						fabs(v1.y - v2.y),
						fabs(v1.z - v2.z)
					);
				float len = length(diff);

				if (len > threshold) {
					atomic_min(&is_equal[0], NOT_EQUAL);
				}
				else {
					atomic_min(&is_equal[0], FULL_EQUAL);
				}
			}
		);

	const std::string compute_aabb_name = "compute_aabb";
	const std::string compute_aabb_code =
		enable_atomics_def +
		atomic_min_f +
		atomic_max_f +
		get_vertex +
		NAME_OF(
			__kernel void compute_aabb(
				__global float* mesh_vertexes, int vertex_size,
				__global float* aabb
			) {
				const int gid = get_global_id(0);
				const int lid = get_local_id(0);

				float3 vrt = get_vertex(gid, mesh_vertexes, vertex_size);

				atomic_min_f(&aabb[0], vrt.x);
				atomic_min_f(&aabb[1], vrt.y);
				atomic_min_f(&aabb[2], vrt.z);

				atomic_max_f(&aabb[3], vrt.x);
				atomic_max_f(&aabb[4], vrt.y);
				atomic_max_f(&aabb[5], vrt.z);
			}
		);

	const std::string summ_vertexes_name = "summ_vertexes";
	const std::string summ_vertexes_code =
		get_vertex +
		NAME_OF(
			__kernel void summ_vertexes(
				int vert_arr_len, int vert_size,
				__global float* vertexes,
				__global float* local_acc,
				__global float* res)
			{
				const int group_size = get_local_size(0);
				const int group_id = get_group_id(0);
				const int gid = get_global_id(0);
				const int lid = get_local_id(0);

				if (gid >= vert_arr_len) return;

				const int offset = group_id * group_size;
				float3 v1 = gid * 2 < vert_arr_len ? get_vertex(gid * 2, vertexes, vert_size) : (float3)(0);
				float3 v2 = gid * 2 + 1 < vert_arr_len ? get_vertex(gid * 2 + 1, vertexes, vert_size) : (float3)(0);
				float3 summ = v1 + v2;

				local_acc[(offset + lid) * vert_size + 0] = summ.x;
				local_acc[(offset + lid) * vert_size + 1] = summ.y;
				local_acc[(offset + lid) * vert_size + 2] = summ.z;

				barrier(CLK_LOCAL_MEM_FENCE);

				for (int iter = 2; iter <= group_size; iter <<= 1) {
					if (lid % iter == 0) {
						float3 temp_vert = get_vertex(offset + lid + (iter >> 1), local_acc, vert_size);
						local_acc[(offset + lid) * vert_size + 0] += temp_vert.x;
						local_acc[(offset + lid) * vert_size + 1] += temp_vert.y;
						local_acc[(offset + lid) * vert_size + 2] += temp_vert.z;
					}
					barrier(CLK_LOCAL_MEM_FENCE);
				}

				if (lid == 0) {
					res[group_id * vert_size + 0] = local_acc[offset * vert_size + 0];
					res[group_id * vert_size + 1] = local_acc[offset * vert_size + 1];
					res[group_id * vert_size + 2] = local_acc[offset * vert_size + 2];
				}
				barrier(CLK_LOCAL_MEM_FENCE);
			}
		);

	const std::string find_farthest_vertex_name = "find_farthest_vertex";
	const std::string find_farthest_vertex_code =
		enable_atomics_def +
		get_vert_len +
		get_vertex +
		NAME_OF(
			__kernel void find_farthest_vertex(
				__global float* vertexes, int vertex_size,
				__global float* result_obb)
			{
				const int gid = get_global_id(0);
				float3 main_vert = get_vertex(gid, vertexes, vert_size);
				float max_len = -1.0f;
				float3 temp_v;
				float3 bb[4];

				for (int id = 0; id < vertexes_cnt; ++id) {
					float3 vert = get_vertex(id, vertexes, vert_size);
					float len = get_len(main_vert - vert);

					if (len > max_len) {
						temp_v = vert;
						max_len = len;
					}
				}
			}
		);

	const std::string compute_cov_mat_name = "compute_cov";
	const std::string compute_cov_mat_code =
		atomic_add_f +
		NAME_OF(
			void update_local_mat(__local float* mat, float3 vrt, float4 center) { \n
				atomic_add_fl(&mat[0 * 3 + 0], (vrt.x - center.x) * (vrt.x - center.x)); \n
				atomic_add_fl(&mat[0 * 3 + 1], (vrt.x - center.x) * (vrt.x - center.y)); \n
				atomic_add_fl(&mat[0 * 3 + 2], (vrt.x - center.x) * (vrt.x - center.z)); \n

				atomic_add_fl(&mat[1 * 3 + 0], (vrt.y - center.y) * (vrt.x - center.x)); \n
				atomic_add_fl(&mat[1 * 3 + 1], (vrt.y - center.y) * (vrt.x - center.y)); \n
				atomic_add_fl(&mat[1 * 3 + 2], (vrt.y - center.y) * (vrt.x - center.z)); \n

				atomic_add_fl(&mat[2 * 3 + 0], (vrt.z - center.z) * (vrt.x - center.x)); \n
				atomic_add_fl(&mat[2 * 3 + 1], (vrt.z - center.z) * (vrt.x - center.y)); \n
				atomic_add_fl(&mat[2 * 3 + 2], (vrt.z - center.z) * (vrt.x - center.z)); \n
			}

			__kernel void compute_cov(\n
				__global float* vertexes, int vertex_size, float4 center, \n
				__global float* cov_mat) { \n
				const int gid = get_global_id(0); \n
				const int lid = get_local_id(0); \n
				float3 vrt = get_vertex(gid, vertexes, vertex_size); \n
				__local float local_mat[9];

				if (lid == 0) {
					for(int i = 0; i < 9; ++i) \n
						local_mat[i] = 0.0f; \n
				} \n
				barrier(CLK_LOCAL_MEM_FENCE); \n

				update_local_mat(local_mat, vrt, center); \n
				barrier(CLK_LOCAL_MEM_FENCE); \n

				if (lid == 0) {
					atomic_add_f(&cov_mat[0 * 3 + 0], local_mat[0]); \n
					atomic_add_f(&cov_mat[0 * 3 + 1], local_mat[1]); \n
					atomic_add_f(&cov_mat[0 * 3 + 2], local_mat[2]); \n

					atomic_add_f(&cov_mat[1 * 3 + 0], local_mat[3]); \n
					atomic_add_f(&cov_mat[1 * 3 + 1], local_mat[4]); \n
					atomic_add_f(&cov_mat[1 * 3 + 2], local_mat[5]); \n

					atomic_add_f(&cov_mat[2 * 3 + 0], local_mat[6]); \n
					atomic_add_f(&cov_mat[2 * 3 + 1], local_mat[7]); \n
					atomic_add_f(&cov_mat[2 * 3 + 2], local_mat[8]); \n
				}
			}
		);

	const std::string compute_obb_name = "compute_obb";
	const std::string compute_obb_code = 
		atomic_min_f +
		atomic_max_f +
		mul_mat_vec +
		NAME_OF(
			__kernel void compute_obb(
				__global float* vertexes, int vertex_size, 
				__global float* inv_mat, float4 center,
				__global float* bounding_box) {
				const int gid = get_global_id(0); \n
				const int lid = get_local_id(0); \n
				__local float min[3]; 
				__local float max[3];

				if (lid == 0) {
					min[0] = FLT_MAX; min[1] = FLT_MAX; min[2] = FLT_MAX;
					max[0] = -FLT_MAX; max[1] = -FLT_MAX; max[2] = -FLT_MAX;
				}
				barrier(CLK_LOCAL_MEM_FENCE); \n

				float3 vrt = get_vertex(gid, vertexes, vertex_size); \n
				vrt = (float3)(
					vrt.x - center.x,
					vrt.y - center.y,
					vrt.z - center.z
				);

				float3 new_pos = mul_mat3_vec3(inv_mat, vrt);
				barrier(CLK_LOCAL_MEM_FENCE); \n

				atomic_min_fl(&min[0], new_pos.x);
				atomic_min_fl(&min[1], new_pos.y);
				atomic_min_fl(&min[2], new_pos.z);

				atomic_max_fl(&max[0], new_pos.x);
				atomic_max_fl(&max[1], new_pos.y);
				atomic_max_fl(&max[2], new_pos.z);

				barrier(CLK_LOCAL_MEM_FENCE); \n
				if (lid == 0) {
					atomic_min_f(&bounding_box[0], min[0]);
					atomic_min_f(&bounding_box[1], min[1]);
					atomic_min_f(&bounding_box[2], min[2]);

					atomic_max_f(&bounding_box[3], max[0]);
					atomic_max_f(&bounding_box[4], max[1]);
					atomic_max_f(&bounding_box[5], max[2]);
				}
			}
		);

	const std::string compute_surface_area_name = "compute_surface_area";
	const std::string compute_surface_area_code =
		enable_atomics_def +
		atomic_add_f +
		calculate_surf_area +
		NAME_OF(
			__kernel void compute_surface_area( \n
				__global float* vertexes, int vertexes_size, \n
				__global int* indexes, int indexes_size, \n
				int vert_size, __global float* surface_area
			) { \n
				const int gid = get_global_id(0); \n 
				const int lid = get_local_id(0);
				__local float local_surf_area; \n
				
				if(lid == 0) local_surf_area = 0.0f;
				barrier(CLK_LOCAL_MEM_FENCE);

				int surf_id = gid * 3;
				if (surf_id < indexes_size &&
					surf_id + 1 < indexes_size &&
					surf_id + 2 < indexes_size) {
					const int ind_1 = indexes[surf_id];
					const int ind_2 = indexes[surf_id + 1];
					const int ind_3 = indexes[surf_id + 2];

					float temp = calculate_surf_area(vertexes, ind_1, ind_2, ind_3, vert_size);
					atomic_add_fl(&local_surf_area, temp);
				}

				barrier(CLK_LOCAL_MEM_FENCE);
				if(lid == 0) atomic_add_f(surface_area, local_surf_area);
			}
		);

	const std::string is_mesh_closed_name = "is_mesh_closed";
	const std::string is_mesh_closed_code =
		typedef_uint32_t +
		cl_structs::face_struct +
		cl_structs::edge_struct +
		cl_structs::get_face_func +
		cl_structs::is_face_contains_edge_func +
		NAME_OF(
			__kernel void is_mesh_closed(
				__global uint32_t* indexes, uint32_t indexes_cnt,
				__global bool* result
			) {
				uint32_t gid = get_global_id(0);
				if (gid >= indexes_cnt - 1) return;

				struct edge_t current_edge;
				int edges_includes = 0;

				if (gid % 3 == 0 || gid % 3 == 1) {
					current_edge.id0 = indexes[gid]; 
					current_edge.id1 = indexes[gid + 1];
				}
				else if (gid % 3 == 2) {
					current_edge.id0 = indexes[gid];
					current_edge.id1 = indexes[gid - 2];
				}

				for (uint32_t face_id = 0; face_id < indexes_cnt / 3; ++face_id) {
					if (*result == false) break;
					struct face_t face = get_face(indexes, face_id);

					if (is_face_contains_edge(face, current_edge))
						++edges_includes;
				}

				if (edges_includes != 2)
					*result = false;
			}
		);

	const std::string is_mesh_vertexes_manifold_name = "is_mesh_vertexes_manifold";
	const std::string is_mesh_vertexes_manifold_code =
		typedef_uint32_t +
		cl_structs::face_struct +
		cl_structs::edge_struct +
		cl_structs::get_face_func +
		cl_structs::init_edge_func +
		cl_structs::init_face_func +
		cl_structs::is_face_null_func +
		cl_structs::is_edges_equal_func +
		cl_structs::is_face_contains_edge_func +
		cl_structs::is_face_contains_vertex_func +
		NAME_OF(
			uint32_t find_next_face( \n
				__global uint32_t * indexes, uint32_t faces_cnt, \n
				struct edge_t edge, uint32_t prev_face_id \n
			) { \n
				for (uint32_t face_id = 0; face_id < faces_cnt; ++face_id) { \n
					if (face_id == prev_face_id) continue; \n
					struct face_t face = get_face(indexes, face_id); \n
					if (is_face_contains_edge(face, edge)) \n
						return face_id; \n
				} \n
				return prev_face_id; \n
			} \n\n

			struct edge_t get_another_edge(struct face_t face, struct edge_t edge) { \n
				struct edge_t new_edge; \n
				init_edge(&new_edge); \n\n

				if (face.id0 == edge.id0) { \n
					if (face.id1 == edge.id1) { \n
						new_edge.id0 = face.id0; \n
						new_edge.id1 = face.id2; \n
					} \n
					if (face.id2 == edge.id1) { \n
						new_edge.id0 = face.id0; \n
						new_edge.id1 = face.id1; \n
					} \n
				} \n\n

				if (face.id1 == edge.id0) { \n
					if (face.id0 == edge.id1) { \n
						new_edge.id0 = face.id1; \n
						new_edge.id1 = face.id2; \n
					} \n
					if (face.id2 == edge.id1) { \n
						new_edge.id0 = face.id1; \n
						new_edge.id1 = face.id0; \n
					} \n
				} \n\n

				if (face.id2 == edge.id0) { \n
					if (face.id0 == edge.id1) { \n
						new_edge.id0 = face.id2; \n
						new_edge.id1 = face.id1; \n
					} \n\n
					if (face.id1 == edge.id1) { \n
						new_edge.id0 = face.id2; \n
						new_edge.id1 = face.id0; \n
					} \n
				} \n

				return new_edge; \n
			} \n\n

			__kernel void is_mesh_vertexes_manifold( \n
				__global uint32_t* indexes, uint32_t indexes_cnt, \n
				uint32_t vertexes_cnt, __global bool* result \n
			) { \n
				uint32_t faces_cnt = indexes_cnt / 3; \n
				uint32_t vertex_id = get_global_id(0); \n
				if(vertex_id > vertexes_cnt) return; \n\n

				uint32_t origin_face_id = 0; \n
				uint32_t prev_face_id = 0; \n
				uint32_t next_face_id = 0; \n\n

				struct face_t origin_face; \n
				struct face_t prev_face; \n
				struct face_t next_face; \n\n

				init_face(&origin_face); \n
				init_face(&prev_face); \n
				init_face(&next_face); \n\n

				struct edge_t current_edge; \n
				struct edge_t origin_edge; \n
				init_edge(&current_edge); \n
				init_edge(&origin_edge); \n\n

				// 1. Find first face and count all faces with current vertex \n
				uint32_t all_faces_with_vertex_cnt = 0; \n
				for (uint32_t face_id = 0; face_id < faces_cnt; ++face_id) { \n
					struct face_t face = get_face(indexes, face_id); \n
					if (is_face_contains_vertex(face, vertex_id)) { \n
						if (is_face_null(origin_face)) { \n
							origin_face_id = face_id; \n
							origin_face = face; \n
						} \n
						++all_faces_with_vertex_cnt; \n
					} \n
				} \n\n

				// 2. Get origin edge \n
				origin_edge.id0 = vertex_id; \n
				if (origin_face.id0 == vertex_id) origin_edge.id1 = origin_face.id1; \n
				if (origin_face.id1 == vertex_id) origin_edge.id1 = origin_face.id2; \n
				if (origin_face.id2 == vertex_id) origin_edge.id1 = origin_face.id0; \n\n

				// 3. Search next edge \n
				prev_face_id = origin_face_id; \n
				next_face_id = find_next_face(indexes, faces_cnt, origin_edge, prev_face_id); \n
				if (next_face_id == prev_face_id) { \n
					*result = false; \n
					return; \n
				} \n\n

				next_face = get_face(indexes, next_face_id); \n
				current_edge = get_another_edge(next_face, origin_edge); \n\n
				uint32_t iter_counter = 1; \n
				while (!is_edges_equal(origin_edge, current_edge) && iter_counter < all_faces_with_vertex_cnt) { \n
					if (*result == false) return; \n
					prev_face_id = next_face_id; \n

					next_face_id = find_next_face(indexes, faces_cnt, current_edge, prev_face_id); \n
					if (next_face_id == prev_face_id) { \n
						*result = false; \n
						return; \n
					} \n

					next_face = get_face(indexes, next_face_id); \n
					current_edge = get_another_edge(next_face, current_edge); \n
					++iter_counter; \n
				} \n\n

				if (iter_counter != all_faces_with_vertex_cnt) {\n
					*result = false; \n
					return;
				}\n\n

				if(!is_edges_equal(origin_edge, current_edge)) { \n
					*result = false; \n
					return; \n
				} \n
			} \n
		);

	const std::string is_mesh_self_intersected_name = "is_mesh_self_intersected";
	const std::string is_mesh_self_intersected_code =
		typedef_uint32_t +
		cross_product +
		get_vertex +
		get_face_normal +
		cl_structs::face_struct +
		cl_structs::edge_struct +
		cl_structs::get_face_func +
		cl_structs::is_face_contains_edge_func +
		cl_structs::is_face_contains_vertex_func +
		is_vertex_of_triangle_func +
		is_point_in_triangle_func +
		get_intersection_point_func +
		NAME_OF(
			__kernel void is_mesh_self_intersected(
				__global float* vertexes, uint32_t vertexes_cnt,
				__global uint32_t * indexes, uint32_t indexes_cnt,
				int vrt_size, __global bool* is_self_intersected
			) {
				uint32_t faces_cnt = indexes_cnt / 3;
				uint32_t face_id = get_global_id(0);
				if (face_id >= faces_cnt) return;
				if (*is_self_intersected) return;

				struct face_t curr_face = get_face(indexes, face_id);
				float3 curr_v0 = get_vertex(curr_face.id0, vertexes, vrt_size);
				float3 curr_v1 = get_vertex(curr_face.id1, vertexes, vrt_size);
				float3 curr_v2 = get_vertex(curr_face.id2, vertexes, vrt_size);

				for (uint32_t id = 0; id < faces_cnt; ++id) {
					if (id == face_id) continue;
					if (*is_self_intersected) return;

					struct face_t face = get_face(indexes, id);
					float3 v0 = get_vertex(face.id0, vertexes, vrt_size);
					float3 v1 = get_vertex(face.id1, vertexes, vrt_size);
					float3 v2 = get_vertex(face.id2, vertexes, vrt_size);

					// TODO: Add check that intersection point not shared point between faces
					float t_param[6];
					bool check_result[6];
					float3 intersection_point[6];

					check_result[0] = get_intersection_point(curr_v0, curr_v1, v0, v1, v2, &intersection_point[0], &t_param[0]) & !is_vertex_of_triangle(v0, v1, v2, intersection_point[0]);
					check_result[1] = get_intersection_point(curr_v1, curr_v2, v0, v1, v2, &intersection_point[1], &t_param[1]) & !is_vertex_of_triangle(v0, v1, v2, intersection_point[1]);
					check_result[2] = get_intersection_point(curr_v2, curr_v0, v0, v1, v2, &intersection_point[2], &t_param[2]) & !is_vertex_of_triangle(v0, v1, v2, intersection_point[2]);
					check_result[3] = get_intersection_point(v0, v1, curr_v0, curr_v1, curr_v2, &intersection_point[3], &t_param[3]) & !is_vertex_of_triangle(v0, v1, v2, intersection_point[3]);
					check_result[4] = get_intersection_point(v1, v2, curr_v0, curr_v1, curr_v2, &intersection_point[4], &t_param[4]) & !is_vertex_of_triangle(v0, v1, v2, intersection_point[4]);
					check_result[5] = get_intersection_point(v2, v0, curr_v0, curr_v1, curr_v2, &intersection_point[5], &t_param[5]) & !is_vertex_of_triangle(v0, v1, v2, intersection_point[5]);

					if (check_result[0] || check_result[1] || check_result[2] ||
						check_result[3] || check_result[4] || check_result[5]) {
						*is_self_intersected = true;
						return;
					}
				}
			}
		);

	const std::string triangulate_mesh_name = "triangulate_mesh";
	const std::string triangulate_mesh_code =
		typedef_uint32_t +
		NAME_OF(
			__kernel void triangulate_mesh(
				__global uint32_t * old_indexes, uint32_t old_indexes_size,
				__global uint32_t * new_indexes, uint32_t new_indexes_size,
				uint32_t old_faces_cnt, uint32_t base_num_verts
			) {
				uint32_t face_id = get_global_id(0);
				if (face_id >= old_faces_cnt) return;

				uint32_t one_face_to_multiply = base_num_verts - 2;
				uint32_t index_id = face_id * base_num_verts;
				uint32_t new_face_id = face_id * one_face_to_multiply;
				uint32_t basic_index = old_indexes[index_id];

				for (uint32_t i = 0; i < base_num_verts - 2; ++i) {
					uint32_t current_index = index_id + i + 1;

					new_indexes[new_face_id * 3 + 0] = old_indexes[basic_index];
					new_indexes[new_face_id * 3 + 1] = old_indexes[current_index];
					new_indexes[new_face_id * 3 + 2] = old_indexes[current_index + 1];

					++new_face_id;
				}
			}
		);

	const std::string compute_volume_name = "compute_volume";
	const std::string compute_volume_code =
		typedef_uint32_t +
		cl_structs::face_struct +
		cl_structs::get_face_func +
		cross_product +
		get_face_normal +
		get_vert_len +
		get_vertex +
		NAME_OF(
			__kernel void compute_volume(
				__global float* vertexes, uint32_t vertexes_size,
				__global uint32_t* indexes, uint32_t indexes_size,
				__global float* volumes, uint32_t faces_cnt
			) {
				uint32_t id = get_global_id(0);
				if (id >= faces_cnt) return;
			
				struct face_t face = get_face(indexes, id);
				float3 v0 = get_vertex(face.id0, vertexes, 3);
				float3 v1 = get_vertex(face.id1, vertexes, 3);
				float3 v2 = get_vertex(face.id2, vertexes, 3);
				float3 norm = get_face_normal(v0, v1, v2);

				float3 center = (v0 + v1 + v2) / 3.0f;
				float3 default_pt = (float3)( 0.0f, 0.0f, 0.0f );

				float3 AB = v1 - v0;
				float3 AC = v2 - v0;

				float3 temp = cross_product(AB, AC);
				float surf_area = get_len_fl3(temp) / 2.0f;

				float3 OG = center - default_pt;
				float sign = dot(OG, norm) > 0.0f ? 1.0f : -1.0f;
				volumes[id] = (1.0f / 3.0f) * surf_area * sign;
			}
		);
}

#endif