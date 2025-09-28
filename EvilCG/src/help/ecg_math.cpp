#include <help/ecg_math.h>

namespace ecg {
    Eigen::Matrix3f convert(const mat3_base& mat) {
        Eigen::Matrix3f eigenMat;
        eigenMat(0, 0) = mat.m00; eigenMat(0, 1) = mat.m01; eigenMat(0, 2) = mat.m02;
        eigenMat(1, 0) = mat.m10; eigenMat(1, 1) = mat.m11; eigenMat(1, 2) = mat.m12;
        eigenMat(2, 0) = mat.m20; eigenMat(2, 1) = mat.m21; eigenMat(2, 2) = mat.m22;
        return eigenMat;
    }

    mat3_base convert(const Eigen::Matrix3f& eigen_mat) {
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

    vec3_base convert(const Eigen::Vector3f& eigen_vec) {
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
        Eigen::Matrix3f eigen_mat = convert(mat);
        Eigen::Matrix3f eigen_inv = eigen_mat.inverse();
        return convert(eigen_inv);
    }

    svd_t compute_svd(const mat3_base& m) {
        Eigen::JacobiSVD<Eigen::Matrix3f> svd(
            convert(m), Eigen::ComputeFullU | Eigen::ComputeFullV
        );

        return svd_t{
            convert(svd.matrixU()),
            convert(svd.singularValues()),
            convert(svd.matrixV())
        };
    }

    mat3_base make_transform(const vec3_base& z_vec, const vec3_base& y_vec) {
        vec3_base z_n = normalize(z_vec);
        vec3_base y_n = normalize(y_vec);
        vec3_base x_n = normalize(cross(y_n, z_n));

        // Ensure orthogonality
        y_n = cross(z_n, x_n);

        return mat3_base{
            x_n.x, y_n.x, z_n.x,
            x_n.y, y_n.y, z_n.y,
            x_n.z, y_n.z, z_n.z
        };
    }

    float distance(vec3_base pt, vec3_base s0, vec3_base s1, vec3_base s2) {
        vec3_base s0s1 = s1 - s0;
        vec3_base s0s2 = s2 - s0;

        vec3_base normal = cross(s0s1, s0s2);
        float distance = abs(dot(normal, pt - s0)) / length(normal);
        return distance;
    }
    
    float distance(vec3_base pt, vec3_base v0, vec3_base v1) {
        vec3_base ap = pt - v0;
        vec3_base line_dir = v1 - v0;

        vec3_base cross_prod = cross(ap, line_dir);
        float dist = length(cross_prod) / length(line_dir);
        return dist;
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

    float dot(const vec3_base& v1, const vec3_base& v2) {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }
}