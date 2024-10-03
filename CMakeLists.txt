# CMake minimum version required
cmake_minimum_required(VERSION 3.10)

# Project name
project(CacheApp)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Boost libraries
find_package(Boost REQUIRED COMPONENTS system thread)

# Include directories for Boost and Crow
include_directories(${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add the executable target
add_executable(cache_server src/server.cpp)

# Link Boost libraries
target_link_libraries(cache_server ${Boost_LIBRARIES})

# Optionally add additional flags for optimization
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(cache_server PRIVATE -O3)
endif()

# Include directories for Crow
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/crow_all.h)

# If you have a specific path to the Crow headers, uncomment this line:
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/path_to_crow_include)
