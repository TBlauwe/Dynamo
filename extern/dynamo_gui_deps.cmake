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

CPMAddPackage("gh:epezent/implot#master")
if (NOT implot_ADDED)
    MESSAGE(FATAL_ERROR "Could not fetch implot")
endif ()

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

MESSAGE(STATUS "Fetching ogdf ...")
CPMAddPackage(
        NAME ogdf
        GITHUB_REPOSITORY ogdf/ogdf
        GIT_TAG catalpa-202002
)
if (ogdf_ADDED)
    MESSAGE(STATUS "Fetching ogdf - done")
    MESSAGE(STATUS "--------------------")
else()
    MESSAGE(FATAL_ERROR "Could not fetch odgf")
endif ()
