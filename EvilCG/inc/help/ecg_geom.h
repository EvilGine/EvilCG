#ifndef ECG_MESH_H
#define ECG_MESH_H
#include <ecg_api_define.h>
#include <ecg_global.h>

namespace ecg {
	constexpr float epsilon = 1e-6f;

	struct ecg_array_t {
		size_t arr_size;
		void* arr_ptr;

		ecg_array_t() : arr_size(0), arr_ptr(nullptr) {}
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

	struct mat4_base {
		float m00;
		float m01;
		float m02;
		float m03;

		float m10;
		float m11;
		float m12;
		float m13;

		float m20;
		float m21;
		float m22;
		float m23;

		float m30;
		float m31;
		float m32;
		float m33;
	};

	/// <summary>
	/// Represents the Singular Value Decomposition (SVD) of a matrix.
	/// It consists of three components: 
	/// - u: Left orthogonal matrix.
	/// - sigma: Diagonal matrix of singular values, stored as a vector.
	/// - v: Right orthogonal matrix.
	/// </summary>
	struct svd_t {
		mat3_base u;
		vec3_base sigma;
		mat3_base v;
	};

	/// <summary>
	/// A simple representation of an axis-aligned bounding box (AABB).
	/// Defined by the minimum and maximum coordinates in 3D space.
	/// </summary>
	struct bounding_box {
		vec3_base min;
		vec3_base max;
	};

	/// <summary>
	/// A full representation of a bounding box in 3D space.
	/// Defined by its eight corner points.
	/// </summary>
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

	/// <summary>
	/// The default representation of grid data in ecg.
	/// Normals are optional in most cases.
	/// </summary>
	struct ecg_mesh_t {
		vec3_base* vertexes;
		uint32_t vertexes_size;

		vec3_base* normals;
		uint32_t normals_size;

		uint32_t* indexes;
		uint32_t indexes_size;

		ecg_mesh_t() : 
			vertexes(nullptr), vertexes_size(0),
			indexes(nullptr), indexes_size(0),
			normals(nullptr), normals_size(0) 
		{}
	};

	struct ecg_internal_mesh {
		ecg_mesh_t* internal_mesh;
		ecg_internal_mesh() : internal_mesh(nullptr) {}
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