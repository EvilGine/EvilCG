#ifndef ECG_API_H
#define ECG_API_H
#include <help/ecg_status.h>
#include <help/ecg_geom.h>

#include <ecg_api_define.h>
#include <ecg_global.h>

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
	/// Methods for checking self-intersection of model
	/// </summary>
	enum self_intersection_method {
		BRUTEFORCE,
		/*BHV_ALGORITHM,*/
		METHODS_COUNT
	};

	/// <summary>
	/// 
	/// </summary>
	/// <param name="mesh"></param>
	/// <returns></returns>
	ECG_API ecg_descriptor register_mesh_buffer(const ecg_mesh_t* mesh, ecg_status* status = nullptr);
	
	/// <summary>
	/// 
	/// </summary>
	/// <param name="mesh"></param>
	/// <returns></returns>
	ECG_API bool unregister_descriptor(const ecg_descriptor mesh, ecg_status* status = nullptr);

	/// <summary>
	/// Computes the Axis-Aligned Bounding Box (AABB) for a given 3D mesh. 
	/// The AABB is a box aligned with the coordinate axes that encloses the mesh, providing 
	/// the minimum and maximum points in each axis direction.
	/// </summary>
	/// <param name="mesh">Pointer to the mesh data structure containing the vertices and geometry information.</param>
	/// <param name="status">Optional pointer to an ecg_status variable that will hold the status of the function execution. 
	/// The status will indicate success or describe any errors encountered during computation.</param>
	/// <returns>A bounding_box structure containing the minimum and maximum points of the AABB.</returns>
	ECG_API bounding_box compute_aabb(const ecg_mesh_t* mesh, ecg_status* status = nullptr);
	ECG_API bounding_box compute_aabb_by_desc(const ecg_descriptor desc, ecg_status* status = nullptr);

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
	ECG_API full_bounding_box compute_obb(const ecg_mesh_t* mesh, ecg_status* status = nullptr);
	ECG_API full_bounding_box compute_obb_by_desc(const ecg_descriptor desc, ecg_status* status = nullptr); // TODO: implement later

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
	ECG_API vec3_base summ_vertexes(const ecg_mesh_t* mesh, ecg_status* status = nullptr);

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
	ECG_API vec3_base get_center(const ecg_mesh_t* mesh, ecg_status* status = nullptr);
	
	/// <summary>
	/// Computes the total surface area of a 3D mesh. The surface area is calculated by iterating over each triangle in the mesh
	/// and summing up the individual areas. Each triangle is defined by three indexed vertices in the mesh's vertex array.
	/// </summary>
	/// <param name="mesh">Pointer to the mesh data structure containing vertex and index arrays that define the mesh geometry.</param>
	/// <param name="status">Optional pointer to an ecg_status variable that will hold the status of the function execution,
	/// indicating success or describing any errors encountered during computation (e.g., null pointer, empty or invalid mesh).</param>
	/// <returns>
	/// The computed surface area as a `float` value. If the mesh is invalid or an error occurs, the function returns `-FLT_MAX`.
	/// </returns>
	ECG_API float compute_surface_area(const ecg_mesh_t* mesh, ecg_status* status = nullptr);
	ECG_API float compute_surface_area_by_desc(const ecg_descriptor desc, ecg_status* status = nullptr); // TODO: implement later
	
	/// <summary>
	/// A function for comparing two meshes with transformations
	/// </summary>
	/// <param name="m1">Pointer to the mesh data structure containing vertex and index arrays that define the mesh geometry.</param>
	/// <param name="m2">Pointer to the mesh data structure containing vertex and index arrays that define the mesh geometry.</param>
	/// <param name="status">Optional pointer to an ecg_status variable that will hold the status of the function execution,
	/// indicating success or describing any errors encountered during computation (e.g., null pointer, empty or invalid mesh).</param>
	/// <returns>
	/// Return enum value that indicates result of comparison.
	/// </returns>
	ECG_API cmp_res compare_meshes(const ecg_mesh_t* m1, const ecg_mesh_t* m2, mat3_base* delta_transform = nullptr, ecg_status* status = nullptr);
	ECG_API cmp_res compare_meshes_by_desc(const ecg_descriptor desc, const ecg_mesh_t* m2, mat3_base* delta_transform = nullptr, ecg_status* status = nullptr); // TODO: implement later

	/// <summary>
	/// Computes the covariance matrix for a given mesh, representing the variance and covariance 
	/// of its vertex positions in 3D space. This matrix is essential for analyzing the distribution 
	/// and orientation of the mesh, often used in tasks such as principal component analysis (PCA).
	/// </summary>
	/// <param name="mesh">
	/// Pointer to the mesh data structure containing vertex and index arrays that define the mesh geometry.
	/// The function requires that the mesh contains valid vertex data; otherwise, an error status is returned.
	/// </param>
	/// <param name="status">
	/// Optional pointer to an <c>ecg_status</c> variable that stores the execution status of the function. 
	/// If provided, it indicates success or describes any errors encountered (e.g., null pointer, empty or invalid mesh).
	/// If <c>nullptr</c>, the function does not return status information.
	/// </param>
	/// <returns>
	/// Returns a 3x3 covariance matrix (<c>mat3_base</c>) representing the spread of the mesh vertices 
	/// in 3D space. This matrix is symmetric and describes the variance in each axis as well as the 
	/// covariance between different axes.
	/// </returns>
	ECG_API mat3_base compute_covariance_matrix(const ecg_mesh_t* mesh, ecg_status* status = nullptr);
	ECG_API mat3_base compute_covariance_matrix_by_desc(const ecg_descriptor desc, ecg_status* status = nullptr); // TODO: implement later

	/// <summary>
	/// 
	/// </summary>
	/// <param name="mesh">
	/// Pointer to the mesh data structure containing vertex and index arrays that define the mesh geometry.
	/// The function requires that the mesh contains valid vertex data; otherwise, an error status is returned.
	/// </param>
	/// <param name="status">
	/// Optional pointer to an <c>ecg_status</c> variable that stores the execution status of the function. 
	/// If provided, it indicates success or describes any errors encountered (e.g., null pointer, empty or invalid mesh).
	/// If <c>nullptr</c>, the function does not return status information.
	/// </param>
	/// <returns></returns>
	ECG_API bool is_mesh_closed(const ecg_mesh_t* mesh, ecg_status* status = nullptr);
	ECG_API bool is_mesh_closed_by_desc(const ecg_descriptor desc, ecg_status* status = nullptr); // TODO: implement later

	/// <summary>
	/// 
	/// </summary>
	/// <param name="mesh"></param>
	/// <param name="status"></param>
	/// <returns></returns>
	ECG_API bool is_mesh_manifold(const ecg_mesh_t* mesh, ecg_status* status = nullptr);
	ECG_API bool is_mesh_manifold_by_desc(const ecg_descriptor desc, ecg_status* status = nullptr); // TODO: implement later

	/// <summary>
	/// 
	/// </summary>
	/// <param name="mesh"></param>
	/// <param name="status"></param>
	/// <returns></returns>
	ECG_API bool is_mesh_self_intersected(const ecg_mesh_t* mesh, self_intersection_method method, ecg_status* status = nullptr);
	ECG_API bool is_mesh_self_intersected_by_desc(const ecg_descriptor desc, self_intersection_method method, ecg_status* status = nullptr); // TODO: implement later

	/// <summary>
	/// 
	/// </summary>
	/// <param name="mesh"></param>
	/// <param name="base_num_vert"></param>
	/// <param name="status"></param>
	/// <returns></returns>
	ECG_API ecg_array_t triangulate_mesh(const ecg_mesh_t* mesh, int base_num_vert, ecg_status* status = nullptr);
	ECG_API ecg_array_t triangulate_mesh_by_desc(const ecg_descriptor desc, int base_num_vert, ecg_status* status = nullptr); // TODO: implement later
	
	/// <summary>
	/// 
	/// </summary>
	/// <param name="mesh"></param>
	/// <param name="status"></param>
	/// <returns></returns>
	ECG_API float compute_volume(const ecg_mesh_t* mesh, ecg_status* status = nullptr);
	ECG_API float compute_volume_by_desc(const ecg_descriptor desc, ecg_status* status = nullptr);

	// [-] Not implemented
	ECG_API ecg_mesh_t* smooth_mesh(const ecg_mesh_t* mesh, float lambda, int iterations, ecg_status* status = nullptr);
	ECG_API ecg_mesh_t* simplify_mesh(const ecg_mesh_t* mesh, float reduction_factor, ecg_status* status = nullptr);
	ECG_API ecg_array_t compute_vertex_normals(const ecg_mesh_t* mesh, ecg_status* status = nullptr);
	ECG_API ecg_array_t compute_faces_normals(const ecg_mesh_t* mesh, ecg_status* status = nullptr);
}

#endif