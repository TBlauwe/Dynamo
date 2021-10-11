add_subdirectory(application)
add_subdirectory(sandbox-console)
add_subdirectory(sandbox-gui)

if(BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
else()
    MESSAGE(STATUS " Skipping benchmarks")
endif()
