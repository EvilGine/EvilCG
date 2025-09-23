#include <help/ecg_checks.h>

namespace ecg {
	void default_mesh_check(const ecg_mesh_t* mesh, ecg_status_handler& op_res, ecg_status* status) {
		if (status != nullptr) *status = ecg_status_code::SUCCESS;
		if (mesh == nullptr) op_res = ecg_status_code::INVALID_ARG;
		if (mesh->vertexes == nullptr || mesh->vertexes_size <= 0) op_res = ecg_status_code::EMPTY_VERTEX_ARR;
		if (mesh->indexes == nullptr || mesh->indexes_size <= 0) op_res = ecg_status_code::EMPTY_INDEX_ARR;
		if (mesh->indexes_size % 3 != 0) op_res = ecg_status_code::NOT_TRIANGULATED_MESH;
	}

	void on_unknown_exception(ecg_status_handler& op_res, ecg_status* status) {
		if (op_res == ecg_status_code::SUCCESS)
			op_res = ecg_status_code::UNKNOWN_EXCEPTION;
		if (status != nullptr)
			*status = op_res.get_status();
		
		if (g_ecg_logger) {
			std::scoped_lock lock{ g_ecg_logger_mutex };
			g_ecg_logger->error("Unknown error: {}", op_res.get_status());
		}
	}
}