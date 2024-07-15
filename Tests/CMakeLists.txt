# Set minimum Version
cmake_minimum_required(VERSION 3.23.0)

# Set Project Name
set(PROJECT_NAME Tests)

# Set Project
project (
	${PROJECT_NAME}  LANGUAGES CXX VERSION 0.0.0 
	DESCRIPTION "Tests - tests for EvilCG"
)

find_package(GTest CONFIG REQUIRED)
include_directories(../EvilCG/inc)

# Source files
set(SOURCE_FILES
	./src/ecg_compute_test.cpp
	./src/ecg_debug_test.cpp
	./src/ecg_core_test.cpp
	./src/ecg_test.cpp
)

enable_testing()
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE EvilCG)
target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

target_compile_options(${PROJECT_NAME} PRIVATE -fsycl)
target_link_options(${PROJECT_NAME} PRIVATE -fsycl)