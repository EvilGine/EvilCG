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
find_package(Boost REQUIRED)
find_package(OpenCL REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(spdlog REQUIRED)
find_package(fmt CONFIG REQUIRED)

include_directories(${Boost_INCLUDE_DIRS}) 
include_directories(${OpenCL_INCLUDE_DIRS})

# Include directories
include_directories(./inc)

# Definitions
add_compile_definitions(${PROJECT_NAME} PRIVATE ECG_USE_SPDLOG)

# Source Files
set(SOURCE_FILES
	./src/core/ecg_host_ctrl.cpp
	./src/core/ecg_program.cpp

	./src/help/ecg_overloads.cpp
	./src/help/ecg_allocate.cpp
	./src/help/ecg_hasher.cpp
	./src/help/ecg_helper.cpp
	./src/help/ecg_checks.cpp
	./src/help/ecg_status.cpp
	./src/help/ecg_logger.cpp
	./src/help/ecg_math.cpp
	./src/help/ecg_geom.cpp
	./src/help/ecg_mem.cpp

	./src/impl/ecg_api_simplification.cpp
	./src/impl/ecg_api_intersections.cpp
	./src/impl/ecg_api_convert.cpp
	./src/impl/ecg_api_import.cpp
	./src/impl/ecg_api_export.cpp
	./src/impl/ecg_api_hulls.cpp

	./src/ecg_api.cpp
)

# Build static library
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_compile_definitions(${PROJECT_NAME} PRIVATE ECG_DLL)

target_link_libraries(${PROJECT_NAME} PUBLIC  Boost::boost)
target_link_libraries(${PROJECT_NAME} PUBLIC  Eigen3::Eigen)

target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::OpenCL)    
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt-header-only)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog_header_only)

set(ECG_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/help/ecg_status.h
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/help/ecg_geom.h

    ${CMAKE_CURRENT_SOURCE_DIR}/inc/ecg_api_define.h
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/ecg_global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/ecg_api.h
)

# Base path to library
set(ECG_INC_DST $<TARGET_FILE_DIR:${PROJECT_NAME}>/inc)

# Make folders
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ECG_INC_DST}/help
)

# Copy headers
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/help/ecg_status.h
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/help/ecg_geom.h
        ${ECG_INC_DST}/help
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/ecg_api_define.h
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/ecg_global.h
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/ecg_api.h
        ${ECG_INC_DST}
)