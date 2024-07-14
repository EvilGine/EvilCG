#ifndef ECG_API_H
#define ECG_API_H
#include <ecg_global.h>
#include <ecg_status.h>
#include <ecg_mesh_t.h>

namespace ecg {
#ifdef _DEBUG
	ecg_status ecg_debug_func();
#endif
	ecg_status check_mesh(mesh_t* mesh);

	namespace compute {
		ecg_status calculate_normals(mesh_t* mesh);
	}

	namespace lod {

	}

}

#endif