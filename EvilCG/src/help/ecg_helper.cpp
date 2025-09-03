#include <help/ecg_helper.h>

namespace ecg {
	std::pair<std::vector<vec3_base>, std::vector<uint32_t>> optimize_geom_data(
		const std::vector<vec3_base>& vertexes, const std::vector<uint32_t>& indexes
	) {
		std::unordered_map<vec3_base, uint32_t, ecg_hash_func, ecg_compare_func> vert_to_index;
		std::vector<vec3_base> opt_vertexes;
		std::vector<uint32_t> opt_indexes;

		for (auto idx : indexes) {
			if (idx >= vertexes.size())
				continue;

			const auto& v = vertexes[idx];
			auto it = vert_to_index.find(v);

			if (it == vert_to_index.end()) {
				uint32_t new_id = static_cast<uint32_t>(opt_vertexes.size());
				opt_vertexes.push_back(v);

				vert_to_index[v] = new_id;
				opt_indexes.push_back(new_id);
			}
			else {
				opt_indexes.push_back(it->second);
			}
		}

		return { opt_vertexes, opt_indexes };
	}

	std::pair<std::vector<vec3_base>, std::vector<uint32_t>> optimize_intersection_set(
		const std::vector<vec3_base>& vertexes, const std::vector<uint32_t>& indexes
	) {
		if (vertexes.size() * 2 != indexes.size())
			return { {}, {} };

		std::unordered_map<vec3_base, std::pair<uint32_t, uint32_t>, ecg_hash_func, ecg_compare_func> unique_vertexes;
		for (size_t id = 0; id < vertexes.size(); ++id) {
			vec3_base vertex = vertexes[id];
			std::pair<uint32_t, uint32_t> polygons = {
				indexes[id * 2 + 0],
				indexes[id * 2 + 1]
			};

			unique_vertexes.insert({ vertex, polygons });
		}

		std::vector<vec3_base> opt_vertexes;
		std::vector<uint32_t> opt_indexes;

		for (const auto& [vertex, polygons] : unique_vertexes) {
			opt_vertexes.push_back(vertex);
			opt_indexes.push_back(polygons.first);
			opt_indexes.push_back(polygons.second);
		}

		return { opt_vertexes, opt_indexes };
	}

	bool ray_intersects_triangle(vec3_base p, vec3_base dir, vec3_base s0, vec3_base s1, vec3_base s2) {
		vec3_base ab = s1 - s0;
		vec3_base cb = s2 - s0;

		vec3_base normal = cross(ab, cb);

		float d = dot(-normal, s0);
		float denom = dot(normal, dir);
		if (std::abs(denom) < 1e-7f)
			return false;

		float t = -(dot(normal, p) + d) / denom;
		if (t < 0)
			return false;

		vec3_base intersect = p + dir * t;
		return check_is_point_in_face(intersect, s0, s1, s2);
	}

	bool check_is_point_in_face(vec3_base p, vec3_base s0, vec3_base s1, vec3_base s2) {
		constexpr float eps = 1e-8f;

		vec3_base v0 = s1 - s0;
		vec3_base v1 = s2 - s0;
		vec3_base v2 = p - s0;

		float d00 = dot(v0, v0);
		float d01 = dot(v0, v1);
		float d11 = dot(v1, v1);
		float d20 = dot(v2, v0);
		float d21 = dot(v2, v1);

		float denom = d00 * d11 - d01 * d01;
		if (std::abs(denom) < eps) return false;

		float v = (d11 * d20 - d01 * d21) / denom;
		float w = (d00 * d21 - d01 * d20) / denom;
		float u = 1.0f - v - w;

		bool res = u >= eps && v >= eps && w >= eps;
		return res;
	}

	std::pair<uint32_t, uint32_t> make_edge(uint32_t a, uint32_t b) {
		return std::pair{ std::min(a,b), std::max(a,b) };
	};
}