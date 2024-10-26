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

	/// <summary>
	/// Compute Axis-Aligned Bounding Box for mesh
	/// </summary>
	/// <param name="mesh">Pointer to mesh</param>
	/// <param name="status">Pointer to function status</param>
	/// <returns></returns>
	ECG_API bounding_box compute_aabb(const mesh_t* mesh, ecg_status* status = nullptr);

	/// <summary>
	/// Convert bounding box structure to full_bounding_box struct
	/// </summary>
	/// <param name="bb">Default Bounding Box</param>
	/// <returns></returns>
	ECG_API full_bounding_box bb_to_full_bb(const bounding_box* bb);

	/// <summary>
	/// Compute Oriented Bounding Box for mesh
	/// </summary>
	/// <param name="mesh">Pointer to mesh</param>
	/// <param name="status">Pointer to function status</param>
	/// <returns></returns>
	ECG_API full_bounding_box compute_obb(const mesh_t* mesh, ecg_status* status = nullptr);

	// [?] Half Implemented
	ECG_API vec3_base get_center(const mesh_t* mesh, ecg_status* status = nullptr);

	// [+] Should be added next
	ECG_API float compute_surface_area(const mesh_t* mesh, ecg_status* status = nullptr);
	ECG_API std::vector<vec3_base> find_nearest_vertices(const mesh_t* mesh, const vec3_base* point, int k, ecg_status* status = nullptr);

	// [-] Not implemented
	ECG_API vec3_base summ_vertexes(const mesh_t* mesh, ecg_status* status = nullptr);
	ECG_API cmp_res compare_meshes(const mesh_t* m1, const mesh_t* m2, ecg_status* status = nullptr);

	ECG_API float compute_volume(const mesh_t* mesh, ecg_status* status = nullptr);

	ECG_API mesh_t* smooth_mesh(const mesh_t* mesh, float lambda, int iterations, ecg_status* status = nullptr);
	ECG_API mesh_t* simplify_mesh(const mesh_t* mesh, float reduction_factor, ecg_status* status = nullptr);
	ECG_API std::vector<vec3_base> compute_vertex_normals(const mesh_t* mesh, ecg_status* status = nullptr);

	ECG_API bool is_mesh_manifold(const mesh_t* mesh, ecg_status* status = nullptr);
	ECG_API bool is_mesh_closed(const mesh_t* mesh, ecg_status* status = nullptr);
}

#endif