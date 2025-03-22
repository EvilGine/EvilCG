#ifndef ECG_HASHER_H
#define ECG_HASHER_H
#include <ecg_global.h>

namespace ecg {
	class ecg_mem_handler;

	/// <summary>
	/// Main hasher for custom structures/classes in ecg
	/// </summary>
	class ecg_hasher {
	public:
		size_t operator()(const ecg_mem_handler& handler) const;
	};

	class ecg_equal {
	public:
		bool operator()(const ecg_mem_handler& lhs, const ecg_mem_handler& rhs) const;
	};
}

#endif