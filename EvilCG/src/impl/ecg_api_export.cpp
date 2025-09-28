#include <ecg_api.h>

#include <core/ecg_cl_programs.h>
#include <core/ecg_host_ctrl.h>
#include <core/ecg_internal.h>
#include <core/ecg_program.h>

#include <help/ecg_allocate.h>
#include <help/ecg_logger.h>
#include <help/ecg_helper.h>
#include <help/ecg_checks.h>
#include <help/ecg_math.h>
#include <help/ecg_geom.h>

namespace ecg {
	void save_ecg_as_obj(std::ofstream& file, const ecg_mesh_t* mesh) {
		if (mesh->vertexes != nullptr) {
			for (size_t id = 0; id < mesh->vertexes_size; ++id) {
				vec3_base vec = mesh->vertexes[id];
				file << std::format("v {} {} {}", vec.x, vec.y, vec.z) << std::endl;
			}
		}

		if (mesh->indexes != nullptr) {
			for (size_t id = 0; id < mesh->indexes_size; id += 3) {
				uint32_t* base = &mesh->indexes[id];
				file << std::format("f {} {} {}", base[0] + 1, base[1] + 1, base[2] + 1) << std::endl;
			}
		}
	}

	void save_mesh(const ecg_mesh_t* mesh, const char* filename, ecg_file_type fl_type, ecg_status* status) {
		ecg_status_handler op_res;

		try {
			if (mesh == nullptr) op_res = ecg_status_code::INVALID_ARG;
			if (filename == nullptr) op_res = ecg_status_code::INVALID_ARG;

			std::string fl = filename;
			std::ofstream file;

			file.open(fl);
			if (!file.is_open()) op_res = ecg_status_code::RUNTIME_ERROR;

			switch (fl_type) {
			case ecg::ECG_OBJ_FILE:
				save_ecg_as_obj(file, mesh);
				break;
			//case ecg::ECG_RAW_FILE:
			//	break;
			default:
				op_res = ecg_status_code::INVALID_ARG;
				break;
			}
		}
		catch (...) {
			on_unknown_exception(op_res, status);
		}
	}
}