robocin_cpp_proto_library(
        NAME simulation_proto_lib
        PROTOS erforce/custom_realism.proto erforce/custom_robot_specs.proto config.proto control.proto error.proto robot_control.proto robot_feedback.proto synchronous.proto
        COMPILE_OPTIONS PRIVATE -w
)