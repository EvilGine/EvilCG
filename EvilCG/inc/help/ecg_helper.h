#ifndef ECG_HELPER_H
#define ECG_HELPER_H
#include <help/ecg_geom.h>
#include <ecg_global.h>
#include <Eigen/Dense>

namespace ecg {
    mat3_base make_transform(const vec3_base& z_vec, const vec3_base& y_vec);
    mat3_base convert_from_eigen(const Eigen::Matrix3f& eigen_mat);
    vec3_base convert_from_eigen(const Eigen::Vector3f& eigen_vec);
    vec3_base cross(const vec3_base& lhs, const vec3_base& rhs);
    Eigen::Matrix3f convert_to_eigen_mat(const mat3_base& mat);
    mat3_base invert(const mat3_base& mat) noexcept;
    vec3_base normalize(const vec3_base& vec);
    mat3_base transpose(const mat3_base& mat);
    svd_t compute_svd(const mat3_base& mat);
    float length(const vec3_base& vec);
    float det(const mat3_base& mat);
}

#endif