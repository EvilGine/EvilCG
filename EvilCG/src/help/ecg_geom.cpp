#include <help/ecg_geom.h>

namespace ecg {
	std::ostream& operator<<(std::ostream& os, const vec3_base& rhs) {
		std::stringstream ss;
		ss << "{ " << rhs.x;
		ss << "; " << rhs.y;
		ss << "; " << rhs.z;
		ss << " }" << std::endl;
		os << ss.str();
		return os;
	}

	vec3_base operator*(const mat3_base& lhs, const vec3_base& rhs) {
		vec3_base result;

		result.x = lhs.m00 * rhs.x + lhs.m01 * rhs.y + lhs.m02 * rhs.z;
		result.y = lhs.m10 * rhs.x + lhs.m11 * rhs.y + lhs.m12 * rhs.z;
		result.z = lhs.m20 * rhs.x + lhs.m21 * rhs.y + lhs.m22 * rhs.z;

		return result;
	}

	vec3_base operator+(const vec3_base& lhs, const vec3_base& rhs) {
		return add_vec(lhs, rhs);
	}

	vec3_base operator-(const vec3_base& lhs, const vec3_base& rhs) {
		return sub_vec(lhs, rhs);
	}

	vec3_base operator/(const vec3_base& lhs, float rhs) {
		return div_vec(lhs, rhs);
	}

	void operator+=(vec3_base& lhs, const vec3_base& rhs) {
		lhs.x += rhs.x;
		lhs.y += rhs.y;
		lhs.z += rhs.z;
	}

	void operator+=(vec3_base& lhs, float rhs) {
		lhs.x += rhs;
		lhs.y += rhs;
		lhs.z += rhs;
	}

	vec3_base add_vec(const vec3_base& lhs, const vec3_base& rhs) {
		return vec3_base(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
	}

	vec3_base sub_vec(const vec3_base& lhs, const vec3_base& rhs) {
		return vec3_base(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
	}

	vec3_base div_vec(const vec3_base& lhs, const float rhs) {
		return vec3_base(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
	}

	bool operator==(const vec3_base& lhs, const vec3_base& rhs) {
		return compare_vec3_base(lhs, rhs);
	}

	bool compare_vec3_base(const vec3_base& lval, const vec3_base& rval) {
		return (lval.x == rval.x) && (lval.y == rval.y) && (lval.z == rval.z);
	}
	
	bool compare_bounding_boxes(const bounding_box& lval, const bounding_box& rval) {
		return (lval.min == rval.min) && (lval.max == rval.max);
	}

	bool operator==(const full_bounding_box& lval, const full_bounding_box& rval) {
		return compare_full_bb(lval, rval);
	}

	bool compare_full_bb(const full_bounding_box& lval, const full_bounding_box& rval) {
		return (lval.p0 == rval.p0) && (lval.p1 == rval.p1) && (lval.p2 == rval.p2) && (lval.p3 == rval.p3);
	}

	bool operator==(const mat3_base& lval, const mat3_base rval) {
		return compare_mat3(lval, rval);
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

	std::ostream& operator<<(std::ostream& os, const mat3_base& rhs) {
		std::stringstream ss;
		ss << "{" << std::endl;
		ss << "\t" << rhs.m00 << " " << rhs.m01 << " " << rhs.m02 << std::endl;
		ss << "\t" << rhs.m10 << " " << rhs.m11 << " " << rhs.m12 << std::endl;
		ss << "\t" << rhs.m20 << " " << rhs.m21 << " " << rhs.m22 << std::endl;
		ss << "\t" << "} ";
		return os;
	}

	mat3_base operator/(const mat3_base& mat, const float del) {
		mat3_base new_mat = {
			mat.m00 / del, mat.m01 / del, mat.m02 / del,
			mat.m10 / del, mat.m11 / del, mat.m12 / del,
			mat.m20 / del, mat.m21 / del, mat.m22 / del
		};
		return new_mat;
	}
}