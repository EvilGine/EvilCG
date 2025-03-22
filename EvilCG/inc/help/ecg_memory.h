#ifndef ECG_MEMORY_H
#define ECG_MEMORY_H
#include <help/ecg_hasher.h>
#include <ecg_global.h>

namespace ecg {
	class ecg_mem_ctrl;
	class ecg_mem_handler;

	/// <summary>
	/// Internal library types
	/// </summary>
	enum class ecg_memory_type {
		ECG_INDEXES_ARRAY,
		ECG_NORMALS_ARRAY,
		ECG_VERTEXES_ARRAY,
		ECG_CL_INTERNAL_MESH,
	};

	/// <summary>
	/// Information for memory init 
	/// </summary>
	struct ecg_mem_init_info_t {
		std::shared_ptr<void> ptr;
		ecg_memory_type type;
		size_t total_bytes;
		bool is_array;
	};

	/// <summary>
	/// Main handler for memory allocated object
	/// </summary>
	class ecg_mem_handler {
	public:
		ecg_mem_handler() = delete;
		virtual ~ecg_mem_handler() = default;

		bool is_array() const;
		uint64_t get_memory_handler() const;
		
		std::shared_ptr<void> get_pointer() const;
		ecg_memory_type get_memory_type() const;
		size_t get_memory_size() const;

		bool operator==(const ecg_mem_handler& other) const;
		bool operator!=(const ecg_mem_handler& other) const;

	protected:
		ecg_mem_handler(const ecg_mem_init_info_t& init_info);
		friend ecg_mem_ctrl;

	private:
		static inline std::atomic<uint64_t> m_ecg_global_handler_id = 1;
		std::shared_ptr<void> m_ecg_memory;
		ecg_memory_type m_memory_type;
		uint64_t m_ecg_handler_id;
		size_t m_ecg_memory_size;
		bool m_is_array;
	};

	/// <summary>
	/// Default interface for memory controller
	/// </summary>
	class ecg_mem_ctrl {
	public:
		static ecg_mem_ctrl& get_instance();
		bool is_contains_handler(uint64_t handler_id);
		ecg_mem_handler ecg_malloc(const ecg_mem_init_info_t& init_info);
		bool ecg_free(const ecg_mem_handler& handler);
		bool ecg_free(uint64_t handler_id);

	protected:
		std::unordered_map<uint64_t, ecg_mem_handler> m_allocated_objects;
		ecg_mem_ctrl() = default;
	};
}

#endif