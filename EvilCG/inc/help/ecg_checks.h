#ifndef ECG_CHECKS_H
#define ECG_CHECKS_H
#include <core/ecg_internal.h>
#include <help/ecg_status.h>
#include <help/ecg_logger.h>
#include <help/ecg_geom.h>

namespace ecg {
	void default_mesh_check(const ecg_mesh_t* mesh, ecg_status_handler& op_res, ecg_status* status);
	void on_unknown_exception(ecg_status_handler& op_res, ecg_status* status);
}

#endif