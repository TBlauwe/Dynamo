MESSAGE(STATUS "Building Victeams")


#MESSAGE(STATUS "Fetching gRPC ...")
#CPMAddPackage(
#        NAME grpc 
#        GITHUB_REPOSITORY grpc/grpc
#        GIT_TAG v1.43.0
#        OPTIONS
#        "BUILD_TESTING OFF"
#        "gRPC_INSTALL ON"
#        "gRPC_BUILD_TESTS OFF"
#        "OPENSSL_NO_ASM ON"
#)
#if (grpc_ADDED)
#    MESSAGE(STATUS "Fetching grpc - done")
#    MESSAGE(STATUS "------------------------")
#else()
#    MESSAGE(FATAL_ERROR "Could not fetch grpc")
#endif ()

add_executable(Victeams main.cpp)
target_compile_features(Victeams PRIVATE cxx_std_20)
target_link_libraries(Victeams PRIVATE dynamo)
set_target_properties(Victeams PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${Dynamo_SOURCE_DIR}/bin/Sandbox-console")
