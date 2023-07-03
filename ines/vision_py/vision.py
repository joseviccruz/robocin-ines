import argparse
import time
import datetime as dt
from typing import Optional, List

from google.protobuf.timestamp_pb2 import Timestamp

from common.multicast_udp_socket import MulticastUdpSocket
from common.zmq_publish_subscribe import ZmqPublisher
from robocin.pb.ssl.third_party.detection.raw_pb2 import SSL_DetectionBall, SSL_DetectionRobot
from robocin.pb.ssl.third_party.detection.raw_wrapper_pb2 import SSL_WrapperPacket
from robocin.pb.ssl.vision.ball_pb2 import Ball
from robocin.pb.ssl.vision.field_pb2 import Field
from robocin.pb.ssl.vision.frame_pb2 import Frame
from robocin.pb.ssl.vision.robot_pb2 import Robot

SECONDS_TO_NANOSECONDS = 1_000_000_000


def now() -> Timestamp:
    floating_point_unix_nanos = time.time_ns()

    seconds = int(floating_point_unix_nanos / SECONDS_TO_NANOSECONDS)
    nanos = int(floating_point_unix_nanos % SECONDS_TO_NANOSECONDS)

    return Timestamp(seconds=seconds, nanos=nanos)


def time_from_timestamp(timestamp: Timestamp) -> float:
    return timestamp.seconds + timestamp.nanos / SECONDS_TO_NANOSECONDS


def make_ball(raw_balls: List[SSL_DetectionBall]) -> Ball:
    ball = Ball()

    if not raw_balls:
        return ball

    raw_ball = raw_balls[0]
    ball.position.x = raw_ball.x
    ball.position.y = raw_ball.y
    ball.position.z = raw_ball.z

    return ball


def make_robot(raw_robot: SSL_DetectionRobot, color: Robot.Color) -> Robot:
    robot = Robot()

    robot.color = color
    robot.id = raw_robot.robot_id

    robot.position.x = raw_robot.x
    robot.position.y = raw_robot.y

    robot.angle = raw_robot.orientation

    return robot


def make_robots(raw_robots: List[SSL_DetectionRobot], color: Robot.Color) -> List[Robot]:
    return [make_robot(raw_robot, color) for raw_robot in raw_robots]


def make_frame(packet: SSL_WrapperPacket, serial_id: int) -> Optional[Frame]:
    if not packet.HasField('detection'):
        return None

    frame = Frame()

    detection_frame = packet.detection

    frame.serial_id = serial_id
    frame.timestamp.CopyFrom(now())
    frame.ball.CopyFrom(make_ball(detection_frame.balls))
    frame.robots.extend(make_robots(detection_frame.robots_yellow, Robot.Color.COLOR_YELLOW) + \
                        make_robots(detection_frame.robots_blue, Robot.Color.COLOR_BLUE))

    return frame


def make_field(packet: SSL_WrapperPacket, serial_id: int) -> Optional[Field]:
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


if __name__ == '__main__':
    # Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('--ip_address', default='224.5.23.2')
    parser.add_argument('--port', default=10006, type=int)
    parser.add_argument('--network_interface', required=True)

    args = parser.parse_args()

    # Create UDP socket to receive vision packets
    udp_socket = MulticastUdpSocket()
    udp_socket.bind(args.ip_address, args.port, args.network_interface)

    # Create ZMQ publisher to publish vision packets
    publisher = ZmqPublisher()
    publisher.bind('ipc:///tmp/vision.ipc')

    # Create Frame and Field counters
    frame_count = 0
    field_count = 0

    while True:
        # Receive vision packet
        data = udp_socket.receive()
        packet = SSL_WrapperPacket()

        try:
            packet.ParseFromString(data)
        except TypeError:
            continue

        # Create Frame
        frame = make_frame(packet, frame_count)
        if frame:
            frame_count += 1
        else:
            continue

        # Create Field
        field = make_field(packet, field_count)
        if field:
            frame.field.CopyFrom(field)
            field_count += 1

        print(f'frame was received: {dt.datetime.now()}.')
        publisher.send('frame', frame)

        # TODO: Sleep to reduce CPU usage.
