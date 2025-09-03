#include <help/ecg_mem.h>

namespace ecg {
	uint64_t ecg_mem::ms_id = 1;

	void ecg_mem::delete_memory(uint64_t handle) {
		m_memory_map.erase(handle);
	}

	void ecg_mem::delete_all_memory() {
		m_memory_map.clear();
	}

}