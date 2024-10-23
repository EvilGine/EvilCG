# Set minimum Version
cmake_minimum_required(VERSION 3.23.0)

# Set Project Name
set(PROJECT_NAME Tests)

# Set Project
project (
	${PROJECT_NAME}  LANGUAGES CXX VERSION 0.0.0 
	DESCRIPTION "Tests - tests for EvilCG"
)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(GTEST_BUILD)
find_package(GTest CONFIG REQUIRED)
include_directories(../EvilCG/inc)
include_directories(./inc)

find_package(OpenCL CONFIG REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})

# Source files
set(SOURCE_FILES
	./src/ecg_api_test.cpp
    ./src/ecg_meshes.cpp
	./src/ecg_main.cpp
)

enable_testing()
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE EvilCG)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::OpenCL)
target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

# Copy EvilCG.dll or EvilCG.so after build
if (WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:EvilCG>"  # Source: EvilCG DLL
        $<TARGET_FILE_DIR:${PROJECT_NAME}>  # Destination: Test executable directory
    )
elseif (UNIX)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:EvilCG>"  # Source: EvilCG SO
        $<TARGET_FILE_DIR:${PROJECT_NAME}>  # Destination: Test executable directory
    )
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory  
    "${CMAKE_SOURCE_DIR}/Models" 
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Models"
)