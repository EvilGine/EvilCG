#ifndef ECG_MESH_H
#define ECG_MESH_H
#include <ecg_global.h>

namespace ecg {
	struct mesh_t {
		std::shared_ptr<glm::vec3> vertexes;
		std::shared_ptr<glm::vec3> normals;
		std::shared_ptr<uint32_t> indexes;
	};

	struct vec3_base {
		float x;
		float y;
		float z;

		vec3_base() : x(0.0f), y(0.0f), z(0.0f) {}
		vec3_base(float base) : x(base), y(base), z(base) {}
		vec3_base(float x, float y, float z) : x(x), y(y), z(z) {}
	};

	struct vec4_base {
		float x;
		float y;
		float z;
		float w;

		vec4_base() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
		vec4_base(float base) : x(base), y(base), z(base), w(base) {}
		vec4_base(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
		vec4_base(const vec3_base& v3, float w) : x(v3.x), y(v3.y), z(v3.z), w(w) {}
	};
}

#endif