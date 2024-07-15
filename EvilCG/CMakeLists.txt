# Set minimum Version
if(CUSTOM_BUILD_TYPE STREQUAL "sycl")
    message("SYCL build")
    if (CMAKE_HOST_WIN32)
        # need CMake 3.25.0+ for IntelLLVM support of target link properties on Windows
        cmake_minimum_required(VERSION 3.25)
        if( NOT DEFINED CMAKE_C_COMPILER)
          set(CMAKE_C_COMPILER icx-cl CACHE STRING "Compiler icx-cl" FORCE)
        endif()
        if( NOT DEFINED CMAKE_CXX_COMPILER)
          set(CMAKE_CXX_COMPILER icx-cl CACHE STRING "Compiler icx-cl" FORCE)
        endif()
        if( NOT DEFINED CMAKE_Fortran_COMPILER)
          set(CMAKE_Fortran_COMPILER ifx CACHE STRING "Compiler ifx" FORCE)
        endif()
    else()
        # CMake 3.22.1 is the minimum recommended for IntelLLVM on Linux
        cmake_minimum_required(VERSION 3.22.1)
        if( NOT DEFINED CMAKE_C_COMPILER)
          set(CMAKE_C_COMPILER icx CACHE STRING "Compiler icx" FORCE)
        endif()
        if( NOT DEFINED CMAKE_CXX_COMPILER)
          set(CMAKE_CXX_COMPILER icpx CACHE STRING "Compiler icpx" FORCE)
        endif()
        if( NOT DEFINED CMAKE_Fortran_COMPILER)
          set(CMAKE_Fortran_COMPILER ifx CACHE STRING "Compiler ifx" FORCE)
        endif()
    endif()
    
    add_compile_definitions(SYCL_BUILD)
    find_package(IntelSYCL REQUIRED)
elseif(CUSTOM_BUILD_TYPE STREQUAL "opencl")
    message("OpenCL build")    
    add_compile_definitions(OPENCL_BUILD)
    cmake_minimum_required(VERSION 3.13)

    find_package(OpenCL CONFIG REQUIRED)
    if(NOT OpenCL_FOUND)
        message(FATAL_ERROR "OpenCL not found")
    endif()
endif()

# Set Project Name
set(PROJECT_NAME EvilCG)
message("Build type: ${CUSTOM_BUILD_TYPE}")

# Set Project
project (
	${PROJECT_NAME} LANGUAGES CXX VERSION 0.0.0 
	DESCRIPTION "EvilCG - main computatuional library"
)

# External libraries
find_package(glm CONFIG REQUIRED)

# Include directories
include_directories(./inc)

# Source Files
set(SOURCE_FILES
	./src/ecg_compute.cpp
	./src/ecg_debug.cpp
	./src/ecg_core.cpp
	./src/ecg_lod.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

if(CUSTOM_BUILD_TYPE STREQUAL "sycl")
    target_compile_options(${PROJECT_NAME} PRIVATE -fsycl)
    target_link_options(${PROJECT_NAME} PRIVATE -fsycl)
elseif(CUSTOM_BUILD_TYPE STREQUAL "opencl") 
    include_directories(${OpenCL_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::OpenCL)
endif()