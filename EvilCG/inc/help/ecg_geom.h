#ifndef ECG_MESH_H
#define ECG_MESH_H
#include <ecg_global.h>
#include <api_define.h>

namespace ecg {
	const float epsilon = 1e-5f;

	struct array_t {
		void* arr_ptr;
		uint32_t arr_sz;

		array_t() :
			arr_ptr(nullptr),
			arr_sz(0) {}
	
		array_t(void* ptr, uint32_t size) :
			arr_ptr(ptr), arr_sz(size) {}
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

	struct mesh_t {
		vec3_base* vertexes;
		uint32_t vertexes_size;

		vec3_base* normals;
		uint32_t normals_size;

		uint32_t* indexes;
		uint32_t indexes_size;

		mesh_t() : 
			vertexes(nullptr), vertexes_size(0),
			indexes(nullptr), indexes_size(0),
			normals(nullptr), normals_size(0) 
		{}
	};

	std::ostream& operator<<(std::ostream& os, const vec3_base& rhs);
	vec3_base operator+(const vec3_base& lhs, const vec3_base& rhs);
	vec3_base operator-(const vec3_base& lhs, const vec3_base& rhs);
	vec3_base operator/(const vec3_base& lhs, const float rhs);

	void operator+=(vec3_base& lhs, const vec3_base& rhs);
	void operator+=(vec3_base& lhs, float rhs);

	extern "C" vec3_base ECG_API add_vec(const vec3_base& lhs, const vec3_base& rhs);
	extern "C" vec3_base ECG_API sub_vec(const vec3_base& lhs, const vec3_base& rhs);
	extern "C" vec3_base ECG_API div_vec(const vec3_base& lhs, float rhs);

	bool operator==(const vec3_base& lhs, const vec3_base& rhs);
	extern "C" bool ECG_API compare_vec3_base(const vec3_base& lval, const vec3_base& rval);

	bool operator==(const bounding_box& lval, const bounding_box& rval);
	extern "C" bool ECG_API compare_bounding_boxes(const bounding_box& lval, const bounding_box& rval);

	bool operator==(const full_bounding_box& lval, const full_bounding_box& rval);
	extern "C" bool ECG_API compare_full_bb(const full_bounding_box& lval, const full_bounding_box& rval);

	bool operator==(const mat3_base& lval, const mat3_base rval);
	extern "C" bool ECG_API compare_mat3(const mat3_base& lval, const mat3_base& rval);

	std::ostream& operator<<(std::ostream& os, const mat3_base& rhs);
	vec3_base operator*(const mat3_base& lhs, const vec3_base& rhs);
	mat3_base operator/(const mat3_base& mat, const float del);

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