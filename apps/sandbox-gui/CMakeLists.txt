MESSAGE(STATUS "Building sandbox GUI")

include(${Dynamo_SOURCE_DIR}/cmake/CPM.cmake)

add_executable(Sandbox-GUI main.cpp)
target_link_libraries(Sandbox-GUI PRIVATE app)
set_target_properties(Sandbox-GUI PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${Dynamo_SOURCE_DIR}/bin/Sandbox-GUI")
add_fonts_to_output_dir(Sandbox-GUI)

