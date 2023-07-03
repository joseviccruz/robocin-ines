from typing import Optional

from robocin.pb.ssl.vision.frame_pb2 import Frame
from robocin.pb.ssl.third_party.game_controller.referee_pb2 import Referee
from common.zmq_publish_subscribe import PubSubMode, ZmqSubscriber
from common.recordio import RecordWriter
from google.protobuf.any_pb2 import Any


def parse_frame(data: bytes) -> Optional[Frame]:
    if data is None:
        return data
    frame = Frame()
    frame.ParseFromString(data)
    return frame


def parse_referee(data: bytes) -> Optional[Referee]:
    if data is None:
        return data
    referee = Referee()
    referee.ParseFromString(data)
    return referee


if __name__ == '__main__':
    while True:
        vision_receiver = ZmqSubscriber('frame')
        vision_receiver.connect('ipc:///tmp/vision.ipc')

        referee_receiver = ZmqSubscriber('referee')
        referee_receiver.connect('ipc:///tmp/referee.ipc')

        record_writer = RecordWriter('log.recordio')

        while True:
            frame = parse_frame(vision_receiver.receive(PubSubMode.DontWait))
            referee = parse_referee(referee_receiver.receive(PubSubMode.DontWait))

            message = Any()

            if frame:
                print(frame)
                message.Pack(frame)

            if referee:
                print(referee)
                message.Pack(referee)

            record_writer.write_protocol_message(message)

            # TODO: Sleep to reduce CPU usage.
