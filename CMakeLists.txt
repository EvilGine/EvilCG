# Set minimum Version
cmake_minimum_required(VERSION 3.23.0)

# Set Project Name
set(PROJECT_NAME EvilCG)

# Minimal C++ version
set(CMAKE_CXX_STANDARD 20)

# Set Project
project (
	${PROJECT_NAME} 
	VERSION 0.0.0 
	DESCRIPTION "EvilCG - solution for computational graphics"
)

# Subdirectories
add_subdirectory(./EvilCG EvilCG)
add_subdirectory(./Tests Tests)