# Set minimum Version
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

#C:\Windows\System32\cmd.exe /E:ON /K ""C:\Program Files (x86)\Intel\oneAPI\setvars.bat" intel64 vs2022"

# Set Project Name
set(PROJECT_NAME EvilCG)
find_package(IntelSYCL REQUIRED)

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
	./src/main_ecg.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

target_compile_options(${PROJECT_NAME} PRIVATE -fsycl)
target_link_options(${PROJECT_NAME} PRIVATE -fsycl)