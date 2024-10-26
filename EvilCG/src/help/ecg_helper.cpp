#include <help/ecg_helper.h>

namespace ecg {
    Eigen::Matrix3f convert_to_eigen_mat(const mat3_base& mat) {
        Eigen::Matrix3f eigenMat;
        eigenMat(0, 0) = mat.m00; eigenMat(0, 1) = mat.m01; eigenMat(0, 2) = mat.m02;
        eigenMat(1, 0) = mat.m10; eigenMat(1, 1) = mat.m11; eigenMat(1, 2) = mat.m12;
        eigenMat(2, 0) = mat.m20; eigenMat(2, 1) = mat.m21; eigenMat(2, 2) = mat.m22;
        return eigenMat;
    }

    mat3_base convert_from_eigen(const Eigen::Matrix3f& eigen_mat) {
        mat3_base mat;
        mat.m00 = eigen_mat(0, 0);
        mat.m01 = eigen_mat(0, 1);
        mat.m02 = eigen_mat(0, 2);

        mat.m10 = eigen_mat(1, 0);
        mat.m11 = eigen_mat(1, 1);
        mat.m12 = eigen_mat(1, 2);

        mat.m20 = eigen_mat(2, 0);
        mat.m21 = eigen_mat(2, 1);
        mat.m22 = eigen_mat(2, 2);

        return mat;
    }

    vec3_base convert_from_eigen(const Eigen::Vector3f& eigen_vec) {
        return vec3_base(eigen_vec(0), eigen_vec(1), eigen_vec(2));
    }

    mat3_base transpose(const mat3_base& mat) {
        return mat3_base{
            mat.m00, mat.m10, mat.m20,
            mat.m01, mat.m11, mat.m21,
            mat.m02, mat.m12, mat.m22
        };
    }

    float det(const mat3_base& mat) {
        float pos =
            (mat.m00 * mat.m11 * mat.m22) +
            (mat.m01 * mat.m12 * mat.m20) +
            (mat.m02 * mat.m10 * mat.m21);

        float neg =
            (mat.m02 * mat.m11 * mat.m20) +
            (mat.m01 * mat.m10 * mat.m22) +
            (mat.m00 * mat.m12 * mat.m21);

        return pos - neg;
    }

    mat3_base invert(const mat3_base& mat) noexcept {
        Eigen::Matrix3f eigen_mat = convert_to_eigen_mat(mat);
        Eigen::Matrix3f eigen_inv = eigen_mat.inverse();
        return convert_from_eigen(eigen_inv);
    }

    float length(const vec3_base& vec) {
        return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    }

    vec3_base normalize(const vec3_base& vec) {
        float len = length(vec);
        return vec / len;
    }

    vec3_base cross(const vec3_base& lhs, const vec3_base& rhs) {
        return vec3_base{
            (lhs.y * rhs.z - lhs.z * rhs.y),
            (lhs.z * rhs.x - lhs.x * rhs.z),
            (lhs.x * rhs.y - lhs.y * rhs.x)
        };
    }

    svd_t compute_svd(const mat3_base& mat) {
        auto eigen_mat = convert_to_eigen_mat(mat);
        Eigen::JacobiSVD<Eigen::Matrix3f> svd(eigen_mat, Eigen::ComputeFullU | Eigen::ComputeFullV);

        Eigen::Matrix3f u = svd.matrixU();
        Eigen::Vector3f sigma = svd.singularValues();
        Eigen::Matrix3f v = svd.matrixV();

        return svd_t{ convert_from_eigen(u), convert_from_eigen(sigma), convert_from_eigen(v) };
    }

    mat3_base make_transform(const vec3_base& z_vec, const vec3_base& y_vec) {
        vec3_base x_vec = cross(y_vec, z_vec);

        vec3_base z_norm = normalize(z_vec);
        vec3_base y_norm = normalize(y_vec);
        vec3_base x_norm = normalize(x_vec);

        return mat3_base{
            x_norm.x, y_norm.x, z_norm.x,
            x_norm.y, y_norm.y, z_norm.y,
            x_norm.z, y_norm.z, z_norm.z
        };
    }
}