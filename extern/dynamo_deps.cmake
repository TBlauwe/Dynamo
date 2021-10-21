MESSAGE(STATUS "Fetching dependencies (Dynamo):")
MESSAGE(STATUS "-------------------------------")

# Helper libraries
# ----------------
# Fetch all dependencies using CPM
include(${Dynamo_SOURCE_DIR}/cmake/CPM.cmake)

# Dependencies
# ------------
MESSAGE(STATUS "Fetching spdlog ...")
CPMAddPackage(
        NAME spdlog
        GITHUB_REPOSITORY gabime/spdlog
        GIT_TAG v1.9.2
        OPTIONS
            "SPDLOG_BUILD_SHARED OFF"
)
if (spdlog_ADDED)
    MESSAGE(STATUS "Fetching spdlog - done")
    MESSAGE(STATUS "----------------------")
    else()
    MESSAGE(FATAL_ERROR "Could not fetch spdlog")
endif ()

MESSAGE(STATUS "Fetching flecs ...")
CPMAddPackage(
        NAME flecs
        GITHUB_REPOSITORY SanderMertens/flecs
        GIT_TAG v2.4.6
        OPTIONS
            "FLECS_STATIC_LIBS ON"
            "FLECS_SHARED_LIBS OFF"
)
if (flecs_ADDED)
    MESSAGE(STATUS "Fetching flecs - done")
    MESSAGE(STATUS "---------------------")
else()
    MESSAGE(FATAL_ERROR "Could not fetch flecs")
endif ()

MESSAGE(STATUS "Fetching taskflow ...")
CPMAddPackage(
        NAME taskflow
        GITHUB_REPOSITORY taskflow/taskflow
        GIT_TAG v3.2.0
        OPTIONS
        "TF_BUILD_TESTS OFF"
        "TF_BUILD_EXAMPLES OFF"
)
if (taskflow_ADDED)
    MESSAGE(STATUS "Fetching taskflow - done")
    MESSAGE(STATUS "------------------------")
else()
    MESSAGE(FATAL_ERROR "Could not fetch taskflow")
endif ()