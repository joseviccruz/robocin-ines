import argparse
import datetime as dt
from typing import Union, Optional

from common.multicast_udp_socket import MulticastUdpSocket
from common.zmq_publish_subscribe import ZmqPublisher
from robocin.pb.ssl.third_party.detection.raw_wrapper_pb2 import SSL_WrapperPacket
from robocin.pb.ssl.third_party.game_controller.tracked_wrapper_pb2 import TrackerWrapperPacket
from vision_py.mappers import field_mapper, frame_mapper

if __name__ == '__main__':
    # Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('--raw_ip_address', default='224.5.23.2')
    parser.add_argument('--raw_port', default=10006, type=int)
    parser.add_argument('--tracked_ip_address', default='224.5.23.2')
    parser.add_argument('--tracked_port', default=10010, type=int)
    parser.add_argument('--network_interface', required=True)
    parser.add_argument('--type', default='raw', choices=['raw', 'tracked'])

    args = parser.parse_args()

    # Create UDP socket to receive raw vision packets
    raw_udp_socket = MulticastUdpSocket()
    raw_udp_socket.bind(args.raw_ip_address, args.raw_port, args.network_interface)

    # Create UDP socket to receive tracked vision packets
    tracked_udp_socket = None
    if args.type != 'raw':
        tracked_udp_socket = MulticastUdpSocket()
        tracked_udp_socket.bind(args.tracked_ip_address, args.tracked_port, args.network_interface)

    # Create ZMQ publisher to publish vision packets
    publisher = ZmqPublisher()
    publisher.bind('ipc:///tmp/vision.ipc')

    # Create Frame and Field counters
    frame_count = 0
    field_count = 0

    while True:
        # Receive vision packet
        data = raw_udp_socket.receive()
        raw_packet = SSL_WrapperPacket()

        frame_packet: Optional[Union[SSL_WrapperPacket, TrackerWrapperPacket]] = None

        try:
            raw_packet.ParseFromString(data)
        except TypeError:
            print(f'{dt.datetime.now()}: the received raw packet was not parsed.')
            continue

        if tracked_udp_socket:
            tracked_data = tracked_udp_socket.receive()
            tracked_packet = TrackerWrapperPacket()

            try:
                tracked_packet.ParseFromString(tracked_data)
            except TypeError:
                print(f'{dt.datetime.now()}: the received tracked packet was not parsed.')
                continue

            frame_packet = tracked_packet

        else:
            frame_packet = raw_packet

        # Create Frame
        frame = frame_mapper(frame_packet, frame_count, args.type)
        if frame:
            frame_count += 1
        else:
            print(f'{dt.datetime.now()}: the frame was not created given the received message.')
            continue

        # Create Field
        field = field_mapper(raw_packet, field_count)
        if field:
            frame.field.CopyFrom(field)
            field_count += 1

        print(f'{dt.datetime.now()}: frame was received.')
        publisher.send('frame', frame)

        # TODO: Sleep to reduce CPU usage.
