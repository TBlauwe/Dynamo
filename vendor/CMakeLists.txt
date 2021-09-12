include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

MESSAGE(STATUS " >>>>> Fetching third-party dependencies <<<<< ")
MESSAGE(STATUS "Fetching doctest ...")
FetchContent_Declare(
        doctest
        GIT_REPOSITORY "https://github.com/onqtam/doctest"
        GIT_TAG 4d8716f1efc1d14aa736ef52ee727bd4204f4c40 # 2.4.6
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(doctest)

MESSAGE(STATUS "Fetching spdlog...")
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
        GIT_TAG eb3220622e73a4889eee355ffa37972b3cac3df5 # 1.9.2
        GIT_PROGRESS TRUE
        CMAKE_ARGS -DSPDLOG_BUILD_SHARED=OFF
)
FetchContent_MakeAvailable(spdlog)

MESSAGE(STATUS "Fetching flecs...")
set(FLECS_STATIC_LIBS ON)
#set(FLECS_PIC ON)
set(FLECS_SHARED_LIBS OFF)
#set(FLECS_DEVELOPER_WARNINGS OFF)
FetchContent_Declare(
        flecs
        GIT_REPOSITORY "https://github.com/SanderMertens/flecs.git"
        GIT_TAG df399a02c2e0e86f71c8ff6ec130ebf4a27965b0 # 2.4.6
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(flecs)
