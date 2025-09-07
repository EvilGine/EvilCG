#ifndef ECG_INTERNAL_H
#define ECG_INTERNAL_H
#include <core/ecg_cl_version.h>
#include <core/ecg_host_ctrl.h>
#include <ecg_global.h>

namespace ecg {
	struct ecg_cl_mesh_t {
		cl::Buffer vertexes_buffer;
		size_t vertexes_buffer_size;
		size_t vertexes_size;

		cl::Buffer indexes_buffer;
		size_t indexes_buffer_size;
		size_t indexes_size;

		bool is_valid = false;

		ecg_cl_mesh_t() :
			vertexes_size(0), indexes_size(0), 
			vertexes_buffer_size(0), indexes_buffer_size(0),
			is_valid(false)
		{ }
	};
}

#endif