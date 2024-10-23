#include <ecg_meshes.h>

ecg_meshes& ecg_meshes::get_instance() {
	static ecg_meshes instance;
	return instance;
}

void ecg_meshes::load_meshes(std::string path_to_meshes) {
	if (!std::filesystem::exists(path_to_meshes)) return;
	auto dir_iter = std::filesystem::directory_iterator(path_to_meshes);

	for (auto& item : dir_iter) {
        auto fl_path = item.path();
        if (fl_path.extension() == ".obj" && fl_path.filename().string().find("_res.obj") == std::string::npos) {
            ecg::mesh_t mesh = load_mesh_from_obj(fl_path.string());
            meshes.push_back(mesh);
        }
	}
}

ecg_meshes::~ecg_meshes() {
    for (auto& mesh : meshes) {
        delete[] mesh.vertexes;
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

ecg::vec3_base parse_vertex(const std::string& line) {
    std::string prefix;
    ecg::vec3_base vec;
    std::stringstream ss(line);
    ss >> prefix >> vec.x >> vec.y >> vec.z;
    return vec;
}

ecg::mesh_t load_mesh_from_obj(std::string filepath) {
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