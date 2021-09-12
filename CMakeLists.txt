cmake_minimum_required(VERSION 3.7...3.21)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(Dynamo
        VERSION 0.1
        DESCRIPTION "CMake experiment"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(vendor)
add_subdirectory(persona)

MESSAGE(STATUS " >>>>> Building Dynamo <<<<< ")
add_executable(Dynamo main.cpp)
target_compile_features(Dynamo PRIVATE cxx_std_20)
target_link_libraries(Dynamo PRIVATE persona)

#MESSAGE(STATUS " >>>>> Adding tests <<<<< ")
#include(CTest)
#list(APPEND CMAKE_MODULE_PATH ${doctest_SOURCE_DIR}/scripts/cmake)
#include(doctest)
#doctest_discover_tests(Dynamo)