cmake_minimum_required(VERSION 3.16)
project(Dynamo
        VERSION 0.1
        DESCRIPTION "CMake experiment"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(persona)

add_executable(Dynamo main.cpp)
target_compile_features(Dynamo PRIVATE cxx_std_20)
target_link_libraries(Dynamo PRIVATE persona)
