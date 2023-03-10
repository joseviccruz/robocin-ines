########################################################################################################################

include(deps)

########################################################################################################################

# Define the project name to be used in the code
set(ROBOCIN_PROJECT_NAME ${CMAKE_PROJECT_NAME})

########################################################################################################################

# Define the project path to be used in the code
get_filename_component(ROBOCIN_PROJECT_PATH "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

########################################################################################################################

# Define the global configuration path to be used in the code
get_filename_component(ROBOCIN_CONFIG_PATH "~/.config/${ROBOCIN_PROJECT_NAME}" ABSOLUTE)

########################################################################################################################

# Add cpp library
# Named parameters:
#  NAME: name of the library
#  HDRS: header files
#  SRCS: source files
#  RSRCS: resource files
#  DEPS: dependencies
function(robocin_cpp_library)
  cmake_parse_arguments(
          ARG                     # prefix of output variables
          ""                      # list of names of the boolean arguments
          "NAME"                  # list of names of mono-valued arguments
          "HDRS;SRCS;RSRCS;DEPS"  # list of names of multi-valued arguments
          ${ARGN}                 # arguments of the function to parse (ARGN contains all the arguments after the function name)
  )

  # if there isn't at least one header file, then the library is not created
  if(NOT ARG_HDRS)
    message(FATAL_ERROR "robocin_cpp_library: no header files given")
  endif()

  # if there isn't at least one source file, then the library is not created
  if(NOT ARG_SRCS)
    message(FATAL_ERROR "robocin_cpp_library: no source files given")
  endif()

  add_library(${ARG_NAME} ${ARG_HDRS} ${ARG_SRCS} ${ARG_RSRCS}) # add library with given name, headers, sources and resources
  target_link_libraries(${ARG_NAME} PUBLIC ${ARG_DEPS}) # link library with given dependencies

  target_include_directories(${ARG_NAME} PRIVATE ${ROBOCIN_PROJECT_PATH})
  target_include_directories(${ARG_NAME} PRIVATE ${CMAKE_BINARY_DIR})

  target_compile_definitions(${ARG_NAME} PRIVATE ROBOCIN_PROJECT_NAME="${ROBOCIN_PROJECT_NAME}")
  target_compile_definitions(${ARG_NAME} PRIVATE ROBOCIN_PROJECT_PATH="${ROBOCIN_PROJECT_PATH}")
  target_compile_definitions(${ARG_NAME} PRIVATE ROBOCIN_CONFIG_PATH="${ROBOCIN_CONFIG_PATH}")

endfunction(robocin_cpp_library)

########################################################################################################################

# Add cpp proto library
# Named parameters:
#  NAME: name of the library
#  PROTOS: proto files
function(robocin_cpp_proto_library)
  cmake_parse_arguments(
          ARG               # prefix of output variables
          ""                # list of names of the boolean arguments
          "NAME"            # list of names of mono-valued arguments
          "PROTOS"          # list of names of multi-valued arguments
          ${ARGN}           # arguments of the function to parse
  )

  # if there isn't at least one proto file, then the library is not created
  if(NOT ARG_PROTOS)
    message(FATAL_ERROR "robocin_cpp_proto_library: no proto files given")
  endif()

  set(proto_hdrs)
  set(proto_srcs)

  foreach(PROTO ${ARG_PROTOS})
    get_filename_component(proto_name ${PROTO} NAME_WE)
    get_filename_component(proto_absolute_file ${PROTO} ABSOLUTE)
    get_filename_component(proto_absolute_path ${proto_absolute_file} DIRECTORY)
    file(RELATIVE_PATH proto_relative_file ${ROBOCIN_PROJECT_PATH} ${proto_absolute_file})
    file(RELATIVE_PATH proto_relative_path ${ROBOCIN_PROJECT_PATH} ${proto_absolute_path})

    set(proto_hdr_file "${CMAKE_BINARY_DIR}/${proto_relative_path}/${proto_name}.pb.h")
    set(proto_src_file "${CMAKE_BINARY_DIR}/${proto_relative_path}/${proto_name}.pb.cc")

    add_custom_command(
            OUTPUT "${proto_hdr_file}" "${proto_src_file}"
            COMMAND $<TARGET_FILE:protobuf::protoc>
            ARGS --proto_path ${ROBOCIN_PROJECT_PATH}
                 --cpp_out "${CMAKE_BINARY_DIR}"
                 "${proto_relative_file}"
            DEPENDS "${proto_absolute_file}"
            WORKING_DIRECTORY ${ROBOCIN_PROJECT_PATH}
    )

    list(APPEND proto_hdrs ${proto_hdr_file})
    list(APPEND proto_srcs ${proto_src_file})

  endforeach(PROTO)

  add_library(${ARG_NAME} ${proto_hdrs} ${proto_srcs})
  target_link_libraries(${ARG_NAME} PUBLIC protobuf::libprotobuf) # link library with given dependencies

  target_include_directories(${ARG_NAME} PRIVATE ${ROBOCIN_PROJECT_PATH})
  target_include_directories(${ARG_NAME} PRIVATE ${CMAKE_BINARY_DIR})

  target_compile_definitions(${ARG_NAME} PRIVATE ROBOCIN_PROJECT_NAME="${ROBOCIN_PROJECT_NAME}")
  target_compile_definitions(${ARG_NAME} PRIVATE ROBOCIN_PROJECT_PATH="${ROBOCIN_PROJECT_PATH}")
  target_compile_definitions(${ARG_NAME} PRIVATE ROBOCIN_CONFIG_PATH="${ROBOCIN_CONFIG_PATH}")

endfunction(robocin_cpp_proto_library)

########################################################################################################################

# Add cpp grpc library
# Named parameters:
#  NAME: name of the library
#  PROTOS: (grpc) proto files
function(robocin_cpp_grpc_library)
  cmake_parse_arguments(
          ARG               # prefix of output variables
          ""                # list of names of the boolean arguments
          "NAME"            # list of names of mono-valued arguments
          "PROTOS"          # list of names of multi-valued arguments
          ${ARGN}           # arguments of the function to parse
  )

  # if there isn't at least one proto file, then the library is not created
  if(NOT ARG_PROTOS)
    message(FATAL_ERROR "robocin_cpp_grpc_library: no proto files given")
  endif()

  set(proto_hdrs)
  set(proto_srcs)
  set(grpc_hdrs)
  set(grpc_srcs)

  foreach(PROTO ${ARG_PROTOS})
    get_filename_component(proto_name ${PROTO} NAME_WE)
    get_filename_component(proto_absolute_file ${PROTO} ABSOLUTE)
    get_filename_component(proto_absolute_path ${proto_absolute_file} DIRECTORY)
    file(RELATIVE_PATH proto_relative_file ${ROBOCIN_PROJECT_PATH} ${proto_absolute_file})
    file(RELATIVE_PATH proto_relative_path ${ROBOCIN_PROJECT_PATH} ${proto_absolute_path})

    set(proto_hdr_file "${CMAKE_BINARY_DIR}/${proto_relative_path}/${proto_name}.pb.h")
    set(proto_src_file "${CMAKE_BINARY_DIR}/${proto_relative_path}/${proto_name}.pb.cc")
    set(grpc_hdr_file "${CMAKE_BINARY_DIR}/${proto_relative_path}/${proto_name}.grpc.pb.h")
    set(grpc_src_file "${CMAKE_BINARY_DIR}/${proto_relative_path}/${proto_name}.grpc.pb.cc")

    add_custom_command(
            OUTPUT "${proto_hdr_file}" "${proto_src_file}" "${grpc_hdr_file}" "${grpc_src_file}"
            COMMAND $<TARGET_FILE:protobuf::protoc>
            ARGS --proto_path ${ROBOCIN_PROJECT_PATH}
                 --cpp_out "${CMAKE_BINARY_DIR}"
                 --grpc_out "${CMAKE_BINARY_DIR}"
                 --plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
                 "${proto_relative_file}"
            DEPENDS "${proto_absolute_file}"
            WORKING_DIRECTORY ${ROBOCIN_PROJECT_PATH}
    )

    list(APPEND proto_hdrs ${proto_hdr_file})
    list(APPEND proto_srcs ${proto_src_file})
    list(APPEND grpc_hdrs ${grpc_hdr_file})
    list(APPEND grpc_srcs ${grpc_src_file})

  endforeach(PROTO)

  add_library(${ARG_NAME} ${proto_hdrs} ${proto_srcs} ${grpc_hdrs} ${grpc_srcs})
  target_link_libraries(${ARG_NAME} PUBLIC protobuf::libprotobuf gRPC::grpc++ gRPC::grpc++_reflection) # link library with given dependencies

  target_include_directories(${ARG_NAME} PRIVATE ${ROBOCIN_PROJECT_PATH})
  target_include_directories(${ARG_NAME} PRIVATE ${CMAKE_BINARY_DIR})

  target_compile_definitions(${ARG_NAME} PRIVATE ROBOCIN_PROJECT_NAME="${ROBOCIN_PROJECT_NAME}")
  target_compile_definitions(${ARG_NAME} PRIVATE ROBOCIN_PROJECT_PATH="${ROBOCIN_PROJECT_PATH}")
  target_compile_definitions(${ARG_NAME} PRIVATE ROBOCIN_CONFIG_PATH="${ROBOCIN_CONFIG_PATH}")

endfunction(robocin_cpp_grpc_library)

########################################################################################################################

# Add cpp unit test
# Named parameters:
#  NAME: name of the test
#  HDRS: header files
#  SRCS: source files
#  RSRCS: resource files
#  DEPS: dependencies
function(robocin_cpp_test)
  cmake_parse_arguments(
          ARG                     # prefix of output variables
          ""                      # list of names of the boolean arguments
          "NAME"                  # list of names of mono-valued arguments
          "HDRS;SRCS;RSRCS;DEPS"  # list of names of multi-valued arguments
          ${ARGN}                 # arguments of the function to parse (ARGN contains all the arguments after the function name)
  )

  # check if at least one source file is given with suffix '_test.cpp'
  if(NOT ARG_SRCS)
    message(FATAL_ERROR "robocin_cpp_test: no source files given")
  else()
    set(FILTERED_SRCS ${ARG_SRCS})
    list(FILTER FILTERED_SRCS INCLUDE REGEX "_test\.cpp$")

    if(NOT FILTERED_SRCS)
      message(FATAL_ERROR "robocin_cpp_test: no source files given with suffix '_test.cpp'")
    endif()
  endif()

  add_executable(${ARG_NAME} ${ARG_HDRS} ${ARG_SRCS} ${ARG_RSRCS}) # add test with given name, headers, sources and resources
  target_link_libraries(${ARG_NAME} PRIVATE GTest::gtest GTest::gtest_main ${ARG_DEPS}) # link library with given dependencies

  target_include_directories(${ARG_NAME} PRIVATE ${ROBOCIN_PROJECT_PATH})
  target_include_directories(${ARG_NAME} PRIVATE ${CMAKE_BINARY_DIR})

  target_compile_definitions(${ARG_NAME} PRIVATE ROBOCIN_PROJECT_NAME="${ROBOCIN_PROJECT_NAME}")
  target_compile_definitions(${ARG_NAME} PRIVATE ROBOCIN_PROJECT_PATH="${ROBOCIN_PROJECT_PATH}")
  target_compile_definitions(${ARG_NAME} PRIVATE ROBOCIN_CONFIG_PATH="${ROBOCIN_CONFIG_PATH}")

  gtest_discover_tests(${ARG_NAME})

endfunction(robocin_cpp_test)

########################################################################################################################

# Add cpp benchmark test
# Named parameters:
#  NAME: name of the test
#  HDRS: header files
#  SRCS: source files
#  RSRCS: resource files
#  DEPS: dependencies
function(robocin_cpp_benchmark_test)
  cmake_parse_arguments(
          ARG                     # prefix of output variables
          ""                      # list of names of the boolean arguments
          "NAME"                  # list of names of mono-valued arguments
          "HDRS;SRCS;RSRCS;DEPS"  # list of names of multi-valued arguments
          ${ARGN}                 # arguments of the function to parse (ARGN contains all the arguments after the function name)
  )

  # check if at least one source file is given with suffix '_benchmark.cpp'
  if(NOT ARG_SRCS)
    message(FATAL_ERROR "robocin_cpp_benchmark_test: no source files given")
  else()
    set(FILTERED_SRCS ${ARG_SRCS})
    list(FILTER FILTERED_SRCS INCLUDE REGEX "_benchmark\.cpp$")

    if(NOT FILTERED_SRCS)
      message(FATAL_ERROR "robocin_cpp_benchmark_test: no source files given with suffix '_benchmark.cpp'")
    endif()
  endif()

  add_executable(${ARG_NAME} ${ARG_HDRS} ${ARG_SRCS} ${ARG_RSRCS}) # add test with given name, headers, sources and resources
  target_link_libraries(${ARG_NAME} PRIVATE benchmark::benchmark benchmark::benchmark_main ${ARG_DEPS}) # link library with given dependencies

  target_include_directories(${ARG_NAME} PRIVATE ${ROBOCIN_PROJECT_PATH})
  target_include_directories(${ARG_NAME} PRIVATE ${CMAKE_BINARY_DIR})

  target_compile_definitions(${ARG_NAME} PRIVATE ROBOCIN_PROJECT_NAME="${ROBOCIN_PROJECT_NAME}")
  target_compile_definitions(${ARG_NAME} PRIVATE ROBOCIN_PROJECT_PATH="${ROBOCIN_PROJECT_PATH}")
  target_compile_definitions(${ARG_NAME} PRIVATE ROBOCIN_CONFIG_PATH="${ROBOCIN_CONFIG_PATH}")

endfunction(robocin_cpp_benchmark_test)

########################################################################################################################

# Add cpp executable
# Named parameters:
#  NAME: name of the executable
#  HDRS: header files
#  SRCS: source files
#  RSRCS: resource files
#  DEPS: dependencies
function(robocin_cpp_executable)
  cmake_parse_arguments(
          ARG                     # prefix of output variables
          ""                      # list of names of the boolean arguments
          "NAME"                  # list of names of mono-valued arguments
          "HDRS;SRCS;RSRCS;DEPS"  # list of names of multi-valued arguments
          ${ARGN}                 # arguments of the function to parse (ARGN contains all the arguments after the function name)
  )

  # check if at least one source file is given with suffix '_main.cpp'
  if(NOT ARG_SRCS)
    message(FATAL_ERROR "robocin_cpp_executable: no source files given")
  else()
    set(FILTERED_SRCS ${ARG_SRCS})
    list(FILTER FILTERED_SRCS INCLUDE REGEX "_main\.cpp$")

    if(NOT FILTERED_SRCS)
      message(FATAL_ERROR "robocin_cpp_executable: no source files given with suffix '_main.cpp'")
    endif()
  endif()

  add_executable(${ARG_NAME} ${ARG_HDRS} ${ARG_SRCS} ${ARG_RSRCS}) # add executable with given name, headers, sources and resources
  target_link_libraries(${ARG_NAME} PRIVATE ${ARG_DEPS}) # link library with given dependencies

  target_include_directories(${ARG_NAME} PRIVATE ${ROBOCIN_PROJECT_PATH})
  target_include_directories(${ARG_NAME} PRIVATE ${CMAKE_BINARY_DIR})

  target_compile_definitions(${ARG_NAME} PRIVATE ROBOCIN_PROJECT_NAME="${ROBOCIN_PROJECT_NAME}")
  target_compile_definitions(${ARG_NAME} PRIVATE ROBOCIN_PROJECT_PATH="${ROBOCIN_PROJECT_PATH}")
  target_compile_definitions(${ARG_NAME} PRIVATE ROBOCIN_CONFIG_PATH="${ROBOCIN_CONFIG_PATH}")

endfunction(robocin_cpp_executable)

########################################################################################################################
