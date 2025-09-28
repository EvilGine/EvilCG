#ifndef ECG_API_IMPORT_H
#define ECG_API_IMPORT_H
#include <help/ecg_allocate.h>
#include <help/ecg_logger.h>
#include <help/ecg_checks.h>
#include <help/ecg_mem.h>
#include <ecg_api.h>

namespace ecg {
	ecg_file_type get_type_by_ext(const std::string& ext) {
		if (ext == ".obj") return ecg_file_type::ECG_OBJ_FILE;
		//if (ext == ".ecgm") return ecg_file_type::ECG_RAW_FILE;
		return ecg_file_type::ECG_UNKNOWN_TYPE;
	}

	namespace obj {
		ecg::vec3_base parse_vertex(const std::string& line) {
			std::string prefix;
			ecg::vec3_base vec;
			std::stringstream ss(line);
			ss >> prefix >> vec.x >> vec.y >> vec.z;
			return vec;
		}

		void parse_face(const std::string& line, std::vector<uint32_t>& indices) {
			const int _obj_default_index_offset = 1;
			std::istringstream s(line.substr(2));
			std::string vertex;

			while (s >> vertex) {
				size_t first_slash = vertex.find('/');
				int index = std::stoi(vertex.substr(0, first_slash)) - _obj_default_index_offset;
				indices.push_back(index);
			}
		}

		ecg_internal_mesh_t import_obj_file(std::ifstream& file) {
			ecg_internal_mesh_t mesh;

			std::vector<ecg::vec3_base> obj_vertexes;
			std::vector<uint32_t> obj_indexes;
			std::string line = "";

			while (std::getline(file, line)) {
				if (line[0] == 'v' && line[1] == ' ') {
					ecg::vec3_base vec = parse_vertex(line);
					obj_vertexes.push_back(vec);
				}
				else if (line.rfind("f ", 0) == 0) {
					parse_face(line, obj_indexes);
				}
			}
			file.close();

			if (!obj_vertexes.empty()) {
				mesh.vertexes = allocate_array<vec3_base>(obj_vertexes.size());
				if(mesh.vertexes.arr_ptr != nullptr)
					std::memcpy(mesh.vertexes.arr_ptr, obj_vertexes.data(), obj_vertexes.size() * sizeof(vec3_base));
			}
			if (!obj_indexes.empty()) {
				mesh.indexes = allocate_array<uint32_t>(obj_indexes.size());
				if(mesh.indexes.arr_ptr != nullptr)
					std::memcpy(mesh.indexes.arr_ptr, obj_indexes.data(), obj_indexes.size() * sizeof(uint32_t));
			}

			return mesh;
		}
	}

	namespace native {
		ecg_internal_mesh_t import_ecg_native_file(std::ifstream& file) {
			// TODO: Implement later, than stab ecg_mesh_t
			return ecg_internal_mesh_t{};
		}
	}

	ecg_internal_mesh_t load_mesh(const char* filename, ecg_status* status) {
		auto& mem_inst = ecg_mem::get_instance();
		ecg_internal_mesh_t result;
		ecg_status_handler op_res;

		try {
			if (filename == nullptr) {
				if (status != nullptr) *status = ecg_status_code::INVALID_ARG;
				return ecg_internal_mesh_t{};
			}
			std::filesystem::path path_to_file = filename;
			
			std::ifstream file;
			file.open(path_to_file);
			if (file.is_open()) {
				ecg_file_type type = get_type_by_ext(path_to_file.extension().string());
				switch (type)
				{
				case ecg::ECG_OBJ_FILE:
					result = obj::import_obj_file(file);
					break;
				//case ecg::ECG_RAW_FILE:
				//	result = import_ecg_native_file(file);
				//	break;
				case ecg::ECG_UNKNOWN_TYPE:
					warning("Unknown file type");
					break;
				default:
					break;
				}
			}
		}
		catch (...) {
			on_unknown_exception(op_res, status);
			mem_inst.delete_memory(result.vertexes.handler);
			mem_inst.delete_memory(result.indexes.handler);
			result = ecg_internal_mesh_t{};
		}

		return result;
	}
}

#endif