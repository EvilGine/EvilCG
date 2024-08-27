#ifndef ECG_GLOBAL_H
#define ECG_GLOBAL_H

#include <glm/glm.hpp>
#if defined(SYCL_BUILD)
	#include <sycl/sycl.hpp>
// #elif defined(OPENCL_BUILD)
#else
	#ifdef __APPLE__
		#include <OpenCL/opencl.h>
	#else
		#include <CL/opencl.hpp>
	#endif
#endif

#include <thread>
#include <mutex>

#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <utility>
#include <ranges>
#include <vector>
#include <cmath>
#include <list>
#include <map>
#include <set>
#include <any>

#endif