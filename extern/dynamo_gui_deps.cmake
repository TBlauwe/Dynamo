MESSAGE(STATUS "Fetching dependencies (Dynamo-GUI):")
MESSAGE(STATUS "-----------------------------------")

# Helper libraries
# ----------------
# Fetch all dependencies using CPM
include(${Dynamo_SOURCE_DIR}/cmake/CPM.cmake)

# Dependencies
# ----------------
CPMAddPackage("gh:ocornut/imgui#docking")
if (NOT imgui_ADDED)
    MESSAGE(FATAL_ERROR "Could not fetch imgui")
endif ()

CPMAddPackage("gh:Nelarius/imnodes#master")
if (NOT imnodes_ADDED)
    MESSAGE(FATAL_ERROR "Could not fetch imnodes")
endif ()

CPMAddPackage("gh:epezent/implot#v0.12")
if (NOT implot_ADDED)
    MESSAGE(FATAL_ERROR "Could not fetch implot")
endif ()

MESSAGE(STATUS "Fetching boost ... (via boost-cmake)")
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
CPMAddPackage("gh:Orphis/boost-cmake#7f97a08b64bd5d2e53e932ddf80c40544cf45edf@1.71.0")
MESSAGE(STATUS "------------------------------------------------------------")

# Define only an interface. We do not want to build imgui and its addons yet.
add_library(imgui_interface INTERFACE)
target_include_directories(imgui_interface INTERFACE
        ${imgui_SOURCE_DIR}
        ${imnodes_SOURCE_DIR}
        ${implot_SOURCE_DIR})


CPMAddPackage("gh:juliettef/IconFontCppHeaders#main")
if (NOT IconFontCppHeaders_ADDED)
    MESSAGE(FATAL_ERROR "Could not fetch IconFontCppHeaders")
endif ()

add_library(icon_font INTERFACE)
target_include_directories(icon_font INTERFACE ${IconFontCppHeaders_SOURCE_DIR})