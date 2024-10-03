cmake_minimum_required(VERSION 3.10)
project(CacheServer)

set(CMAKE_CXX_STANDARD 17)

# Include directories for Crow and nlohmann/json
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Find Boost
find_package(Boost REQUIRED COMPONENTS system thread)

# Add the executable target first
add_executable(cache_server src/server.cpp)

# Link Boost libraries after the target has been defined
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(cache_server ${Boost_LIBRARIES})  # Link Boost libraries
endif()
