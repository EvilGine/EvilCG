#ifndef ECG_HASHER_H
#define ECG_HASHER_H
#include <help/ecg_geom.h>
#include <ecg_global.h>

namespace ecg {
	class ecg_hash_func {
	public:
		size_t operator()(const std::pair<uint32_t, uint32_t>& obj) const noexcept;
		size_t operator()(const vec3_base& obj) const noexcept;
	};

	class ecg_compare_func {
	public:
		bool operator()(const std::pair<uint32_t, uint32_t>& a, const std::pair<uint32_t, uint32_t>& b) const noexcept;
		bool operator()(const vec3_base& a, const vec3_base& b) const noexcept;
	};

	template <typename T> int sign(T val) {
		return (T(0) < val) - (val < T(0));
	}
}

#endif