import datetime as dt
from time import sleep

from google.protobuf.any_pb2 import Any

from common.recordio import RecordReader
from common.zmq_publish_subscribe import ZmqPublisher
from robocin.pb.ssl.third_party.game_controller.referee_pb2 import Referee
from robocin.pb.ssl.vision.frame_pb2 import Frame

if __name__ == '__main__':
    reader = RecordReader('log.recordio')

    vision_pub = ZmqPublisher()
    vision_pub.bind('ipc:///tmp/vision.ipc')

    referee_pub = ZmqPublisher()
    referee_pub.bind('ipc:///tmp/referee.ipc')

    index = 0

    while True:
        for data in reader:
            message = Any()
            message.ParseFromString(data)

            frame = Frame()
            referee = Referee()

            if message.Unpack(frame):
                print(frame)
                vision_pub.send(frame, 'frame')
                sleep(0.016)

            elif message.Unpack(referee):
                print(referee)
                referee_pub.send(frame, 'referee')
                sleep(0.016)

            else:
                print(f'{dt.datetime.now()}: the message {index} was not emitted.')

            index += 1
