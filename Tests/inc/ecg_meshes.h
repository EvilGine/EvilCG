#ifndef ECG_MESHES_H
#define ECG_MESHES_H
#include <ecg_api.h>

class ecg_meshes {
public:
	std::vector<ecg::mesh_t> meshes;
	static ecg_meshes& get_instance();
	void load_meshes(std::string path_to_meshes = "");
	virtual ~ecg_meshes();

private:
	ecg_meshes() = default;
};

ecg::vec3_base parse_vertex(const std::string& line);
ecg::mesh_t load_mesh_from_obj(std::string filepath);

#endif