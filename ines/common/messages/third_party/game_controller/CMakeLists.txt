robocin_cpp_proto_library(
        NAME game_controller_proto_lib
        PROTOS common.proto event.proto geometry.proto referee.proto tracked.proto tracked_wrapper.proto
        COMPILE_OPTIONS PRIVATE -w
)