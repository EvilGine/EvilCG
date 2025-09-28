#include <help/ecg_helper.h>

namespace ecg {
	std::pair<std::vector<vec3_base>, std::vector<uint32_t>> optimize_geometry(
		const std::vector<vec3_base>& vertices,
		const std::vector<uint32_t>& indices
	) {
		std::unordered_map<vec3_base, uint32_t, ecg_hash_func, ecg_compare_func> vertex_map;
		std::vector<vec3_base> optimized_vertices;
		std::vector<uint32_t> optimized_indices;

		for (auto idx : indices) {
			if (idx >= vertices.size()) continue;

			const auto& vertex = vertices[idx];
			auto found = vertex_map.find(vertex);

			if (found == vertex_map.end()) {
				uint32_t new_index = static_cast<uint32_t>(optimized_vertices.size());
				optimized_vertices.push_back(vertex);
				vertex_map[vertex] = new_index;
				optimized_indices.push_back(new_index);
			}
			else {
				optimized_indices.push_back(found->second);
			}
		}

		return { optimized_vertices, optimized_indices };
	}

	std::pair<std::vector<vec3_base>, std::vector<uint32_t>> optimize_intersection(
		const std::vector<vec3_base>& vertices,
		const std::vector<uint32_t>& indices
	) {
		if (vertices.size() * 2 != indices.size()) return { {}, {} };

		std::unordered_map<vec3_base, std::pair<uint32_t, uint32_t>, ecg_hash_func, ecg_compare_func> unique_vertices;
		for (size_t i = 0; i < vertices.size(); ++i) {
			vec3_base v = vertices[i];
			unique_vertices[v] = { indices[i * 2], indices[i * 2 + 1] };
		}

		std::vector<vec3_base> optimized_vertices;
		std::vector<uint32_t> optimized_indices;
		optimized_vertices.reserve(unique_vertices.size());
		optimized_indices.reserve(indices.size());

		for (const auto& [vertex, polys] : unique_vertices) {
			optimized_vertices.push_back(vertex);
			optimized_indices.push_back(polys.first);
			optimized_indices.push_back(polys.second);
		}

		return { optimized_vertices, optimized_indices };
	}

	bool ray_hits_triangle(
		const vec3_base& ray_origin, const vec3_base& ray_dir,
		const vec3_base& v0, const vec3_base& v1, const vec3_base& v2
	) {
		vec3_base edge1 = v1 - v0;
		vec3_base edge2 = v2 - v0;
		vec3_base n = cross(edge1, edge2);

		float d = -dot(n, v0);
		float denom = dot(n, ray_dir);
		if (std::abs(denom) < 1e-7f) return false;

		float t = -(dot(n, ray_origin) + d) / denom;
		if (t < 0) return false;

		vec3_base intersection = ray_origin + ray_dir * t;
		return point_in_triangle(intersection, v0, v1, v2);
	}

	bool point_in_triangle(const vec3_base& p, const vec3_base& a,
		const vec3_base& b, const vec3_base& c) {
		const float eps = 1e-8f;
		vec3_base v0 = b - a;
		vec3_base v1 = c - a;
		vec3_base v2 = p - a;

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

		return u >= eps && v >= eps && w >= eps;
	}

	std::pair<uint32_t, uint32_t> make_edge(uint32_t a, uint32_t b) {
		return std::pair{ std::min(a,b), std::max(a,b) };
	};

	edge_t make_edge_struct(uint32_t a, uint32_t b) {
		return edge_t { std::min(a,b), std::max(a,b) };
	}
}