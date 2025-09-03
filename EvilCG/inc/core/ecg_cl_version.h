#ifndef ECG_CL_VERSION_H
#define ECG_CL_VERSION_H

#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120

#if defined(SYCL_BUILD)
	#include <sycl/sycl.hpp>
	// #elif defined(OPENCL_BUILD)
#else
	#ifdef __APPLE__
		#include <core/ecg_cl_version.h>	
		#include <OpenCL/opencl.h>
	#else
		#include <core/ecg_cl_version.h>
		#include <CL/opencl.hpp>
	#endif
#endif

#endif