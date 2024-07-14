#include <ecg_api.h>

namespace ecg {
	ecg_status check_mesh(mesh_t* mesh) {
        if (mesh == nullptr)
            return ARG_NULL_PTR;
        if (mesh->vertexes == nullptr || mesh->indexes == nullptr)
            return ARG_NULL_PTR;

        if (mesh->vertexes->size() == 0)
            return EMPTY_VERTEXES;
        if (mesh->indexes->size() == 0)
            return EMPTY_INDEXES;

        return SUCCESS;
	}
}