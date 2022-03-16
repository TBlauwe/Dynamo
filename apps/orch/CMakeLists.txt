MESSAGE(STATUS "Building ORCH")

MESSAGE(STATUS "Fetching gRPC ...")
#include(cmake/gRPC.cmake)

## Include generated *.pb.h files
#generate_proto_target(
#	myproto										#target
#	"${CMAKE_CURRENT_BINARY_DIR}/proto_gens"	#output directory
#	#proto files
#	"proto/address.proto"						
#	"proto/addressbook.proto"						
#	"proto/orch.proto"						
#)
#
## SERVER
#add_executable(SERVER server.cpp)
#target_link_libraries(SERVER 
#    myproto
#)
#
## CLIENT
#add_executable(CLIENT client.cpp)
#target_link_libraries(CLIENT
#	myproto
#)

# ORCH CLIENT
add_executable(ORCH orch.cpp)
target_compile_features(ORCH PRIVATE cxx_std_20) 
target_link_libraries(ORCH PRIVATE dynamo )
#myproto
#  ${_REFLECTION}
#  ${_GRPC_GRPCPP}
#  ${_PROTOBUF_LIBPROTOBUF}
#)
set_target_properties(ORCH PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${Dynamo_SOURCE_DIR}/bin/orch")
