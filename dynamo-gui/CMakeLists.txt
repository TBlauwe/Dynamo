file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "${Dynamo_SOURCE_DIR}/dynamo-gui/include/dynamo-gui/*.hpp")

add_library(dynamo-gui src/dynamo-gui.cpp ${HEADER_LIST})
target_include_directories(dynamo-gui PUBLIC include)
target_link_libraries(dynamo-gui PUBLIC dynamo imgui_int icon_font)
target_link_libraries(dynamo-gui PRIVATE dynamo)
target_compile_features(dynamo-gui PUBLIC cxx_std_17)

source_group(
        TREE "${Dynamo_SOURCE_DIR}/dynamo-gui/include"
        PREFIX "Header Files"
        FILES ${HEADER_LIST})
