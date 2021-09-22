# Fetch all dependencies using either CPM or git submodules
# FetchContent is preferred over git submodules when appropriate
include(CMakePrintHelpers)
include(${Dynamo_SOURCE_DIR}/cmake/CPM.cmake)

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
        GIT_TAG v2.4.7
        OPTIONS
            "FLECS_STATIC_LIBS ON"
            "FLECS_SHARED_LIBS OFF"
)
MESSAGE(STATUS " ------------------------------------")

MESSAGE(STATUS "Fetching Boost via boost-cmake (could take some time)")
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
CPMAddPackage("gh:Orphis/boost-cmake#7f97a08b64bd5d2e53e932ddf80c40544cf45edf@1.71.0")
MESSAGE(STATUS " ------------------------------------")

# >>>>>>>>>>>>>>>>> Git submodules <<<<<<<<<<<<<<<<<
find_package(Git QUIET)
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