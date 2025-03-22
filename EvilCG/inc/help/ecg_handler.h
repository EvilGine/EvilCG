#ifndef ECG_HANDLER_H
#define ECG_HANDLER_H
#include <ecg_global.h>

namespace ecg {
	/// <summary>
	/// Basic handler for specific types
	/// </summary>
	class ecg_base_handler {
	protected:
		static inline std::atomic<uint64_t> m_ecg_global_handler_id = 1;
		uint64_t m_ecg_handler_id;
	};
	
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
	class ecg_mem_handler : public ecg_base_handler {
	public:
		ecg_mem_handler() = default;
		virtual ~ecg_mem_handler() = default;
		void init_memory(const ecg_mem_init_info_t& init_info);

		bool is_array() const;
		uint64_t get_descriptor() const;

		std::shared_ptr<void> get_pointer() const;
		ecg_memory_type get_memory_type() const;
		size_t get_memory_size() const;

		bool operator==(const ecg_mem_handler& other) const;
		bool operator!=(const ecg_mem_handler& other) const;

	private:
		std::shared_ptr<void> m_ecg_memory;
		bool m_is_initialized = false;
		ecg_memory_type m_memory_type;
		size_t m_ecg_memory_size;
		bool m_is_array;
	};
}

#endif