set(detection_protos
        detection/geometry.proto
        detection/raw.proto
        detection/raw_wrapper.proto)

set(game_controller_protos
        game_controller/common.proto
        game_controller/event.proto
        game_controller/geometry.proto
        game_controller/referee.proto
        game_controller/tracked.proto
        game_controller/tracked_wrapper.proto)

set(simulation_protos
        simulation/erforce/custom_realism.proto
        simulation/erforce/custom_robot_specs.proto
        simulation/config.proto
        simulation/control.proto
        simulation/error.proto
        simulation/robot_control.proto
        simulation/robot_feedback.proto
        simulation/synchronous.proto)

robocin_cpp_grpc_library(
        NAME common_messages_third_party_proto_lib
        PROTOS ${detection_protos} ${game_controller_protos} ${simulation_protos}
)

target_compile_options(third_party_proto_lib PRIVATE -w)