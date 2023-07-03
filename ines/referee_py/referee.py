import argparse
import datetime as dt

from common.multicast_udp_socket import MulticastUdpSocket
from common.zmq_publish_subscribe import ZmqPublisher
from robocin.pb.ssl.third_party.game_controller.referee_pb2 import Referee

if __name__ == '__main__':
    # Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('--ip_address', default='224.5.23.1')
    parser.add_argument('--port', default=10003, type=int)
    parser.add_argument('--network_interface', required=True)

    args = parser.parse_args()

    # Create UDP socket to receive vision packets
    udp_socket = MulticastUdpSocket()
    udp_socket.bind(args.ip_address, args.port, args.network_interface)

    # Create ZMQ publisher to publish vision packets
    publisher = ZmqPublisher()
    publisher.bind('ipc:///tmp/referee.ipc')

    while True:
        # Receive referee packet
        data = udp_socket.receive()
        packet = Referee()

        try:
            packet.ParseFromString(data)
        except TypeError:
            continue

        if packet:
            print(f'{dt.datetime.now()}: referee was received.')
            publisher.send('referee', packet)
        else:
            print(f'No message: {len(data)}')

        # TODO: Sleep to reduce CPU usage.
