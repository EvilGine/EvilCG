#ifndef ECG_INTERNAL_H
#define ECG_INTERNAL_H
#include <core/ecg_host_ctrl.h>
#include <ecg_global.h>

namespace ecg {
	struct ecg_cl_internal_mesh {
		cl::Buffer vertexes;
		size_t vertexes_size;

		cl::Buffer indexes;
		size_t indexes_size;

		cl::Buffer normals;
		size_t normals_size;

		ecg_cl_internal_mesh() :
			vertexes_size(0), indexes_size(0), normals_size(0)
		{ }
	};
}

#endif