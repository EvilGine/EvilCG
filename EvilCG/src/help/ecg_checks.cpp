#include <help/ecg_checks.h>

namespace ecg {
	void default_mesh_check(const ecg_mesh_t* mesh, ecg_status_handler& op_res, ecg_status* status) {
		if (status != nullptr) *status = ecg_status_code::SUCCESS;
		if (mesh == nullptr) op_res = ecg_status_code::INVALID_ARG;
		if (mesh->vertexes == nullptr || mesh->vertexes_size <= 0) op_res = ecg_status_code::EMPTY_VERTEX_ARR;
		if (mesh->indexes == nullptr || mesh->indexes_size <= 0) op_res = ecg_status_code::EMPTY_INDEX_ARR;
		if (mesh->indexes_size % 3 != 0) op_res = ecg_status_code::NOT_TRIANGULATED_MESH;
	}

	void default_desc_check(const ecg_mem_ctrl& mem_ctrl, const ecg_descriptor desc, ecg_status_handler& op_res, ecg_status* status) {
		if (status != nullptr) *status = ecg_status_code::SUCCESS;
		if (!mem_ctrl.is_contains_handler(desc.descriptor_id)) op_res = ecg_status_code::INVALID_ARG;

		ecg_cl_internal_mesh internal_mesh = get_cl_internal_mesh(mem_ctrl, desc);
		if (internal_mesh.indexes_size <= 0) op_res = ecg_status_code::EMPTY_INDEX_ARR;
		if (internal_mesh.vertexes_size <= 0) op_res = ecg_status_code::EMPTY_VERTEX_ARR;
		if (internal_mesh.indexes_size % 3 != 0) op_res = ecg_status_code::NOT_TRIANGULATED_MESH;
	}

	void on_unknown_exception(ecg_status_handler& op_res, ecg_status* status) {
		if (op_res == ecg_status_code::SUCCESS)
			op_res = ecg_status_code::UNKNOWN_EXCEPTION;
		if (status != nullptr)
			*status = op_res.get_status();
	}
}