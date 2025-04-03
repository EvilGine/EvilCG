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

set(CMAKE_CXX_STANDARD 20)

# External libraries
find_package(glm REQUIRED)
find_package(OpenCL REQUIRED)
find_package(Eigen3 REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})

# Include directories
include_directories(./inc)

# Source Files
set(SOURCE_FILES
	./src/core/ecg_simplification.cpp
	./src/core/ecg_host_ctrl.cpp
	./src/core/ecg_program.cpp

	./src/help/ecg_allocate.cpp
	./src/help/ecg_handler.cpp
	./src/help/ecg_checks.cpp	
	./src/help/ecg_hasher.cpp
	./src/help/ecg_memory.cpp
	./src/help/ecg_helper.cpp
	./src/help/ecg_status.cpp
	./src/help/ecg_geom.cpp

	./src/ecg_api.cpp
)

# Build static library
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_compile_definitions(${PROJECT_NAME} PRIVATE ECG_DLL)

target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
target_link_libraries(${PROJECT_NAME} PRIVATE Eigen3::Eigen)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::OpenCL)
