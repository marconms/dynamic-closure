cmake_minimum_required(VERSION 3.5.0)
project(thesis-material VERSION 0.1.0)

set(FIND_GRAPHBLAS_PATH "/usr/local/lib/cmake/SuiteSparse" CACHE STRING "Location of the FindGraphBLAS.cmake file" )
list(APPEND CMAKE_MODULE_PATH ${FIND_GRAPHBLAS_PATH})

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  DOWNLOAD_EXTRACT_TIMESTAMP NEW
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

# GraphBLAS and LAGraph
find_package(GraphBLAS 7 REQUIRED MODULE)
find_library(LAGraph lagraph)

# Add projects
add_subdirectory(dynamic-closure)
add_subdirectory(benchmarks)

