MESSAGE(STATUS "Fetching dependencies (Dynamo):")
MESSAGE(STATUS "-------------------------------")

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
        #GIT_TAG ec5eaa6d6894dea41bbda13f7a896b87e4b35e7f
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

MESSAGE(STATUS "Fetching Ranges ...")
CPMAddPackage(
        NAME ranges 
        GITHUB_REPOSITORY ericniebler/range-v3
        GIT_TAG 0.11.0
        OPTIONS
        "RANGE_V3_DOCS OFF"
        "RANGE_V3_TESTS OFF"
        "RANGE_V3_PERF OFF"
        "RANGE_V3_EXAMPLES OFF"
)
if (ranges_ADDED)
    MESSAGE(STATUS "Fetching  - Ranges done")
    MESSAGE(STATUS "-----------------------")
else()
    MESSAGE(FATAL_ERROR "Could not fetch taskflow")
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

MESSAGE(STATUS "Fetching RandomLib ...")
CPMAddPackage(
        NAME RandomLib 
        GITHUB_REPOSITORY effolkronium/random
        GIT_TAG master
)
if (RandomLib_ADDED)
    MESSAGE(STATUS "Fetching RandomLib - done")
    MESSAGE(STATUS "------------------------")
else()
    MESSAGE(FATAL_ERROR "Could not fetch RandomLib")
endif ()

MESSAGE(STATUS "Fetching boost ... (via boost-cmake)")
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
CPMAddPackage("gh:Orphis/boost-cmake#7f97a08b64bd5d2e53e932ddf80c40544cf45edf@1.71.0")
MESSAGE(STATUS "------------------------------------------------------------")
MESSAGE(STATUS "Fetching ogdf ...")

CPMAddPackage(
        NAME ogdf
        GITHUB_REPOSITORY ogdf/ogdf
        GIT_TAG catalpa-202002
)
if (ogdf_ADDED)
    MESSAGE(STATUS "Fetching ogdf - done")
    MESSAGE(STATUS "--------------------")
else()
    MESSAGE(FATAL_ERROR "Could not fetch odgf")
endif ()

