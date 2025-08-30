#include <help/ecg_handler.h>

namespace ecg {
	ecg_mem_handler::~ecg_mem_handler() {
		if (!m_is_initialized) return;
		destroy_handler();
	}

	void ecg_mem_handler::destroy_handler() {
		switch (m_memory_type) {
		case ecg::ecg_memory_type::ECG_INDEXES_ARRAY:
			break;
		case ecg::ecg_memory_type::ECG_NORMALS_ARRAY:
			break;
		case ecg::ecg_memory_type::ECG_VECTORS_ARRAY:
			break;
		case ecg::ecg_memory_type::ECG_CL_INTERNAL_MESH:
			break;
		case ecg::ecg_memory_type::ECG_LIBRARY_ALLOCATED_MESH: 
			{
				auto mesh_ptr = static_cast<ecg_mesh_t*>(m_ecg_memory.get());
				delete[] mesh_ptr->vertexes;
				delete[] mesh_ptr->indexes;
				delete[] mesh_ptr->normals;
			}
			break;
		case ecg::ecg_memory_type::NONE_MEMORY_TYPE:
			assert(false && "Incorrect memory type");
			break;
		default:
			assert(false && "Unknown memory type");
			break;
		}
	}

	void ecg_mem_handler::init_memory(const ecg_mem_init_info_t& init_info) {
		if (m_ecg_memory != nullptr) destroy_handler();
		m_ecg_memory_size = init_info.total_bytes;
		m_is_array = init_info.is_array;
		m_memory_type = init_info.type;
		m_ecg_memory = init_info.ptr;

		if (++m_ecg_global_handler_id == 0) ++m_ecg_global_handler_id;
		m_ecg_handler_id = m_ecg_global_handler_id;
		m_is_initialized = true;
	}

	bool ecg_mem_handler::is_array() const { return m_is_array; }
	uint64_t ecg_mem_handler::get_descriptor() const { return m_ecg_handler_id; }

	std::shared_ptr<void> ecg_mem_handler::get_pointer() const { return m_ecg_memory; }
	ecg_memory_type ecg_mem_handler::get_memory_type() const { return m_memory_type; }
	size_t ecg_mem_handler::get_memory_size() const { return m_ecg_memory_size; }

	bool ecg_mem_handler::operator==(const ecg_mem_handler& other) const {
		return
			m_ecg_memory == other.m_ecg_memory &&
			m_ecg_handler_id == other.m_ecg_handler_id;
	}

	bool ecg_mem_handler::operator!=(const ecg_mem_handler& other) const {
		return !(*this == other);
	}
}