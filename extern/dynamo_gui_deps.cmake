message(STATUS "Fetching imgui and addons")
CPMAddPackage("gh:ocornut/imgui#docking")
CPMAddPackage("gh:Nelarius/imnodes#master")
CPMAddPackage("gh:epezent/implot#master")

if (imgui_ADDED AND imnodes_ADDED)
    add_library(imgui_interface INTERFACE)
    target_include_directories(imgui_interface INTERFACE ${imgui_SOURCE_DIR} ${imnodes_SOURCE_DIR} ${implot_SOURCE_DIR})
endif()
MESSAGE(STATUS " ------------------------------------")

message(STATUS "Fetching IconFontCppHeaders")
CPMAddPackage("gh:juliettef/IconFontCppHeaders#main")

if (IconFontCppHeaders_ADDED)
    add_library(icon_font INTERFACE)
    target_include_directories(icon_font INTERFACE ${IconFontCppHeaders_SOURCE_DIR})
endif()
MESSAGE(STATUS " ------------------------------------")

