########################################################################################################################

# add_subdirectory(client_cpp)
add_subdirectory(common)
# add_subdirectory(referee_cpp)
# add_subdirectory(vision_cpp)

########################################################################################################################

set(protobuf_subdir robocin/pb)

function(get_protobufs prefix_path result)
    file(GLOB_RECURSE files "${prefix_path}/*.proto")
    set(${result} ${files} PARENT_SCOPE)
endfunction()

get_protobufs("${protobuf_subdir}/utility" utility_pbs)
get_protobufs("${protobuf_subdir}/ssl" ssl_pbs)

robocin_cpp_proto_library(
        NAME utility_proto_lib
        PROTOS ${utility_pbs}
)

robocin_cpp_proto_library(
        NAME ssl_proto_lib
        PROTOS ${ssl_pbs}
)

########################################################################################################################
