MESSAGE(STATUS "Building Sandbox console")
add_executable(Sandbox-console main.cpp)
target_compile_features(Sandbox-console PRIVATE cxx_std_20)
target_link_libraries(Sandbox-console PRIVATE dynamo)
set_target_properties(Sandbox-console PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${Dynamo_SOURCE_DIR}/bin/Sandbox-console")

