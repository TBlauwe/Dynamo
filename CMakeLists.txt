cmake_minimum_required(VERSION 3.7...3.21)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# ===================
#      META-DATA
# ===================
project(Dynamo
        VERSION 0.1
        DESCRIPTION "CMake experiment"
        LANGUAGES CXX
)

# =======================
#      CONFIGURATION
# =======================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FETCHCONTENT_QUIET TRUE)
add_subdirectory(extern)    # fetch dependencies

# ==================
#      BUILDING
# ==================
MESSAGE(STATUS " =======================================")
MESSAGE(STATUS "                 BUILDING")
MESSAGE(STATUS " =======================================")
MESSAGE(STATUS " Building Persona")
add_subdirectory(persona)   # library

MESSAGE(STATUS " Building Dynamo ")
add_executable(Dynamo main.cpp)
target_compile_features(Dynamo PRIVATE cxx_std_20)
target_link_libraries(Dynamo PRIVATE persona)

# ==================
#      TESTING
# ==================
#if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
#    MESSAGE(STATUS " =======================================")
#    MESSAGE(STATUS "                 TESTING")
#    MESSAGE(STATUS " =======================================")
#    include(CTest)
#    list(APPEND CMAKE_MODULE_PATH ${doctest_SOURCE_DIR}/scripts/cmake)
#    include(doctest)
#    doctest_discover_tests(Dynamo)
#endif()

MESSAGE(STATUS " =======================================")
MESSAGE(STATUS "                 WRAPPING")
MESSAGE(STATUS " =======================================")
