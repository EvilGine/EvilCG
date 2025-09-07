#ifndef ECG_ALLOCATE_H
#define ECG_ALLOCATE_H
#include <core/ecg_host_ctrl.h>
#include <core/ecg_internal.h>
#include <help/ecg_status.h>
#include <help/ecg_geom.h>
#include <help/ecg_mem.h>

namespace ecg {
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

	template <typename Type> 
	void safe_copy_to_arr(const ecg_array_t& arr, std::vector<Type>& container) {
		if (arr.arr_ptr == nullptr)
			return;
		if (container.size() != arr.arr_size)
			return;

		std::memcpy(arr.arr_ptr, container.data(), sizeof(Type) * container.size());
	}

	ecg_cl_mesh_t allocate_cl_mesh(const ecg_mesh_t* mesh);
}

#endif