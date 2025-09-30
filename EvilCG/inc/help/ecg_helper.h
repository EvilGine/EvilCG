#ifndef ECG_HELPER_H
#define ECG_HELPER_H
#include <help/ecg_overloads.h>
#include <help/ecg_hasher.h>
#include <help/ecg_geom.h>
#include <help/ecg_math.h>
#include <ecg_global.h>

namespace ecg {
	/// <summary>
	/// 
	/// </summary>
	/// <param name="vertexes"></param>
	/// <param name="indexes"></param>
	/// <returns></returns>
	std::pair<std::vector<vec3_base>, std::vector<uint32_t>> optimize_geometry(
		const std::vector<vec3_base>& vertexes, 
		const std::vector<uint32_t>& indexes
	);
	
	/// <summary>
	/// 
	/// </summary>
	/// <param name="vertexes"></param>
	/// <param name="indexes"></param>
	/// <returns></returns>
	std::pair<std::vector<vec3_base>, std::vector<uint32_t>> optimize_intersection(
		const std::vector<vec3_base>& vertices,
		const std::vector<uint32_t>& indices
	);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="vertices"></param>
	/// <param name="indices"></param>
	/// <returns></returns>
	std::vector<vec3_base> normalize_mesh(const std::span<vec3_base>& vertices);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="p"></param>
	/// <param name="dir"></param>
	/// <param name="s0"></param>
	/// <param name="s1"></param>
	/// <param name="s2"></param>
	/// <returns></returns>
	bool ray_hits_triangle(
		const vec3_base& ray_origin, const vec3_base& ray_dir,
		const vec3_base& v0, const vec3_base& v1, const vec3_base& v2
	);
	
	/// <summary>
	/// 
	/// </summary>
	/// <param name="p"></param>
	/// <param name="s0"></param>
	/// <param name="s1"></param>
	/// <param name="s2"></param>
	/// <returns></returns>
	bool point_in_triangle(const vec3_base& p, const vec3_base& a, const vec3_base& b, const vec3_base& c);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <returns></returns>
	std::pair<uint32_t, uint32_t> make_edge(uint32_t a, uint32_t b);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <returns></returns>
	edge_t make_edge_struct(uint32_t a, uint32_t b);
}

#endif