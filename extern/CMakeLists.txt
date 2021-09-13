# Fetch all dependencies using either CPM or git submodules
# FetchContent is preferred over git submodules when appropriate
include(CMakePrintHelpers)

MESSAGE(STATUS " =======================================")
MESSAGE(STATUS "    Fetching third-party dependencies")
MESSAGE(STATUS " =======================================")
# >>>>>>>>>>>>>>>>> CPM <<<<<<<<<<<<<<<<<
# doctest
# spdlog
# flecs

# see https://github.com/cpm-cmake/CPM.cmake for more info
include(../cmake/CPM.cmake)
MESSAGE(STATUS " >>>>> using CPM <<<<<")

MESSAGE(STATUS " ------------------------------------")
MESSAGE(STATUS "Fetching doctest")
CPMAddPackage(
        NAME doctest
        GITHUB_REPOSITORY onqtam/doctest
        GIT_TAG 2.4.6
)

MESSAGE(STATUS " ------------------------------------")
MESSAGE(STATUS "Fetching spdlog")
CPMAddPackage(
        NAME spdlog
        GITHUB_REPOSITORY gabime/spdlog
        GIT_TAG v1.9.2
        OPTIONS
            "SPDLOG_BUILD_SHARED OFF"
)

MESSAGE(STATUS " ------------------------------------")
MESSAGE(STATUS "Fetching flecs")
CPMAddPackage(
        NAME flecs
        GITHUB_REPOSITORY SanderMertens/flecs
        GIT_TAG v2.4.6
        OPTIONS
            "FLECS_STATIC_LIBS ON"
            "FLECS_SHARED_LIBS OFF"
)

# >>>>>>>>>>>>>>>>> Git submodules <<<<<<<<<<<<<<<<<
find_package(Git QUIET)
MESSAGE(STATUS " >>>>> using git submodules <<<<<")
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.gitmodules")
    # Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS " >>>>> Submodule update <<<<<")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    else()
        message(STATUS "Skipping submodule update (GIT_SUBMODULE OFF)")
    endif()
else()
    message(STATUS "No git submodule detected (no .gitmodules file detected in project's root) --> skipping")
endif()

if(EXISTS "${PROJECT_SOURCE_DIR}/extern/${SUBDIR}" AND NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/${SUBDIR}CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()