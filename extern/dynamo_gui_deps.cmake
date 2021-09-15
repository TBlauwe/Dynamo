message(STATUS "Fetching imgui")
CPMAddPackage("gh:ocornut/imgui#docking")

if (imgui_ADDED)
    add_library(imgui_int INTERFACE)
    target_include_directories(imgui_int INTERFACE ${imgui_SOURCE_DIR})
endif()

message(STATUS "Fetching IconFontCppHeaders")
CPMAddPackage("gh:juliettef/IconFontCppHeaders#main")

if (IconFontCppHeaders_ADDED)
    add_library(icon_font INTERFACE)
    target_include_directories(icon_font INTERFACE ${IconFontCppHeaders_SOURCE_DIR})
endif()
MESSAGE(STATUS " ------------------------------------")
