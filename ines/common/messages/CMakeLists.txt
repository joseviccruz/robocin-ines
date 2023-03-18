add_subdirectory(third_party)

robocin_cpp_proto_library(
        NAME vision_proto_lib
        PROTOS vision.proto
)

robocin_cpp_proto_library(
        NAME referee_proto_lib
        PROTOS referee.proto
)
