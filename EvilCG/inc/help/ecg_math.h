#ifndef ECG_MATH_H
#define ECG_MATH_H
#include <help/ecg_overloads.h>
#include <help/ecg_geom.h>
#include <ecg_global.h>
#include <Eigen/Dense>

namespace ecg {
    mat3_base convert(const Eigen::Matrix3f& eigen_mat);
    vec3_base convert(const Eigen::Vector3f& eigen_vec);
    Eigen::Matrix3f convert(const mat3_base& mat);

    mat3_base make_transform(const vec3_base& z_vec, const vec3_base& y_vec);
    mat3_base invert(const mat3_base& mat) noexcept;
    vec3_base normalize(const vec3_base& vec);
    mat3_base transpose(const mat3_base& mat);

    vec3_base cross(const vec3_base& lhs, const vec3_base& rhs);
    float dot(const vec3_base& v1, const vec3_base& v2);
    svd_t compute_svd(const mat3_base& mat);
    float length(const vec3_base& vec);
    float det(const mat3_base& mat);

    float distance(vec3_base pt, vec3_base s0, vec3_base s1, vec3_base s2);
    float distance(vec3_base pt, vec3_base v0, vec3_base v1);
}

#endif