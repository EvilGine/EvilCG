#ifndef ECG_MESHES_H
#define ECG_MESHES_H
#include <ecg_api.h>

struct ecg_test_mesh {
	std::filesystem::path full_path;
	ecg::ecg_mesh_t mesh;
};

typedef std::shared_ptr<ecg_test_mesh> ecg_test_mesh_ptr;

class ecg_meshes {
public:
	std::map<std::string, ecg_test_mesh_ptr> loaded_meshes_by_name;
	std::vector<ecg_test_mesh_ptr> template_meshes;
	std::vector<ecg_test_mesh_ptr> loaded_meshes;

	static ecg_meshes& get_instance();
	void load_meshes(std::string path_to_meshes = "");
	virtual ~ecg_meshes();

	static void save_bb_to_obj(const ecg::full_bounding_box* bb, std::string obj_file);
	static void save_bb_to_obj(const ecg::bounding_box* bb, std::string obj_file);
	static void save_mesh_to_obj(const ecg::ecg_mesh_t* mesh, std::string obj_file);
	static std::string to_string(const ecg::vec3_base& vec);
	static void init_mesh(ecg::ecg_mesh_t* mesh, size_t size);
	static void delete_mesh(ecg::ecg_mesh_t* mesh);

private:
	void parse_face(const std::string& line, std::vector<uint32_t>& indices);
	ecg::vec3_base parse_vertex(const std::string& line);

	static std::string convert_bb_to_str(const ecg::full_bounding_box* bb);
	ecg::ecg_mesh_t load_mesh_from_obj(std::string filepath);
	void internal_init();
	ecg_meshes();
};

#endif