include(FetchContent)

MESSAGE(STATUS "Fetching doctest")
FetchContent_Declare(
        DocTest
        GIT_REPOSITORY "https://github.com/onqtam/doctest"
        GIT_TAG master
)
FetchContent_MakeAvailable(DocTest)

MESSAGE(STATUS "Fetching spdlog")
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
        GIT_TAG master
)
FetchContent_MakeAvailable(spdlog)
