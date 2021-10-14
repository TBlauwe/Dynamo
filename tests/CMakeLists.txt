CPMAddPackage(
        NAME doctest
        GITHUB_REPOSITORY onqtam/doctest
        GIT_TAG 2.4.6
)
if(NOT doctest_ADDED)
    MESSAGE(FATAL_ERROR "Could not fetch doctest")
endif ()

add_executable(Tests main.cpp simulation.cpp)
target_compile_features(Tests PRIVATE cxx_std_20)
target_link_libraries(Tests PRIVATE dynamo doctest::doctest)
set_target_properties(Tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${Dynamo_SOURCE_DIR}/bin/Tests")
add_test(NAME Dynamo::test COMMAND Tests)

# include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake) # Doesn't seem to work :/