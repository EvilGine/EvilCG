#ifndef ECG_INTERNAL_H
#define ECG_INTERNAL_H
#include <cl/ecg_host_ctrl.h>
#include <ecg_global.h>

namespace ecg {
	struct ecg_cl_internal_mesh {
		cl::Buffer vertexes;
		cl::Buffer indexes;
		cl::Buffer normals;
	};
}

#endif