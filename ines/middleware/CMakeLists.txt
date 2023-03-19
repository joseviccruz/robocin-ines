robocin_cpp_grpc_library(
        NAME middleware_grpc_lib
        PROTOS middleware.proto
)

robocin_cpp_library(
        NAME middleware_lib
        HDRS middleware.h
        SRCS middleware.cpp
        DEPS middleware_grpc_lib vision_proto_lib referee_proto_lib
)

robocin_cpp_executable(
        NAME middleware
        SRCS middleware_main.cpp
        DEPS middleware_lib
)