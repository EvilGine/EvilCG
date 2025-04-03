#ifndef ECG_MEMORY_H
#define ECG_MEMORY_H
#include <help/ecg_handler.h>
#include <help/ecg_hasher.h>
#include <ecg_global.h>

namespace ecg {
	class ecg_mem_ctrl;
	class ecg_mem_handler;

	/// <summary>
	/// Default interface for memory controller
	/// </summary>
	class ecg_mem_ctrl {
	public:
		static ecg_mem_ctrl& get_instance();
		bool is_contains_handler(uint64_t handler_id) const;
		ecg_mem_handler get_mem_handler(uint64_t handler_id) const;
		ecg_mem_handler ecg_mem_register(const ecg_mem_init_info_t& init_info);
		bool ecg_mem_free(const ecg_mem_handler& handler);
		bool ecg_mem_free(uint64_t handler_id);
		bool ecg_mem_free_all();

	protected:
		std::unordered_map<uint64_t, ecg_mem_handler> m_allocated_objects;
		std::unordered_set<std::shared_ptr<void>> m_pointers;
		std::mutex m_controller_mutex;
		ecg_mem_ctrl() = default;
	};
}

#endif