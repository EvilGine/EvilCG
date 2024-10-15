#ifndef ECG_API_H
#define ECG_API_H
#include <help/ecg_status.h>
#include <help/ecg_geom.h>
#include <ecg_global.h>
#include <api_define.h>

#ifdef ENABLE_ECG_CL
	#include <cl/ecg_host_ctrl.h>
	#include <cl/ecg_program.h>
#endif

namespace ecg {	
	ECG_API vec3_base get_center(mesh_t* mesh, ecg_status* status = nullptr);
	ECG_API vec3_base summ_vertexes(mesh_t* mesh, ecg_status* status = nullptr);
	ECG_API bounding_box compute_aabb(mesh_t* mesh, ecg_status* status = nullptr);
	ECG_API full_bounding_box compute_obb(mesh_t* mesh, ecg_status* status = nullptr);
}

#endif