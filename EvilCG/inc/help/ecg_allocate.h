#ifndef ECG_ALLOCATE_H
#define ECG_ALLOCATE_H
#include <core/ecg_internal.h>
#include <help/ecg_geom.h>

namespace ecg {
	enum ecg_memory_type {
		ECG_INDEXES_ARRAY,
		ECG_VECTORS_ARRAY
	};

	template <typename Type>
	ecg_array_t allocate_array(int items_cnt, ecg_memory_type type) {
		ecg_array_t result;
		result.arr_size = 0;
		result.arr_ptr = nullptr;

		auto ptr = std::shared_ptr<Type[]>(new Type[items_cnt]);
		if (ptr != nullptr) {
			result.arr_size = sizeof(Type) * items_cnt;
			result.arr_ptr = ptr.get();
		}

		return result;
	}
}

#endif