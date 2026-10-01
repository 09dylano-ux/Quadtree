cmake_minimum_required(VERSION 3.20)
project(SpatialPartitioningQuadtree LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(QuadtreeDemo src/main.cpp include/Quadtree.hpp)
