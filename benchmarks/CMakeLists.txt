cmake_minimum_required(VERSION 3.0.0) 
project(benchmarks)

# Set common variables
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories ( ${SOURCE_DIR} )

# -------------------
# Sources
# -------------------

# Set header filess
set(HEADERS
    ${INCLUDE_DIR}/DyC_Benchmarks.h
)

file(GLOB BENCHMARK_SOURCES "${SOURCE_DIR}/*.c" )
foreach( benchmark_source ${BENCHMARK_SOURCES} )
    get_filename_component(file_name ${benchmark_source} NAME)
    string(REPLACE ".c" "" benchmark_name ${file_name} )

    add_executable(${benchmark_name} ${benchmark_source} ${HEADERS})
    target_link_libraries(
        ${benchmark_name}
        PRIVATE
            ${GRAPHBLAS_LIBRARIES}
            ${LAGraph}
            dynamic-closure
    )
    target_include_directories(
        ${benchmark_name}
        PRIVATE
            ${GRAPHBLAS_INCLUDE_DIR}
            ${INCLUDE_DIR}
    )
endforeach( benchmark_source ${BENCHMARK_SOURCES} )



# -------------------
# Assets
# -------------------

file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

