# Copyright 2018 gRPC authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# cmake build file for C++ route_guide example.
# Assumes protobuf and gRPC have been installed using cmake.
# See cmake_externalproject/CMakeLists.txt for all-in-one cmake build
# that automatically builds all the dependencies before building route_guide.

find_package(Threads REQUIRED)

option(USE_SYSTEM_GRPC "Use system installed gRPC" OFF)
set(gRPC_FETCH_CONTENT_VERSION "v1.45.0" CACHE STRING "If not using system gRPC, it will be build using fetch content with the specified version")
if(USE_SYSTEM_GRPC)
  # Find system-installed gRPC
  find_package(gRPC CONFIG REQUIRED)
  message(STATUS "Using system grpc")
else()
	# Build gRPC using FetchContent
	CPMAddPackage(
        NAME grpc
        GITHUB_REPOSITORY grpc/grpc
		GIT_TAG        ${gRPC_FETCH_CONTENT_VERSION}
		OPTIONS
			"BUILD_TESTING OFF"
			"gRPC_BUILD_TESTS OFF"
	)
	#set(ABSL_PROPAGATE_CXX_STD ON)
	#set(BUILD_TESTING OFF)
	#set(gRPC_BUILD_TESTS OFF)
	#set(gRPC_BUILD_GRPC_NODE_PLUGIN OFF)
	#set(gRPC_BUILD_GRPC_RUBY_PLUGIN OFF)
	#set(gRPC_BUILD_GRPC_PYTHON_PLUGIN OFF)
	#set(gRPC_BUILD_GRPC_PHP_PLUGIN OFF)
	#set(protobuf_MSVC_STATIC_RUNTIME ON)
	#set(Protobuf_USE_STATIC_LIBS ON)
	#set(BUILD_SHARED_LIBS OFF)
	#set(gRPC_BUILD_GRPC_CSHARP_PLUGIN OFF)
	#set(gRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN OFF)
	#set(gRPC_BUILD_CSHARP_EXT OFF)
	#set(OPENSSL_NO_ASM ON)

	# Since FetchContent uses add_subdirectory under the hood, we can use
	# the grpc targets directly from this build.
	set(_PROTOBUF_LIBPROTOBUF libprotobuf)
	set(_REFLECTION grpc++_reflection)
	set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
	set(_GRPC_GRPCPP grpc++)
	if(CMAKE_CROSSCOMPILING)
		find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
	else()
		set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)
	endif()
endif()

function(generate_proto_target _TARGET)
  if(NOT ARGN)
    message(SEND_ERROR "Error: generate_proto() called without any proto files")
    return()
  endif()

  foreach(_proto ${ARGN})
	get_filename_component(hw_proto ${_proto} ABSOLUTE)
	get_filename_component(hw_proto_name "${_proto}" NAME_WE)
	get_filename_component(hw_proto_path "${hw_proto}" PATH)

	# Generated sources
	set(_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/proto_gens")
	set(hw_proto_srcs "${_GEN_DIR}/${_TARGET}/${hw_proto_name}.pb.cc")
	set(hw_proto_hdrs "${_GEN_DIR}/${_TARGET}/${hw_proto_name}.pb.h")
	set(hw_grpc_srcs "${_GEN_DIR}/${_TARGET}/${hw_proto_name}.grpc.pb.cc")
	set(hw_grpc_hdrs "${_GEN_DIR}/${_TARGET}/${hw_proto_name}.grpc.pb.h")

	LIST(APPEND PROTO_HDRS ${hw_proto_srcs} ${hw_grpc_srcs})
    LIST(APPEND PROTO_SRCS ${hw_proto_hdrs} ${hw_proto_hdrs})

	add_custom_command(
		  OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
		  COMMAND ${_PROTOBUF_PROTOC}
		  ARGS --grpc_out "${_GEN_DIR}/${_TARGET}"
			--cpp_out "${_GEN_DIR}/${_TARGET}"
			-I "${hw_proto_path}"
			--plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
			"${hw_proto}"
		  DEPENDS "${hw_proto}")
	endforeach()

	add_library(${_TARGET}
		${PROTO_HDRS}
		${PROTO_SRCS}
	)
	target_include_directories(${_TARGET}
		INTERFACE
		${_GEN_DIR}
	)
	target_link_libraries(${_TARGET}
		${_REFLECTION}
		${_GRPC_GRPCPP}
		${_PROTOBUF_LIBPROTOBUF}
	)
endfunction()
