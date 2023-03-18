robocin_cpp_proto_library(
        NAME detection_proto_lib
        PROTOS geometry.proto raw.proto raw_wrapper.proto
        COMPILE_OPTIONS PRIVATE -w
)