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

void ecg_meshes::save_mesh_to_obj(const ecg::mesh_t* mesh, std::string obj_file) {
    std::ofstream file;
    file.open(obj_file);

    if (file.is_open()) {

    }

    file.close();
}

void ecg_meshes::load_meshes(std::string path_to_meshes) {
	if (!std::filesystem::exists(path_to_meshes)) return;
	auto dir_iter = std::filesystem::directory_iterator(path_to_meshes);

	for (auto& item : dir_iter) {
        auto fl_path = item.path();
        if (fl_path.extension() == ".obj" && fl_path.filename().string().find("_res.obj") == std::string::npos) {
            if (fl_path.filename().string().find("_test") != std::string::npos) continue;
            ecg::mesh_t mesh = load_mesh_from_obj(fl_path.string());
            meshes.push_back(ecg_test_mesh{ fl_path, mesh});
        }
	}
}

ecg_meshes::~ecg_meshes() {
    for (auto& item : meshes) {
        delete[] item.mesh.vertexes;
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

ecg::mesh_t ecg_meshes::load_mesh_from_obj(std::string filepath) {
    ecg::mesh_t mesh;
    std::string line;

    std::ifstream file;
    file.open(filepath);

    std::vector<ecg::vec3_base> obj_vertexes;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (line[0] == 'v' && line[1] == ' ') {
                ecg::vec3_base vec = parse_vertex(line);
                obj_vertexes.push_back(vec);
            }
        }
        file.close();
    }

    mesh.vertexes_size = obj_vertexes.size();
    mesh.vertexes = new ecg::vec3_base[obj_vertexes.size()];
    std::memcpy(mesh.vertexes, obj_vertexes.data(), obj_vertexes.size() * sizeof(ecg::vec3_base));

    return mesh;
}