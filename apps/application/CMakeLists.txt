MESSAGE(STATUS "applications")
MESSAGE(STATUS "------------")
find_package(Vulkan REQUIRED FATAL_ERROR)

CPMAddPackage(
        NAME glfw
        GITHUB_REPOSITORY glfw/glfw
        GIT_TAG 3.3.2
        OPTIONS
        "GLFW_BUILD_TESTS OFF"
        "GLFW_BUILD_EXAMPLES OFF"
        "GLFW_BUILD_DOCS OFF"
)
if(glfw_ADDED)
else()
    MESSAGE(FATAL_ERROR "Could not fetch glfw3")
endif ()
MESSAGE(STATUS "------------")


# Now we can build imgui and its addons
FILE(GLOB imgui_SOURCES ${imgui_SOURCE_DIR}/*.cpp)
list(APPEND imgui_SOURCES ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp)
list(APPEND imgui_SOURCES ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp)
list(APPEND imgui_SOURCES ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp)
list(APPEND imgui_SOURCES ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp)
list(APPEND imgui_SOURCES ${imnodes_SOURCE_DIR}/imnodes.cpp)
list(APPEND imgui_SOURCES ${implot_SOURCE_DIR}/implot.cpp)
list(APPEND imgui_SOURCES ${implot_SOURCE_DIR}/implot_demo.cpp)
list(APPEND imgui_SOURCES ${implot_SOURCE_DIR}/implot_items.cpp)
add_library(imgui STATIC ${imgui_SOURCES})
target_link_libraries(imgui PUBLIC glfw Vulkan::Vulkan)
target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imnodes_SOURCE_DIR}
        ${implot_SOURCE_DIR}
        )

add_library(app
        src/application.cpp
        include/application/application.h)
target_include_directories(app PUBLIC include)
target_link_libraries(app PUBLIC dynamo-gui imgui icon_font)
target_compile_features(dynamo-gui PUBLIC cxx_std_17)
set_target_properties(app PROPERTIES FONTS_DIR ${Dynamo_SOURCE_DIR}/apps/application/fonts)

function(add_fonts_to_output_dir target)
    get_target_property(fonts_dir app FONTS_DIR)
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory fonts)
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${fonts_dir} $<TARGET_FILE_DIR:${target}>/fonts)
endfunction()