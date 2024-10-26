#ifndef ECG_MESHES_H
#define ECG_MESHES_H
#include <ecg_api.h>

struct ecg_test_mesh {
	std::filesystem::path full_path;
	ecg::mesh_t mesh;
};

class ecg_meshes {
public:
	std::vector<ecg_test_mesh> loaded_meshes;
	std::vector<ecg_test_mesh> template_meshes;

	static ecg_meshes& get_instance();
	void load_meshes(std::string path_to_meshes = "");
	virtual ~ecg_meshes();

	static void save_bb_to_obj(const ecg::full_bounding_box* bb, std::string obj_file);
	static void save_bb_to_obj(const ecg::bounding_box* bb, std::string obj_file);
	static void save_mesh_to_obj(const ecg::mesh_t* mesh, std::string obj_file);
	static void init_mesh(ecg::mesh_t* mesh, size_t size);
	static void delete_mesh(ecg::mesh_t* mesh);

private:
	static std::string convert_bb_to_str(const ecg::full_bounding_box* bb);
	ecg::vec3_base parse_vertex(const std::string& line);
	ecg::mesh_t load_mesh_from_obj(std::string filepath);
	void internal_init();
	ecg_meshes();
};

#endif