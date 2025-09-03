#include <help/ecg_overloads.h>
#include <help/ecg_geom.h>

namespace ecg {
	vec3_base add_vec(const vec3_base& lhs, const vec3_base& rhs) {
		return vec3_base(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
	}

	vec3_base sub_vec(const vec3_base& lhs, const vec3_base& rhs) {
		return vec3_base(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
	}

	vec3_base div_vec(const vec3_base& lhs, const float rhs) {
		return vec3_base(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
	}

	vec3_base mul_vec(const vec3_base& lhs, const float rhs) {
		return vec3_base{ lhs.x * rhs, lhs.y * rhs, lhs.z * rhs };
	}

	bool compare_vec3_base(const vec3_base& lval, const vec3_base& rval) {
		return (lval.x == rval.x) && (lval.y == rval.y) && (lval.z == rval.z);
	}
	
	bool compare_bounding_boxes(const bounding_box& lval, const bounding_box& rval) {
		return (lval.min == rval.min) && (lval.max == rval.max);
	}


	bool compare_full_bb(const full_bounding_box& lval, const full_bounding_box& rval) {
		return (lval.p0 == rval.p0) && (lval.p1 == rval.p1) && (lval.p2 == rval.p2) && (lval.p3 == rval.p3);
	}
	
	bool compare_mat3(const mat3_base& lval, const mat3_base& rval) {
		return
			std::abs(lval.m00 - rval.m00) <= epsilon &&
			std::abs(lval.m01 - rval.m01) <= epsilon &&
			std::abs(lval.m02 - rval.m02) <= epsilon &&

			std::abs(lval.m10 - rval.m10) <= epsilon &&
			std::abs(lval.m11 - rval.m11) <= epsilon &&
			std::abs(lval.m12 - rval.m12) <= epsilon &&

			std::abs(lval.m20 - rval.m20) <= epsilon &&
			std::abs(lval.m21 - rval.m21) <= epsilon &&
			std::abs(lval.m22 - rval.m22) <= epsilon;
	}
}