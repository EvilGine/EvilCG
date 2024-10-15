# CMake minimum version
cmake_minimum_required(VERSION 3.13)

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
find_package(OpenCL CONFIG REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})

# Include directories
include_directories(./inc)

# Source Files
set(SOURCE_FILES
	./src/cl/ecg_host_ctrl.cpp
	./src/cl/ecg_program.cpp

	./src/help/ecg_status.cpp
	./src/help/ecg_geom.cpp

	./src/ecg_api.cpp
)

# Build static library
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_compile_definitions(${PROJECT_NAME} PRIVATE ECG_DLL)

target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::OpenCL)
