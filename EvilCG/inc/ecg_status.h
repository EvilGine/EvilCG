#ifndef ECG_STATUS_H
#define ECG_STATUS_H

namespace ecg {
	const int ARG_NULL_PTR = -2;
	const int UNKNOWN_ERROR = -1;
	const int SUCCESS = 0;

	const int EMPTY_MESH = 1;
	const int EMPTY_INDEXES = 2;
	const int EMPTY_VERTEXES = 3;

	typedef int ecg_status;
}

#endif