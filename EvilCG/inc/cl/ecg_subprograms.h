#ifndef ECG_SUBPROGRAMS_H
#define ECG_SUBPROGRAMS_H
#include <ecg_global.h>

namespace ecg {
	#define NAME_OF(VAR) "\n"#VAR

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

	const std::string cross_product =
		NAME_OF(
			float3 cross_product(float3 a, float3 b) {
				return (float3)(
					a.y * b.z - a.z * b.y,
					a.z * b.x - a.x * b.z,
					a.x * b.y - a.y * b.x
				);
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
}

#endif