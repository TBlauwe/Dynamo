MESSAGE(STATUS " =======================================")
MESSAGE(STATUS "                 BENCHMARKS")
MESSAGE(STATUS " =======================================")

MESSAGE(STATUS "Fetching google/benchmark")
CPMAddPackage(
        NAME benchmark
        GITHUB_REPOSITORY google/benchmark
        VERSION 1.6.0
        OPTIONS "BENCHMARK_ENABLE_TESTING Off"
)
if(benchmark_ADDED)
    # enable c++11 to avoid compilation errors
    set_target_properties(benchmark PROPERTIES CXX_STANDARD 11)
endif()
MESSAGE(STATUS " ------------------------------------")


MESSAGE(STATUS ">>>>> Building benchmarks")

add_executable(Benchmarks benchmark.cpp)
target_compile_features(Benchmarks PRIVATE cxx_std_20)
target_link_libraries(Benchmarks PRIVATE dynamo benchmark::benchmark)
set_target_properties(Benchmarks PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${Dynamo_SOURCE_DIR}/bin/Benchmarks")