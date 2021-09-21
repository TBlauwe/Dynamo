file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "${Dynamo_SOURCE_DIR}/imgui-addons/include/imgui-addons/*.hpp")

add_library(imgui-addons src/imgui-addons.cpp ${HEADER_LIST})
target_include_directories(imgui-addons PUBLIC include)
target_link_libraries(imgui-addons PUBLIC imgui_interface icon_font Boost::boost)
target_compile_features(imgui-addons PUBLIC cxx_std_17)

source_group(
        TREE "${Dynamo_SOURCE_DIR}/imgui-addons/include"
        PREFIX "Header Files"
        FILES ${HEADER_LIST})
