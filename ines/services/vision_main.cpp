#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/timestamp.pb.h>

#include "ines/common/publish_subscribe.h"
#include "ines/common/utility.h"
#include "ines/common/zmq_publish_subscribe.h"
#include "ines/services/common.pb.h"
#include "ines/services/vision.pb.h"
#include "ines/third_part/ssl_vision_wrapper.pb.h"

#include "google/protobuf/util/time_util.h"

#include <net/if.h>
#include <sys/ioctl.h>

using ines::ITopicPublisher;
// using ines::PubSubMode;
using ines::ZmqPublisher;

constexpr int kBufferSize = 1024;
constexpr int kPort = 10006;

int main() {
  // Define the multicast group and port
  std::string multicast_group = "224.5.23.2";
  int multicast_port = 10006;

  // Define the network interface information
  std::string interface_address = "192.168.1.18";
  std::string interface_name = "wlp60s0";

  // Create a UDP socket
  int sock = socket(AF_INET, SOCK_DGRAM, 0);

  // Set the socket options to enable multicast and specify the network interface
  int reuse_addr = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));

  struct sockaddr_in bind_address {};
  memset(&bind_address, 0, sizeof(bind_address));
  bind_address.sin_family = AF_INET;
  bind_address.sin_port = htons(multicast_port);
  bind_address.sin_addr.s_addr = inet_addr(multicast_group.c_str());
  bind(sock, (struct sockaddr*) &bind_address, sizeof(bind_address));

  struct ip_mreqn mreq {};
  memset(&mreq, 0, sizeof(mreq));
  mreq.imr_multiaddr.s_addr = inet_addr(multicast_group.c_str());
  mreq.imr_address.s_addr = inet_addr(interface_address.c_str());
  setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, &mreq, sizeof(mreq));

  // Receive and process packets indefinitely
  while (true) {
    std::array<char, kBufferSize> buffer = {};
    struct sockaddr_in sender {};
    socklen_t sender_len = sizeof(sender);
    ssize_t received = recvfrom(sock,
                                buffer.data(),
                                kBufferSize,
                                0,
                                reinterpret_cast<sockaddr*>(&sender),
                                &sender_len);

    if (received == 0) {
      continue;
    }

    // std::cout << "Received packet from " << inet_ntoa(sender.sin_addr) << ": "
    //           << std::string(buffer.data(), received) << std::endl;
    SSL_WrapperPacket packet;
    if (packet.ParseFromArray(buffer.data(), static_cast<int>(received))) {
      if (packet.has_detection()) {
        std::cout << "Received detection packet." << std::endl;
      }

      if (packet.has_geometry()) {
        std::cout << "Received geometry packet." << std::endl;
      }
    } else {
      std::cout << "Failed to parse packet." << std::endl;
    }
  }

  close(sock);

  return 0;
}
