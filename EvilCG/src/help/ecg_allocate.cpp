#include <help/ecg_allocate.h>

namespace ecg {
	ecg_cl_internal_mesh get_cl_internal_mesh(const ecg_mem_ctrl& mem_ctrl, const ecg_descriptor desc) {
		ecg_cl_internal_mesh internal_mesh;
		auto handler = mem_ctrl.get_mem_handler(desc.descriptor_id);
		if (handler.get_memory_type() == ecg_memory_type::ECG_CL_INTERNAL_MESH) {
			auto ptr = handler.get_pointer();
			internal_mesh = *static_cast<ecg_cl_internal_mesh*>(ptr.get());
		}
		return internal_mesh;
	}

	ecg_internal_mesh allocate_internal_mesh() {
		ecg_mem_ctrl& mem_ctrl = ecg_mem_ctrl::get_instance();
		ecg_mem_init_info_t memory_init_info = {};
		ecg_internal_mesh result;

		memory_init_info.type = ecg_memory_type::ECG_LIBRARY_ALLOCATED_MESH;
		memory_init_info.ptr = std::shared_ptr<ecg_mesh_t>(new ecg_mesh_t);
		memory_init_info.total_bytes = sizeof(ecg_mesh_t);
		memory_init_info.is_array = false;

		ecg_mem_handler handler = mem_ctrl.ecg_mem_register(memory_init_info);
		result.internal_mesh = static_cast<ecg_mesh_t*>(memory_init_info.ptr.get());
		result.descriptor_id = handler.get_descriptor();

		return result;
	}
}