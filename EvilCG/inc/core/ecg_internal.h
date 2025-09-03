#ifndef ECG_INTERNAL_H
#define ECG_INTERNAL_H
#include <core/ecg_cl_version.h>
#include <core/ecg_host_ctrl.h>
#include <ecg_global.h>

namespace ecg {
	struct ecg_cl_mesh_t {
		cl::Buffer vertexes_buffer;
		uint32_t vertexes_buffer_size;
		size_t vertexes_size;

		cl::Buffer indexes_buffer;
		uint32_t indexes_buffer_size;
		size_t indexes_size;

		cl::Buffer normals;
		size_t normals_size;

		bool is_valid = false;

		ecg_cl_mesh_t() :
			vertexes_size(0), indexes_size(0), normals_size(0)
		{ }
	};
}

#endif