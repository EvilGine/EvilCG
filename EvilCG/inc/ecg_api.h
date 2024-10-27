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
	/// Computes the Axis-Aligned Bounding Box (AABB) for a given 3D mesh. 
	/// The AABB is a box aligned with the coordinate axes that encloses the mesh, providing 
	/// the minimum and maximum points in each axis direction.
	/// </summary>
	/// <param name="mesh">Pointer to the mesh data structure containing the vertices and geometry information.</param>
	/// <param name="status">Optional pointer to an ecg_status variable that will hold the status of the function execution. 
	/// The status will indicate success or describe any errors encountered during computation.</param>
	/// <returns>A bounding_box structure containing the minimum and maximum points of the AABB.</returns>
	ECG_API bounding_box compute_aabb(const mesh_t* mesh, ecg_status* status = nullptr);

	/// <summary>
	/// Converts a bounding_box structure to a full_bounding_box structure, adding additional information 
	/// as required. The full_bounding_box structure typically extends the bounding box details 
	/// for use in advanced bounding calculations or rendering applications.
	/// </summary>
	/// <param name="bb">A bounding_box structure representing the axis-aligned bounding box.</param>
	/// <returns>A full_bounding_box structure with extended bounding details derived from the given bounding_box.</returns>
	ECG_API full_bounding_box bb_to_full_bb(const bounding_box* bb);

	/// <summary>
	/// Computes the Oriented Bounding Box (OBB) for a given 3D mesh. 
	/// The OBB is a bounding box that is not constrained to align with the coordinate axes, 
	/// allowing it to enclose the mesh more tightly in 3D space. This can be useful for collision detection and 
	/// physics simulations where a more precise bounding shape is required.
	/// </summary>
	/// <param name="mesh">Pointer to the mesh data structure containing the vertices and geometry information.</param>
	/// <param name="status">Optional pointer to an ecg_status variable that will hold the status of the function execution. 
	/// The status will indicate success or describe any errors encountered during computation.</param>
	/// <returns>A full_bounding_box structure representing the OBB for the specified mesh.</returns>
	ECG_API full_bounding_box compute_obb(const mesh_t* mesh, ecg_status* status = nullptr);

	/// <summary>
	/// Computes the sum of all vertex positions in the specified mesh.
	/// The result is a single vec3_base vector representing the cumulative position of all vertices.
	/// This can be used for various calculations, such as finding the center of mass or balancing points 
	/// for the mesh's vertices.
	/// </summary>
	/// <param name="mesh">Pointer to the mesh data structure containing the vertices whose positions will be summed.</param>
	/// <param name="status">Optional pointer to an ecg_status variable that will hold the status of the function execution. 
	/// The status will indicate success or describe any errors encountered during computation.</param>
	/// <returns>A vec3_base vector that contains the summed vertex positions of the mesh.</returns>
	ECG_API vec3_base summ_vertexes(const mesh_t* mesh, ecg_status* status = nullptr);

	/// <summary>
	/// Calculates the geometric center (centroid) of a 3D mesh. 
	/// The center is computed as the average position of all vertices in the mesh, providing a central point 
	/// that can be useful for transformations, rotations, and physics calculations.
	/// </summary>
	/// <param name="mesh">Pointer to the mesh data structure containing the vertex information.</param>
	/// <param name="status">Optional pointer to an ecg_status variable that will hold the status of the function execution. 
	/// This status will indicate success or describe any errors encountered during computation (e.g., null pointer or empty mesh).</param>
	/// <returns>A vec3_base vector representing the calculated center point of the mesh.
	/// If the mesh has no vertices, returns a vec3_base initialized to zero or another appropriate default.</returns>
	ECG_API vec3_base get_center(const mesh_t* mesh, ecg_status* status = nullptr);
	
	// [+] Should be added next
	ECG_API float compute_surface_area(const mesh_t* mesh, ecg_status* status = nullptr);
	ECG_API std::vector<vec3_base> find_nearest_vertices(const mesh_t* mesh, const vec3_base* point, int k, ecg_status* status = nullptr);

	// [-] Not implemented
	ECG_API cmp_res compare_meshes(const mesh_t* m1, const mesh_t* m2, ecg_status* status = nullptr);

	ECG_API float compute_volume(const mesh_t* mesh, ecg_status* status = nullptr);

	ECG_API mesh_t* smooth_mesh(const mesh_t* mesh, float lambda, int iterations, ecg_status* status = nullptr);
	ECG_API mesh_t* simplify_mesh(const mesh_t* mesh, float reduction_factor, ecg_status* status = nullptr);
	ECG_API std::vector<vec3_base> compute_vertex_normals(const mesh_t* mesh, ecg_status* status = nullptr);

	ECG_API bool is_mesh_manifold(const mesh_t* mesh, ecg_status* status = nullptr);
	ECG_API bool is_mesh_closed(const mesh_t* mesh, ecg_status* status = nullptr);
}

#endif