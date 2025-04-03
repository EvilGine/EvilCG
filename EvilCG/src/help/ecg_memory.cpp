#include <help/ecg_memory.h>

namespace ecg {
	ecg_mem_ctrl& ecg_mem_ctrl::get_instance() {
		static ecg_mem_ctrl instance;
		return instance;
	}

	bool ecg_mem_ctrl::is_contains_handler(uint64_t handler_id) const {
		auto it = m_allocated_objects.find(handler_id);
		if (it != m_allocated_objects.end()) return true;
		return false;
	}

	ecg_mem_handler ecg_mem_ctrl::get_mem_handler(uint64_t handler_id) const {
		auto it = m_allocated_objects.find(handler_id);
		return it != m_allocated_objects.end() ? it->second : ecg_mem_handler();
	}

	ecg_mem_handler ecg_mem_ctrl::ecg_mem_register(const ecg_mem_init_info_t& init_info) {
		std::scoped_lock lock{ m_controller_mutex };
		ecg_mem_handler handler; 
		
		if (!m_pointers.contains(init_info.ptr)) {
			handler.init_memory(init_info);
			m_pointers.insert(init_info.ptr);
			uint64_t handler_id = handler.get_descriptor();
			m_allocated_objects.try_emplace(handler_id, handler);
		}
		
		return handler;
	}
	
	bool ecg_mem_ctrl::ecg_mem_free(const ecg_mem_handler& handler) {
		uint64_t handler_id = handler.get_descriptor();
		return ecg_mem_free(handler_id);
	}

	bool ecg_mem_ctrl::ecg_mem_free(uint64_t handler_id) {
		std::scoped_lock lock{ m_controller_mutex };
		auto it = m_allocated_objects.find(handler_id);

		if (it != m_allocated_objects.end()) {
			auto ptr = it->second.get_pointer();
			m_allocated_objects.erase(it);
			m_pointers.erase(ptr);
			return true;
		}

		return false;
	}

	bool ecg_mem_ctrl::ecg_mem_free_all() {
		std::scoped_lock lock{ m_controller_mutex };
		std::unordered_map<uint64_t, ecg_mem_handler>().swap(m_allocated_objects);
		std::unordered_set<std::shared_ptr<void>>().swap(m_pointers);
		return true;
	}
}