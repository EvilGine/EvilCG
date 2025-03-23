#ifndef ECG_INTERNAL_H
#define ECG_INTERNAL_H
#include <cl/ecg_host_ctrl.h>
#include <ecg_global.h>

namespace ecg {
	struct ecg_cl_internal_mesh {
		cl::Buffer vertexes;
		size_t vertexes_size;

		cl::Buffer indexes;
		size_t indexes_size;

		cl::Buffer normals;
		size_t normals_size;
	};
}

#endif