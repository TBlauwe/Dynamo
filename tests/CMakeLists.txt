MESSAGE(STATUS " =======================================")
MESSAGE(STATUS "                 TESTING")
MESSAGE(STATUS " =======================================")

MESSAGE(STATUS "Fetching doctest")
CPMAddPackage(
        NAME doctest
        GITHUB_REPOSITORY onqtam/doctest
        GIT_TAG 2.4.6
)

MESSAGE(STATUS "Building tests")

add_executable(Tests main.cpp simulation.cpp)
target_compile_features(Tests PRIVATE cxx_std_20)
target_link_libraries(Tests PRIVATE dynamo doctest::doctest)
set_target_properties(Tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${Dynamo_SOURCE_DIR}/bin/Tests")

enable_testing()
include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)