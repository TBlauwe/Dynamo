# =================================================
# Dynamo - C++ library to simulate cognitive agents
# =================================================

# CMake version
# -------------
cmake_minimum_required(VERSION 3.12...3.21)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# Helper libraries
# -------------
include(CMakePrintHelpers)

# Project information
# -------------------
project(Dynamo
        VERSION 0.1
        DESCRIPTION "C++ library to simulate cognitive agents."
        HOMEPAGE_URL "https://github.com/tblauwe/dynamo"
        LANGUAGES CXX
)


# Meta-data
# ---------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)
set(FETCHCONTENT_QUIET TRUE)

# Build options
# -------------
option(DYN_BUILD_DYNAMO_GUI     "Build a library using imgui for debugging a simulation." ON)
option(DYN_BUILD_APPS           "Build several applications to showcase the library." ON)
option(DYN_BUILD_BENCHMARKS     "Build benchmarks using google benchmarks." ON)
option(DYN_BUILD_DOCS           "Build documentation using doxygen (must be installed)." ON)
option(DYN_BUILD_TESTS          "Build tests using doctest." ON)
option(DYN_ENABLE_TEST_COVERAGE "Enable test coverage. --UNUSED FEATURE--" OFF)
MESSAGE(STATUS "Build options :")
MESSAGE(STATUS "===============")
cmake_print_variables(DYN_BUILD_DYNAMO_GUI)
cmake_print_variables(DYN_BUILD_APPS)
cmake_print_variables(DYN_BUILD_BENCHMARKS)
cmake_print_variables(DYN_BUILD_DOCS)
cmake_print_variables(DYN_BUILD_TESTS)
cmake_print_variables(DYN_ENABLE_TEST_COVERAGE)
MESSAGE(STATUS "")

# Dependencies
# ------------
MESSAGE(STATUS "Dependencies")
MESSAGE(STATUS "============")
include(extern/dynamo_deps.cmake) # dynamo dependencies
if(DYN_BUILD_DYNAMO_GUI)
    MESSAGE(STATUS "")
    include(extern/dynamo_gui_deps.cmake) # dynamo-gui dependencies
endif()

# Building
# --------
MESSAGE(STATUS "")
MESSAGE(STATUS "Building targets")
MESSAGE(STATUS "================")
MESSAGE(STATUS "dynamo")
add_subdirectory(dynamo)

if(DYN_BUILD_DYNAMO_GUI)
    MESSAGE(STATUS "imgui-addons")
    add_subdirectory(imgui-addons)

    MESSAGE(STATUS "dynamo-gui")
    add_subdirectory(dynamo-gui)
endif()

if(DYN_BUILD_APPS)
    MESSAGE(STATUS "applications")
    add_subdirectory(apps)
endif()

if(DYN_BUILD_BENCHMARKS)
    MESSAGE(STATUS "")
    MESSAGE(STATUS "Building benchmarks")
    MESSAGE(STATUS "===================")
    add_subdirectory(benchmarks)
endif()

# ==================
#      TESTING
# ==================
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    if(DYN_BUILD_TESTS)
        include(CTest)
        MESSAGE(STATUS "")
        MESSAGE(STATUS "Building tests")
        MESSAGE(STATUS "==============")
        add_subdirectory(tests)
    endif()
endif()

# =========================
#       DOCUMENTATION
# =========================
if(DYN_BUILD_DOCS)
    MESSAGE(STATUS "")
    MESSAGE(STATUS "Building documentation")
    MESSAGE(STATUS "======================")
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # set input and output files
        add_custom_target(build-time-make-directory ALL
                COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/bin/docs)

        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_SOURCE_DIR}/bin/docs/Doxyfile.out)

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

        # note the option ALL which allows to build the docs together with the application
        add_custom_target(doc_doxygen
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT "Generating API documentation with Doxygen"
                VERBATIM )

        MESSAGE(STATUS "Done !")
        MESSAGE(STATUS "Location : ${CMAKE_CURRENT_SOURCE_DIR}/bin/docs/")
    else (DOXYGEN_FOUND)
        MESSAGE(SEND_ERROR "Doxygen need to be installed to generate the doxygen documentation : https://www.doxygen.nl/download.html")
    endif()
endif()

MESSAGE(STATUS "")
MESSAGE(STATUS "Wrapping")
MESSAGE(STATUS "========")