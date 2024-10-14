# Set minimum Version
cmake_minimum_required(VERSION 3.23.0)

# Set Project Name
set(PROJECT_NAME Tests)

# Set Project
project (
	${PROJECT_NAME}  LANGUAGES CXX VERSION 0.0.0 
	DESCRIPTION "Tests - tests for EvilCG"
)

add_compile_definitions(GTEST_BUILD)
find_package(GTest CONFIG REQUIRED)
include_directories(../EvilCG/inc)

# Source files
set(SOURCE_FILES
	./src/ecg_api_test.cpp
	./src/ecg_main.cpp
)

enable_testing()
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE EvilCG)
target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

target_compile_options(${PROJECT_NAME} PRIVATE -fsycl)
target_link_options(${PROJECT_NAME} PRIVATE -fsycl)