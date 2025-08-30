#ifndef ECG_SIMPLIFICATION_H
#define ECG_SIMPLIFICATION_H
#include <help/ecg_status.h>
#include <help/ecg_geom.h>
#include <ecg_global.h>

namespace ecg {
	void center_point_simplification(const ecg_mesh_t* mesh, ecg_internal_mesh& result_mesh, ecg_status_handler& status);
	void qem_simplification(const ecg_mesh_t* mesh, ecg_internal_mesh& result_mesh, ecg_status_handler& status);
}

#endif