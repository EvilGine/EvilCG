#include <help/ecg_hasher.h>
#include <boost/container_hash/hash.hpp>

namespace ecg {
	constexpr float c_equal_eps = std::numeric_limits<float>::epsilon();
	constexpr float default_eps = 1E-06f;

	inline long quantize_float(float value, float epsilon = default_eps) {
		return static_cast<long>(std::round(value / epsilon));
	}

	size_t ecg_hash_func::operator()(const vec3_base& obj) const noexcept {
		std::size_t seed = 0;
		boost::hash_combine(seed, quantize_float(obj.x));
		boost::hash_combine(seed, quantize_float(obj.y));
		boost::hash_combine(seed, quantize_float(obj.z));
		return seed;
	}

	size_t ecg_hash_func::operator()(const std::pair<uint32_t, uint32_t>& obj) const noexcept {
		std::size_t seed = 0;
		std::hash<uint32_t> hi;

		boost::hash_combine(seed, hi(std::min(obj.first, obj.second)));
		boost::hash_combine(seed, hi(std::max(obj.first, obj.second)));

		return seed;
	}

	size_t ecg_hash_func::operator()(const face_t& obj) const noexcept {
		std::size_t seed = 0;
		std::hash<uint32_t> hi;

		std::array<uint32_t, 3> f = { obj.ind_1, obj.ind_2, obj.ind_3 };
		std::sort(f.begin(), f.end());

		boost::hash_combine(seed, hi(f[0]));
		boost::hash_combine(seed, hi(f[1]));
		boost::hash_combine(seed, hi(f[2]));

		return seed;
	}

	size_t ecg_hash_func::operator()(const edge_t& obj) const noexcept {
		std::size_t seed = 0;
		std::hash<uint32_t> hi;

		boost::hash_combine(seed, hi(std::min(obj.a, obj.b)));
		boost::hash_combine(seed, hi(std::max(obj.a, obj.b)));

		return seed;
	}

	bool ecg_compare_func::operator()(const std::pair<uint32_t, uint32_t>& a, const std::pair<uint32_t, uint32_t>& b) const noexcept {
		return (a.first == b.first && a.second == b.second) || (a.first == b.second && a.second == b.first);
	}

	bool ecg_compare_func::operator()(const vec3_base& a, const vec3_base& b) const noexcept {
		return compare_vec3_base(a, b, default_eps);
	}

	bool ecg_compare_func::operator()(const face_t& a, const face_t& b) const noexcept {
		std::array<uint32_t, 3> f1 = { a.ind_1, a.ind_2, a.ind_3 };
		std::array<uint32_t, 3> f2 = { b.ind_1, b.ind_2, b.ind_3 };
		
		std::sort(f1.begin(), f1.end());
		std::sort(f2.begin(), f2.end());

		return 
			f1[0] == f2[0] && 
			f1[1] == f2[1] && 
			f1[2] == f2[2];
	}

	bool ecg_compare_func::operator()(const edge_t& a, const edge_t& b) const noexcept {
		return (a.a == b.a && a.b == b.b) || (a.a == b.b && a.b == b.a);
	}

	bool ecg_less_func::operator()(const vec3_base& a, const vec3_base& b) const noexcept {
		if (std::abs(a.x - b.x) > ecg::default_eps) return a.x < b.x;
		if (std::abs(a.y - b.y) > ecg::default_eps) return a.y < b.y;
		if (std::abs(a.z - b.z) > ecg::default_eps) return a.z < b.z;
		return false;
	}

	bool ecg_less_func::operator()(const face_t& a, const face_t& b) const noexcept {
		// Maybe better vector len variant
		std::array<uint32_t, 3> f1 = { a.ind_1, a.ind_2, a.ind_3 };
		std::array<uint32_t, 3> f2 = { b.ind_1, b.ind_2, b.ind_3 };

		std::sort(f1.begin(), f1.end());
		std::sort(f2.begin(), f2.end());

		if (f1[0] != f2[0]) return f1[0] < f2[0];
		if (f1[1] != f2[1]) return f1[1] < f2[1];
		return f1[2] < f2[2];
	}
}