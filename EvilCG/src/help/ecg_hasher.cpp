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

	bool ecg_compare_func::operator()(const std::pair<uint32_t, uint32_t>& a, const std::pair<uint32_t, uint32_t>& b) const noexcept {
		return (a.first == b.first && a.second == b.second) || (a.first == b.second && a.second == b.first);
	}

	bool ecg_compare_func::operator()(const vec3_base& a, const vec3_base& b) const noexcept {
		return compare_vec3_base(a, b, default_eps);
	}

	bool ecg_less_func::operator()(const vec3_base& a, const vec3_base& b) const noexcept {
		if (std::abs(a.x - b.x) > ecg::default_eps) return a.x < b.x;
		if (std::abs(a.y - b.y) > ecg::default_eps) return a.y < b.y;
		if (std::abs(a.z - b.z) > ecg::default_eps) return a.z < b.z;
		return false;
	}
}