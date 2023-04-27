robocin_cpp_proto_library(
  NAME common_proto_lib
  PROTOS common.proto
)

robocin_cpp_proto_library(
  NAME vision_proto_lib
  PROTOS vision.proto
)

robocin_cpp_proto_library(
  NAME referee_proto_lib
  PROTOS referee.proto
)

robocin_cpp_executable(
    NAME vision
    SRCS vision_main.cpp
    DEPS zmq_publish_subscribe
         vision_proto_lib
         common_proto_lib
         absl::time
         absl::random_random
         absl::strings
         utility
)

robocin_cpp_executable(
    NAME client
    SRCS client_main.cpp
    DEPS zmq_publish_subscribe
         vision_proto_lib
         common_proto_lib
         absl::time
         absl::synchronization
         absl::random_random
         absl::strings
         utility
)
