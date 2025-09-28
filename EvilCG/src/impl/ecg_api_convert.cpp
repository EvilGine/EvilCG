#include <ecg_api.h>

namespace ecg {
	ecg_mesh_t get_mesh_from_internal_mesh(const ecg_internal_mesh_t internal_mesh) {
		ecg::ecg_mesh_t mesh;
		mesh.indexes = static_cast<uint32_t*>(internal_mesh.indexes.arr_ptr);
		mesh.indexes_size = internal_mesh.indexes.arr_size;

		mesh.vertexes = static_cast<ecg::vec3_base*>(internal_mesh.vertexes.arr_ptr);
		mesh.vertexes_size = internal_mesh.vertexes.arr_size;
		return mesh;
	}
}