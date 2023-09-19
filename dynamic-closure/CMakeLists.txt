cmake_minimum_required(VERSION 3.0.0) 
project(dynamic-closure)

set(SOURCE_BINARY_NAME dynamic-closure)
set(TEST_BINARY_NAME dynamic-closure-test)

# Set common variables
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)

# -------------------
# Sources
# -------------------

# Set source files
set(SOURCES
    # Algorithms
    # ${SOURCE_DIR}/algorithms/DyC_DecreaseGeneral.c
    ${SOURCE_DIR}/algorithms/DyC_GeneralizedFloydWarshall.c
    ${SOURCE_DIR}/algorithms/DyC_DecreaseIdempotent.c
    ${SOURCE_DIR}/algorithms/DyC_DistrustMatrix.c
    ${SOURCE_DIR}/algorithms/DyC_IncreaseColumn.c
    ${SOURCE_DIR}/algorithms/DyC_IncreaseRow.c

    # Utilities
    ${SOURCE_DIR}/utilities/DyC_Semiring.c
    ${SOURCE_DIR}/utilities/DyC_Init.c
    ${SOURCE_DIR}/utilities/DyC_Finalize.c
)

# Set header files
set(HEADERS
    ${INCLUDE_DIR}/DynamicClosure.h
    ${INCLUDE_DIR}/DyC_Internal.h
)

add_library(${SOURCE_BINARY_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(
    ${SOURCE_BINARY_NAME}
    PRIVATE
        ${GRAPHBLAS_LIBRARIES}
)
target_include_directories(
    ${SOURCE_BINARY_NAME}
    PRIVATE
        ${GRAPHBLAS_INCLUDE_DIR}
    PUBLIC
        ${INCLUDE_DIR}
)

# -------------------
# Tests
# -------------------

set(TESTS
    ${TEST_DIR}/semiring-test.cc
    ${TEST_DIR}/distrust-matrix-test.cc
    ${TEST_DIR}/decrease-idempotent-test.cc
    ${TEST_DIR}/generalized-floyd-warshall-test.cc
    ${TEST_DIR}/increase-column-test.cc
    ${TEST_DIR}/increase-row-test.cc
)

add_executable(${TEST_BINARY_NAME} ${TESTS})
target_link_libraries(
    ${TEST_BINARY_NAME}
    PRIVATE
        ${SOURCE_BINARY_NAME}
        ${GRAPHBLAS_LIBRARIES}
        ${LAGraph}
        GTest::gtest_main
)
target_include_directories(
    ${TEST_BINARY_NAME}
    PRIVATE
        ${GRAPHBLAS_INCLUDE_DIR}
)

# -------------------
# Assets
# -------------------

file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# -------------------
# GTest
# -------------------

include(GoogleTest)
gtest_discover_tests(${TEST_BINARY_NAME})

