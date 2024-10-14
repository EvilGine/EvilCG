#ifndef ECG_STATUS_H
#define ECG_STATUS_H
#include <ecg_global.h>

namespace ecg {
	const uint32_t ARG_NULL_PTR = -2;
	const uint32_t UNKNOWN_ERROR = -1;
	const uint32_t SUCCESS = 0;

	const uint32_t EMPTY_MESH = 1;
	const uint32_t EMPTY_INDEXES = 2;
	const uint32_t EMPTY_VERTEXES = 3;

	typedef uint32_t ecg_status;
}

#endif