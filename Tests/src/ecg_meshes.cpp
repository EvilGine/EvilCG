#include <ecg_meshes.h>

ecg_meshes& ecg_meshes::get_instance() {
	static ecg_meshes instance;
	return instance;
}

void ecg_meshes::save_bb_to_obj(const ecg::full_bounding_box* bb, std::string obj_file) {
    std::ofstream file;
    file.open(obj_file);

    if (file.is_open()) {
        file << ecg_meshes::convert_bb_to_str(bb);
    }

    file.close();
}

void ecg_meshes::save_bb_to_obj(const ecg::bounding_box* bb, std::string obj_file) {
    ecg::full_bounding_box full_bb = ecg::bb_to_full_bb(bb);
    ecg_meshes::save_bb_to_obj(&full_bb, obj_file);
}

void ecg_meshes::save_mesh_to_obj(const ecg::ecg_mesh_t* mesh, std::string obj_file) {
    std::ofstream file;
    file.open(obj_file);

    if (file.is_open()) {
        // Write vertices
        for (uint32_t i = 0; i < mesh->vertexes_size; ++i) {
            file << "v " << to_string(mesh->vertexes[i]) << std::endl;
        }

        // Write normals
        for (uint32_t i = 0; i < mesh->normals_size; ++i) {
            file << "vn " << to_string(mesh->normals[i]) << std::endl;
        }

        // Write faces
        for (uint32_t i = 0; i < mesh->indexes_size; i += 3) {
            file << "f ";
            for (uint32_t j = 0; j < 3; ++j) {
                file << (mesh->indexes[i + j] + 1);
                if (mesh->normals_size > 0) {
                    file << "//" << (mesh->indexes[i + j] + 1);
                }
                file << " ";
            }
            file << std::endl;
        }
    }

    file.close();
}

std::string ecg_meshes::to_string(const ecg::vec3_base& vec) {
    return std::to_string(vec.x) + " " + std::to_string(vec.y) + " " + std::to_string(vec.z);
}

void ecg_meshes::init_mesh(ecg::ecg_mesh_t* mesh, size_t size) {
    if (mesh == nullptr) return;

    mesh->vertexes_size = mesh->normals_size = size;

    size_t triangle_count = size / 3;
    size_t index_count = triangle_count * 3;
    mesh->indexes_size = index_count;

    mesh->vertexes = new ecg::vec3_base[size];
    mesh->normals = new ecg::vec3_base[size];
    mesh->indexes = new uint32_t[index_count];

    uint32_t cnt = 1;
    for (uint32_t i = 0; i < size; ++i) {
        mesh->vertexes[i] = ecg::vec3_base(cnt, cnt * 2.0f, cnt * 3.0f);
        mesh->normals[i] = ecg::vec3_base(0.0f, 1.0f, 0.0f);
        ++cnt;
    }

    for (uint32_t i = 0; i < triangle_count; ++i) {
        mesh->indexes[i * 3] = i * 3;
        mesh->indexes[i * 3 + 1] = i * 3 + 1;
        mesh->indexes[i * 3 + 2] = i * 3 + 2;
    }
}

void ecg_meshes::delete_mesh(ecg::ecg_mesh_t* mesh) {
    if (mesh == nullptr) return;
    
    delete[] mesh->vertexes;
    mesh->vertexes_size = 0;

    delete[] mesh->normals;
    mesh->normals_size = 0;

    delete[] mesh->indexes;
    mesh->indexes_size = 0;
}

void ecg_meshes::load_meshes(std::string path_to_meshes) {
	if (!std::filesystem::exists(path_to_meshes)) return;
	auto dir_iter = std::filesystem::directory_iterator(path_to_meshes);

	for (auto& item : dir_iter) {
        auto fl_path = item.path();
        auto fl_name = fl_path.filename().string();

        if (fl_path.extension() == ".obj" && fl_name.find("_res.obj") == std::string::npos) {
            if (fl_name.find("_test") != std::string::npos) continue;
            ecg::ecg_mesh_t mesh = load_mesh_from_obj(fl_path.string());

            ecg_test_mesh_ptr mesh_ptr(new ecg_test_mesh{ fl_path, mesh });
            loaded_meshes_by_name[fl_name] = mesh_ptr;
            loaded_meshes.push_back(mesh_ptr);
        }
	}
}

ecg_meshes::~ecg_meshes() {
    for (auto& item : template_meshes) {
        delete_mesh(&item->mesh);
    }

    for (auto& item : loaded_meshes) {
        delete_mesh(&item->mesh);
    }
}

void save_to_file(std::string data, std::string filename) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "[ERR]:> Could not open file " << filename << " for writing." << std::endl;
        return;
    }

    file << data;
    file.close();
}

std::string ecg_meshes::convert_bb_to_str(const ecg::full_bounding_box* bb) {
    std::stringstream ss;
    if (bb == nullptr) return "";

    ss << "v " << bb->p0.x << " " << bb->p0.y << " " << bb->p0.z << "\n";
    ss << "v " << bb->p1.x << " " << bb->p1.y << " " << bb->p1.z << "\n";
    ss << "v " << bb->p2.x << " " << bb->p2.y << " " << bb->p2.z << "\n";
    ss << "v " << bb->p3.x << " " << bb->p3.y << " " << bb->p3.z << "\n";
    ss << "v " << bb->p4.x << " " << bb->p4.y << " " << bb->p4.z << "\n";
    ss << "v " << bb->p5.x << " " << bb->p5.y << " " << bb->p5.z << "\n";
    ss << "v " << bb->p6.x << " " << bb->p6.y << " " << bb->p6.z << "\n";
    ss << "v " << bb->p7.x << " " << bb->p7.y << " " << bb->p7.z << "\n";

    ss << "f 1 2 3 4\n";
    ss << "f 5 6 7 8\n";
    ss << "f 1 2 6 5\n";
    ss << "f 2 3 7 6\n";
    ss << "f 3 4 8 7\n";
    ss << "f 4 1 5 8\n";

    return ss.str();
}

ecg::vec3_base ecg_meshes::parse_vertex(const std::string& line) {
    std::string prefix;
    ecg::vec3_base vec;
    std::stringstream ss(line);
    ss >> prefix >> vec.x >> vec.y >> vec.z;
    return vec;
}

void ecg_meshes::parse_face(const std::string& line, std::vector<uint32_t>& indices) {
    std::istringstream s(line.substr(2));
    std::string vertex;
    while (s >> vertex) {
        size_t first_slash = vertex.find('/');
        int index = std::stoi(vertex.substr(0, first_slash)) - 1;  // OBJ indices start at 1
        indices.push_back(index);
    }
}

ecg::ecg_mesh_t ecg_meshes::load_mesh_from_obj(std::string filepath) {
    ecg::ecg_mesh_t mesh;
    std::string line;

    std::ifstream file;
    file.open(filepath);

    std::vector<ecg::vec3_base> obj_vertexes;
    std::vector<uint32_t> obj_indexes;

    if (file.is_open()) {
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
    }

    mesh.vertexes_size = obj_vertexes.size();
    mesh.vertexes = new ecg::vec3_base[obj_vertexes.size()];
    std::memcpy(mesh.vertexes, obj_vertexes.data(), obj_vertexes.size() * sizeof(ecg::vec3_base));

    mesh.indexes_size = obj_indexes.size();
    mesh.indexes = new uint32_t[obj_indexes.size()];
    std::memcpy(mesh.indexes, obj_indexes.data(), obj_indexes.size() * sizeof(uint32_t));

    return mesh;
}

void ecg_meshes::internal_init() {
    template_meshes.resize(10);
    const size_t increment = 1024;
    size_t init_size = 13;

    for (auto& item : template_meshes) {
        item = ecg_test_mesh_ptr(new ecg_test_mesh());
        item->full_path = "mesh_" + std::to_string(init_size);
        init_mesh(&item->mesh, init_size);
        init_size += increment;
    }
}

ecg_meshes::ecg_meshes() {
    static std::once_flag init_flg;
    std::call_once(init_flg, [this]() { this->internal_init(); });
}