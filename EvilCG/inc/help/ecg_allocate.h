#ifndef ECG_ALLOCATE_H
#define ECG_ALLOCATE_H
#include <core/ecg_internal.h>
#include <help/ecg_geom.h>
#include <help/ecg_mem.h>

namespace ecg {
	enum ecg_memory_type {
		ECG_INDEXES_ARRAY,
		ECG_VECTORS_ARRAY
	};

	template <typename Type>
	ecg_array_t allocate_array(size_t items_cnt) {
		ecg_array_t result;
		result.arr_size = 0;
		result.arr_ptr = nullptr;

		if (items_cnt == 0)
			return result;

		auto& mem_inst = ecg_mem::get_instance();
		auto handle_data = mem_inst.allocate<Type>(items_cnt);

		if (handle_data.ptr != nullptr) {
			result.arr_ptr = handle_data.ptr.get();
			result.handler = handle_data.handle;
			result.arr_size = items_cnt;
		}

		return result;
	}
}

#endif