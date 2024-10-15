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

	struct mat3_base {
		float m00;
		float m01;
		float m02;

		float m10;
		float m11;
		float m12;

		float m20;
		float m21;
		float m22;
	};

	struct svd_t {
		mat3_base u;
		vec3_base sigma;
		mat3_base v;
	};

	struct bounding_box {
		vec3_base min;
		vec3_base max;
	};

	struct full_bounding_box {
		vec3_base p0;
		vec3_base p1;
		vec3_base p2;
		vec3_base p3;

		vec3_base p4;
		vec3_base p5;
		vec3_base p6;
		vec3_base p7;
	};

	const full_bounding_box default_full_bb = {};

	const bounding_box default_bb = {
		vec3_base(FLT_MAX),
		vec3_base(-FLT_MAX),
	};

	const mat3_base null_mat3 = {
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f
	};

	const mat3_base one_mat3 = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
}

#endif