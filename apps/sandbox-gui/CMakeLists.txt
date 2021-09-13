MESSAGE(STATUS ">>>>> Building Sandbox GUI")

set(BUILD_SHARED_LIBS OFF)
include(../../cmake/CPM.cmake)

message(STATUS "Fetching SFML...")
CPMAddPackage(
        NAME SFML
        GITHUB_REPOSITORY SFML/SFML
        GIT_TAG 2.5.1
        OPTIONS
            "SFML_BUILD_AUDIO FALSE"
            "SFML_BUILD_NETWORK FALSE"
)

message(STATUS "Fetching imgui...")
CPMAddPackage("gh:ocornut/imgui#docking")

message(STATUS "Fetching imgui-sfml...")
CPMAddPackage(
        NAME imgui-sfml
        GITHUB_REPOSITORY eliasdaler/imgui-sfml
        GIT_TAG v2.3
        OPTIONS
            "IMGUI_DIR ${imgui_SOURCE_DIR}"
            "IMGUI_SFML_FIND_SFML OFF"
            "IMGUI_SFML_IMGUI_DEMO OFF"
)

add_executable(Sandbox-GUI main.cpp)
target_link_libraries(Sandbox-GUI PRIVATE ImGui-SFML::ImGui-SFML)
set_target_properties(Sandbox-GUI PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Sandbox-GUI")

MESSAGE(STATUS "------------------------------")
