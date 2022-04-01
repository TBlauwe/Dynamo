MESSAGE(STATUS "Fetching dependencies (Dynamo):")
MESSAGE(STATUS "-------------------------------")

# Dependencies
# ------------
MESSAGE(STATUS "Fetching flecs ...")
CPMAddPackage(
        NAME flecs
        GITHUB_REPOSITORY SanderMertens/flecs
        #GIT_TAG bd17c61502a3272fcbf4f11f941d57c9ac485642
        GIT_TAG master
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
        GIT_TAG v3.3.0
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

