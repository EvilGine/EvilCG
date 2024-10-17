#ifndef ECG_API_H
#define ECG_API_H
#include <help/ecg_status.h>
#include <help/ecg_geom.h>
#include <ecg_global.h>
#include <api_define.h>

#ifdef ENABLE_ECG_CL
	#include <cl/ecg_host_ctrl.h>
	#include <cl/ecg_program.h>
#endif

namespace ecg {	
	/// <summary>
	/// Compare results
	/// </summary>
	enum cmp_res {
		UNDEFINED = INT32_MAX,
		FULL_EQUAL = 1,
		NOT_EQUAL = -1,
	};

	ECG_API vec3_base get_center(mesh_t* mesh, ecg_status* status = nullptr);
	ECG_API vec3_base summ_vertexes(mesh_t* mesh, ecg_status* status = nullptr);
	ECG_API bounding_box compute_aabb(mesh_t* mesh, ecg_status* status = nullptr);
	ECG_API full_bounding_box compute_obb(mesh_t* mesh, ecg_status* status = nullptr);
	ECG_API cmp_res compare_meshes(mesh_t* m1, mesh_t* m2, ecg_status* status = nullptr);

	ECG_API float compute_volume(mesh_t* mesh, ecg_status* status = nullptr);
	ECG_API float compute_surface_area(mesh_t* mesh, ecg_status* status = nullptr);

	ECG_API std::vector<vec3_base> find_nearest_vertices(mesh_t* mesh, const vec3_base& point, int k, ecg_status* status = nullptr);

	ECG_API mesh_t* smooth_mesh(mesh_t* mesh, float lambda, int iterations, ecg_status* status = nullptr);
	ECG_API mesh_t* simplify_mesh(mesh_t* mesh, float reduction_factor, ecg_status* status = nullptr);
	ECG_API std::vector<vec3_base> compute_vertex_normals(mesh_t* mesh, ecg_status* status = nullptr);

	ECG_API bool is_mesh_manifold(mesh_t* mesh, ecg_status* status = nullptr);
	ECG_API bool is_mesh_closed(mesh_t* mesh, ecg_status* status = nullptr);
}

#endif