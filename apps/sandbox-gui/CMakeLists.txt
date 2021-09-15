MESSAGE(STATUS ">>>>> Building Sandbox GUI")

include(${Dynamo_SOURCE_DIR}/cmake/CPM.cmake)

add_executable(Sandbox-GUI main.cpp)
target_link_libraries(Sandbox-GUI PRIVATE app)
set_target_properties(Sandbox-GUI PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Sandbox-GUI")