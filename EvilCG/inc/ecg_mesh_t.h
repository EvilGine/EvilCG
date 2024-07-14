#ifndef ECG_MESH_H
#define ECG_MESH_H
#include <ecg_global.h>

namespace ecg {
	struct mesh_t {
		std::shared_ptr<std::vector<glm::vec3>> vertexes;
		std::shared_ptr<std::vector<glm::vec3>> indexes;
		std::shared_ptr<std::vector<glm::vec3>> normals;
	};
}

#endif