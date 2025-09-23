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
	std::pair<std::vector<vec3_base>, std::vector<uint32_t>> optimize_geom_data(
		const std::vector<vec3_base>& vertexes, const std::vector<uint32_t>& indexes
	);
	
	/// <summary>
	/// 
	/// </summary>
	/// <param name="vertexes"></param>
	/// <param name="indexes"></param>
	/// <returns></returns>
	std::pair<std::vector<vec3_base>, std::vector<uint32_t>> optimize_intersection_set(
		const std::vector<vec3_base>& vertexes, const std::vector<uint32_t>& indexes
	);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="p"></param>
	/// <param name="dir"></param>
	/// <param name="s0"></param>
	/// <param name="s1"></param>
	/// <param name="s2"></param>
	/// <returns></returns>
	bool ray_intersects_triangle(vec3_base p, vec3_base dir, vec3_base s0, vec3_base s1, vec3_base s2);
	
	/// <summary>
	/// 
	/// </summary>
	/// <param name="p"></param>
	/// <param name="s0"></param>
	/// <param name="s1"></param>
	/// <param name="s2"></param>
	/// <returns></returns>
	bool check_is_point_in_face(vec3_base p, vec3_base s0, vec3_base s1, vec3_base s2);

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