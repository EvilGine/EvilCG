#ifndef API_DEFINE_H
#define API_DEFINE_H

#ifdef _WIN32
	#ifdef ECG_DLL
		#define ECG_API __declspec(dllexport)
	#else
		#define ECG_API __declspec(dllimport)
	#endif
#elif __GNUC__
	#ifdef ECG_DLL
		#define ECG_API __attribute__((visibility("default")))
	#else
		#define ECG_API
	#endif
#else
	#pragma message("Warning: Unknown platform. ECG_API is not defined.")
	#define ECG_API
#endif

#endif