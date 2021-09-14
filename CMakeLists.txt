cmake_minimum_required(VERSION 3.12...3.21)
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
        HOMEPAGE_URL "https://github.com/tblauwe/dynamo"
        LANGUAGES CXX
)

# =======================
#      CONFIGURATION
# =======================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

set(FETCHCONTENT_QUIET TRUE)
option(BUILD_DYNAMO_GUI "Build imgui lib for dynamo" ON)
option(BUILD_APPS       "Build applications" ON)

# =======================
#      DEPENDENCIES
# =======================
MESSAGE(STATUS " =======================================")
MESSAGE(STATUS "    Fetching third-party dependencies")
MESSAGE(STATUS " =======================================")
MESSAGE(STATUS " [----- Dynamo dependencies -----]")
include(extern/dynamo_deps.cmake)    # fetch dependencies
if(BUILD_DYNAMO_GUI)
    MESSAGE(STATUS " [----- Dynamo-GUI dependencies -----]")
    include(extern/dynamo_gui_deps.cmake)    # fetch dependencies
endif()

# ==================
#      BUILDING
# ==================
MESSAGE(STATUS " =======================================")
MESSAGE(STATUS "                 BUILDING")
MESSAGE(STATUS " =======================================")
MESSAGE(STATUS " Building Dynamo")
add_subdirectory(dynamo)   # library

if(BUILD_DYNAMO_GUI)
    MESSAGE(STATUS " Building Dynamo-GUI ")
    add_subdirectory(dynamo-gui)
else()
    MESSAGE(STATUS " Skipping Dynamo-GUI ")
endif()

if(BUILD_APPS)
    MESSAGE(STATUS " Building applications ")
    add_subdirectory(apps)
else()
    MESSAGE(STATUS " Skipping applications ")
endif()

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
