#ifndef ECG_ALLOCATE_H
#define ECG_ALLOCATE_H
#include <core/ecg_internal.h>
#include <help/ecg_memory.h>
#include <help/ecg_geom.h>

namespace ecg {
	template <typename Type>
	ecg_array_t allocate_array(int items_cnt, ecg_memory_type type) {
		auto& mem_ctrl = ecg_mem_ctrl::get_instance();
		ecg_mem_init_info_t init_info;
		ecg_array_t result;

		init_info.ptr = std::shared_ptr<Type[]>(new Type[items_cnt]);
		init_info.total_bytes = sizeof(Type) * items_cnt;
		init_info.is_array = true;
		init_info.type = type;

		ecg_mem_handler mem_handler = mem_ctrl.ecg_mem_register(init_info);
		result.descriptor_id = mem_handler.get_descriptor();
		result.arr_size = init_info.total_bytes;
		result.arr_ptr = init_info.ptr.get();
		return result;
	}

	ecg_cl_internal_mesh get_cl_internal_mesh(const ecg_mem_ctrl& mem_ctrl, const ecg_descriptor desc);
	ecg_internal_mesh allocate_internal_mesh();
}

#endif