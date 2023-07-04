from typing import List, Tuple, Optional, Union

from common.timestamp_utility import now, timestamp_from_unix_seconds
from robocin.pb.ssl.third_party.detection.raw_pb2 import SSL_DetectionRobot, SSL_DetectionBall
from robocin.pb.ssl.third_party.detection.raw_wrapper_pb2 import SSL_WrapperPacket
from robocin.pb.ssl.third_party.game_controller.common_pb2 import RobotId
from robocin.pb.ssl.third_party.game_controller.tracked_pb2 import TrackedBall, KickedBall, TrackedRobot
from robocin.pb.ssl.third_party.game_controller.tracked_wrapper_pb2 import TrackerWrapperPacket
from robocin.pb.ssl.vision.ball_pb2 import Ball
from robocin.pb.ssl.vision.field_pb2 import Field
from robocin.pb.ssl.vision.frame_pb2 import Frame
from robocin.pb.ssl.vision.robot_pb2 import Robot

METERS_TO_MILLIMETERS = 1_000


def ball_mapper_from_raw_ball(raw_balls: List[SSL_DetectionBall]) -> Ball:
    ball = Ball()

    if not raw_balls:
        return ball

    raw_ball = raw_balls[0]
    ball.position.x = raw_ball.x
    ball.position.y = raw_ball.y
    ball.position.z = raw_ball.z

    return ball


def robot_mapper_from_raw_robot(raw_robot: SSL_DetectionRobot, color: int) -> Robot:
    robot = Robot()

    robot.color = color
    robot.id = raw_robot.robot_id

    robot.position.x = raw_robot.x
    robot.position.y = raw_robot.y

    robot.angle = raw_robot.orientation

    return robot


def robots_mapper_from_raw_robots(raw_robots: List[SSL_DetectionRobot], color: int) -> List[Robot]:
    return [robot_mapper_from_raw_robot(raw_robot, color) for raw_robot in raw_robots]


def frame_mapper_from_raw_wrapper_packet(packet: SSL_WrapperPacket, serial_id: int):
    frame = Frame()

    if not packet.HasField('detection'):
        return frame

    raw_frame = packet.detection

    frame.serial_id = serial_id
    frame.timestamp.CopyFrom(now())
    frame.ball.CopyFrom(ball_mapper_from_raw_ball(raw_frame.balls))
    frame.robots.extend(robots_mapper_from_raw_robots(raw_frame.robots_yellow, Robot.Color.COLOR_YELLOW) +
                        robots_mapper_from_raw_robots(raw_frame.robots_blue, Robot.Color.COLOR_BLUE))

    return frame


def robot_id_and_color_mapper_from_robot_id(robot_id: RobotId) -> Tuple[int, int]:
    return robot_id.id, int(robot_id.team)


def ball_mapper_from_tracked_ball(tracked_balls: List[TrackedBall], kicked_ball: Optional[KickedBall]) -> Ball:
    ball = Ball()

    if not tracked_balls:
        return ball

    tracked_ball = tracked_balls[0]

    ball.position.x = tracked_ball.pos.x * METERS_TO_MILLIMETERS
    ball.position.y = tracked_ball.pos.y * METERS_TO_MILLIMETERS
    ball.position.z = tracked_ball.pos.z * METERS_TO_MILLIMETERS

    ball.velocity.x = tracked_ball.vel.x * METERS_TO_MILLIMETERS
    ball.velocity.y = tracked_ball.vel.y * METERS_TO_MILLIMETERS
    ball.velocity.z = tracked_ball.vel.z * METERS_TO_MILLIMETERS

    if kicked_ball:
        robot_id, robot_color = robot_id_and_color_mapper_from_robot_id(kicked_ball.robot_id)

        ball.kick_information.robot_color = robot_color
        ball.kick_information.robot_id = robot_id

        ball.kick_information.start_position.x = kicked_ball.pos.x * METERS_TO_MILLIMETERS
        ball.kick_information.start_position.y = kicked_ball.pos.y * METERS_TO_MILLIMETERS

        ball.kick_information.start_velocity.x = kicked_ball.vel.x * METERS_TO_MILLIMETERS
        ball.kick_information.start_velocity.y = kicked_ball.vel.y * METERS_TO_MILLIMETERS
        ball.kick_information.start_velocity.z = kicked_ball.vel.z * METERS_TO_MILLIMETERS

        ball.kick_information.start_timestamp.CopyFrom(timestamp_from_unix_seconds(kicked_ball.start_timestamp))

        ball.kick_information.stop_position.x = kicked_ball.stop_pos.x * METERS_TO_MILLIMETERS
        ball.kick_information.stop_position.y = kicked_ball.stop_pos.y * METERS_TO_MILLIMETERS

        ball.kick_information.stop_timestamp.CopyFrom(timestamp_from_unix_seconds(kicked_ball.stop_timestamp))

    return ball


def robot_mapper_from_tracked_robot(tracked_robot: TrackedRobot) -> Robot:
    robot = Robot()

    robot_id, robot_color = robot_id_and_color_mapper_from_robot_id(tracked_robot.robot_id)

    robot.color = robot_color
    robot.id = robot_id

    robot.position.x = tracked_robot.pos.x * METERS_TO_MILLIMETERS
    robot.position.y = tracked_robot.pos.y * METERS_TO_MILLIMETERS

    robot.angle = tracked_robot.orientation

    robot.velocity.x = tracked_robot.vel.x * METERS_TO_MILLIMETERS
    robot.velocity.y = tracked_robot.vel.y * METERS_TO_MILLIMETERS

    robot.angular_velocity = tracked_robot.vel_angular

    return robot


def robots_mapper_from_tracked_robots(tracked_robots: List[TrackedRobot]) -> List[Robot]:
    return [robot_mapper_from_tracked_robot(tracked_robot) for tracked_robot in tracked_robots]


def frame_mapper_from_tracked_wrapper_packet(packet: TrackerWrapperPacket, serial_id: int) -> Optional[Frame]:
    frame = Frame()

    if not packet.HasField('tracked_frame'):
        return frame

    tracked_frame = packet.tracked_frame

    frame.serial_id = serial_id
    frame.timestamp.CopyFrom(now())
    frame.ball.CopyFrom(ball_mapper_from_tracked_ball(tracked_frame.balls, tracked_frame.kicked_ball))
    frame.robots.extend(robots_mapper_from_tracked_robots(tracked_frame.robots))

    return frame


def frame_mapper(packet: Union[SSL_WrapperPacket, TrackerWrapperPacket], serial_id: int, type: str) -> Optional[Frame]:
    if type == 'raw':
        return frame_mapper_from_raw_wrapper_packet(packet, serial_id)

    return frame_mapper_from_tracked_wrapper_packet(packet, serial_id)


def field_mapper(packet: SSL_WrapperPacket, serial_id: int) -> Optional[Field]:
    if not packet.HasField('geometry'):
        return None

    detection_field = packet.geometry.field

    field = Field()
    field.serial_id = serial_id

    FIELD_LENGTH = 9000.0
    FIELD_WIDTH = 6000.0
    GOAL_DEPTH = 180.0
    GOAL_WIDTH = 1000.0
    PENALTY_AREA_DEPTH = 1000.0
    PENALTY_AREA_WIDTH = 2000.0
    BOUNDARY_WIDTH = 300.0
    GOAL_CENTER_TO_PENALTY_MARK = 6000.0

    field.length = detection_field.field_length or FIELD_LENGTH
    field.width = detection_field.field_width or FIELD_WIDTH
    field.goal_depth = detection_field.goal_depth or GOAL_DEPTH
    field.goal_width = detection_field.goal_width or GOAL_WIDTH
    field.penalty_area_depth = detection_field.penalty_area_depth or PENALTY_AREA_DEPTH
    field.penalty_area_width = detection_field.penalty_area_width or PENALTY_AREA_WIDTH
    field.boundary_width = detection_field.boundary_width or BOUNDARY_WIDTH
    field.goal_center_to_penalty_mark = detection_field.goal_center_to_penalty_mark or GOAL_CENTER_TO_PENALTY_MARK

    return field
