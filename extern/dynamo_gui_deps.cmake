MESSAGE(STATUS "Fetching dependencies (Dynamo-GUI):")
MESSAGE(STATUS "-----------------------------------")

# Helper libraries
# ----------------
# Fetch all dependencies using CPM
include(${Dynamo_SOURCE_DIR}/cmake/CPM.cmake)

# Dependencies
# ----------------
message(STATUS "Fetching imgui ...")
CPMAddPackage("gh:ocornut/imgui#docking")
if (imgui_ADDED)
    MESSAGE(STATUS "Fetching imgui - done")
    MESSAGE(STATUS "---------------------")
else()
    MESSAGE(FATAL_ERROR "Could not fetch imgui")
endif ()

CPMAddPackage("gh:Nelarius/imnodes#master")
if (imnodes_ADDED)
    MESSAGE(STATUS "Fetching imnodes - done")
    MESSAGE(STATUS "-----------------------")
else()
    MESSAGE(FATAL_ERROR "Could not fetch imnodes")
endif ()

CPMAddPackage("gh:epezent/implot#master")
if (implot_ADDED)
    MESSAGE(STATUS "Fetching implot - done")
    MESSAGE(STATUS "-----------------------")
else()
    MESSAGE(FATAL_ERROR "Could not fetch implot")
endif ()

# Define only an interface. We do not want to build imgui and its addons yet.
add_library(imgui_interface INTERFACE)
target_include_directories(imgui_interface INTERFACE
        ${imgui_SOURCE_DIR}
        ${imnodes_SOURCE_DIR}
        ${implot_SOURCE_DIR})


message(STATUS "Fetching IconFontCppHeaders ...")
CPMAddPackage("gh:juliettef/IconFontCppHeaders#main")
if (IconFontCppHeaders_ADDED)
    MESSAGE(STATUS "Fetching IconFontCppHeaders - done")
    MESSAGE(STATUS "----------------------------------")
else()
    MESSAGE(FATAL_ERROR "Could not fetch IconFontCppHeaders")
endif ()

add_library(icon_font INTERFACE)
target_include_directories(icon_font INTERFACE ${IconFontCppHeaders_SOURCE_DIR})