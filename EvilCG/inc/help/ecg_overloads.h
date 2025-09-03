#ifndef ECG_OVERLOADS_H
#define ECG_OVERLOADS_H
#include <help/ecg_geom.h>

namespace ecg {
	std::ostream& operator<<(std::ostream& os, const vec3_base& rhs);
	vec3_base operator+(const vec3_base& lhs, const vec3_base& rhs);
	vec3_base operator-(const vec3_base& lhs, const vec3_base& rhs);
	vec3_base operator/(const vec3_base& lhs, const float rhs);
	vec3_base operator*(const vec3_base& lhs, const float rhs);

	bool operator==(const full_bounding_box& lval, const full_bounding_box& rval);
	bool operator==(const bounding_box& lval, const bounding_box& rval);
	bool operator==(const mat3_base& lval, const mat3_base rval);
	bool operator==(const vec3_base& lhs, const vec3_base& rhs);

	void operator+=(vec3_base& lhs, const vec3_base& rhs);
	void operator+=(vec3_base& lhs, float rhs);

	vec3_base operator-(const vec3_base& v);

	std::ostream& operator<<(std::ostream& os, const mat3_base& rhs);
	vec3_base operator*(const mat3_base& lhs, const vec3_base& rhs);
	mat3_base operator/(const mat3_base& mat, const float del);
}

#endif