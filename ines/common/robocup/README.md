# Robocup
This directory contains the libraries needed for a RoboCup Small Size League game.

## Protobufs

The `.proto` files in this directory are named differently from the original files. The mapping between the files described here and the original files are shown below:

### detection

| File Name             | Corresponding SSL Vision Proto        |
| ----------------------| --------------------------------------|
| geometry.proto        | ssl_vision_geometry.proto             |
| raw.proto             | ssl_vision_detection.proto            |
| raw_wrapper.proto     | ssl_vision_wrapper.proto              |
| tracked.proto         | ssl_vision_tracked.proto              |
| tracked_wrapper.proto | ssl_vision_detection_tracked.proto    |

### game_controller

| File Name       | Corresponding SSL Game Controller Proto          |
| ----------------| -------------------------------------------------|
| common.proto    | ssl_gc_common.proto                              |
| event.proto     | ssl_gc_game_event.proto                          |
| geometry.proto  | ssl_gc_geometry.proto                            |
| referee.proto   | ssl_gc_referee_message.proto                     |

### game_controller

| File Name                        | Corresponding SSL Simulation Proto               |
| ---------------------------------| -------------------------------------------------|
| config.proto                     | ssl_simulation_config.proto                      |
| control.proto                    | ssl_simulation_control.proto                     |
| error.proto                      | ssl_simulation_error.proto                       |
| robot_control.proto              | ssl_simulation_robot_control.proto               |
| robot_feedback.proto             | ssl_simulation_robot_feedback.proto              |
| custom_robot_specs_erforce.proto | ssl_simulation_custom_erforce_robot_spec.proto   |
| synchronous.proto                | ssl_simulation_synchronous.proto                 |




