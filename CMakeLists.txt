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
option(BUILD_DYNAMO_GUI     "Build imgui lib for dynamo" ON)
option(BUILD_APPS           "Build applications" ON)
option(BUILD_BENCHMARKS     "Build benchmarks" ON)
option(BUILD_DOCS           "Build documentation" ON)
option(BUILD_TESTS          "Build tests" ON)
option(ENABLE_TEST_COVERAGE "Enable test coverage" ON)

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
    add_subdirectory(imgui-addons)
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
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    if(BUILD_TESTS)
        add_subdirectory(apps/tests)
    endif()
endif()

# =========================
#       DOCUMENTATION
# =========================
if(BUILD_DOCS)
    MESSAGE(STATUS " =======================================")
    MESSAGE(STATUS "                 DOCUMENTATION")
    MESSAGE(STATUS " =======================================")
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # set input and output files
        add_custom_target(build-time-make-directory ALL
                COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/bin/docs)

        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_SOURCE_DIR}/bin/docs/Doxyfile.out)

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")

        # note the option ALL which allows to build the docs together with the application
        add_custom_target(doc_doxygen
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT "Generating API documentation with Doxygen"
                VERBATIM )
    else (DOXYGEN_FOUND)
        message("Doxygen need to be installed to generate the doxygen documentation : https://www.doxygen.nl/download.html")
    endif (DOXYGEN_FOUND)
endif()

MESSAGE(STATUS " =======================================")
MESSAGE(STATUS "                 WRAPPING")
MESSAGE(STATUS " =======================================")
