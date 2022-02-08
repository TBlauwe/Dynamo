file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "${Dynamo_SOURCE_DIR}/dynamo/include/dynamo/*.hpp")

add_library(dynamo src/simulation.cpp src/core.cpp src/flow.cpp ${HEADER_LIST} )
target_include_directories(dynamo PUBLIC include)
target_link_libraries(dynamo PUBLIC Taskflow spdlog::spdlog flecs_static OGDF Boost::boost range-v3 effolkronium_random)
target_compile_features(dynamo PUBLIC cxx_std_17)

source_group(
        TREE "${Dynamo_SOURCE_DIR}/dynamo/include"
        PREFIX "Header Files"
        FILES ${HEADER_LIST})
