cmake_minimum_required(VERSION 3.23)
set(EXAMPLE_NAME HullsExample)

project(
	${EXAMPLE_NAME}
	VERSION 0.0.0
)

find_package(OpenCL REQUIRED)
include_directories(./../../EvilCG/inc)

set(SOURCE_FILES
	./main.cpp
)

add_executable(${EXAMPLE_NAME} ${SOURCE_FILES})
target_link_libraries(${EXAMPLE_NAME} PRIVATE EvilCG)
target_link_libraries(${EXAMPLE_NAME} PRIVATE OpenCL::OpenCL)

add_custom_command(TARGET ${EXAMPLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:EvilCG>
            $<TARGET_FILE_DIR:${EXAMPLE_NAME}>
)

add_custom_command(TARGET ${EXAMPLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/../rabbit.obj
            $<TARGET_FILE_DIR:${EXAMPLE_NAME}>
)