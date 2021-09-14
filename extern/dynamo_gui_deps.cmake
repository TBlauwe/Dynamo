message(STATUS "Fetching imgui")
CPMAddPackage("gh:ocornut/imgui#docking")

if (imgui_ADDED)
    add_library(imgui_int INTERFACE)
    target_include_directories(imgui_int INTERFACE ${imgui_SOURCE_DIR})
endif()
MESSAGE(STATUS " ------------------------------------")
