import time
import network
import argparse

from robocin.pb.ssl.third_party.detection.raw_wrapper_pb2 import SSL_WrapperPacket

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--ip_address', default='224.5.23.2')
    parser.add_argument('--port', default=10006)
    parser.add_argument('--network_interface', required=True)

    args = parser.parse_args()

    udp_socket = network.MulticastUdpSocket()
    udp_socket.bind(args.ip_address, args.port, args.network_interface)

    while True:
        data = udp_socket.receive()

        packet = SSL_WrapperPacket()
        packet.ParseFromString(data)

        print(packet)

        time.sleep(0.003)
