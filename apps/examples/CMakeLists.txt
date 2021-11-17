file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "${Dynamo_SOURCE_DIR}/examples/include/examples/*.hpp")
file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "${Dynamo_SOURCE_DIR}/examples/src/*.cpp")

add_library(examples ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(examples PUBLIC include)
target_link_libraries(examples PUBLIC dynamo)
target_compile_features(examples PUBLIC cxx_std_20)

source_group(
        TREE "${Dynamo_SOURCE_DIR}/examples/include"
        PREFIX "Header Files"
        FILES ${HEADER_LIST})
