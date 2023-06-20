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

using ines::ITopicPublisher;
// using ines::PubSubMode;
using ines::ZmqPublisher;

constexpr int kBufferSize = 1024;
constexpr int kPort = 10006;

int main() {
  std::unique_ptr<ITopicPublisher> publisher = std::make_unique<ZmqPublisher>();
  publisher->bind("ipc:///tmp/vision.ipc");

  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    std::cerr << "Failed to create socket." << std::endl;
    return 1;
  }

  sockaddr_in server_address{};
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(kPort);

  if (bind(sockfd, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)) < 0) {
    std::cerr << "Failed to bind socket." << std::endl;
    close(sockfd);
    return 1;
  }

  ip_mreq multicast_request{};
  multicast_request.imr_multiaddr.s_addr = inet_addr("224.5.23.2");
  multicast_request.imr_interface.s_addr = htonl(INADDR_ANY);
  if (setsockopt(sockfd,
                 IPPROTO_IP,
                 IP_ADD_MEMBERSHIP,
                 reinterpret_cast<char*>(&multicast_request),
                 sizeof(multicast_request)) < 0) {
    std::cerr << "Failed to join multicast group." << std::endl;
    close(sockfd);
    return 1;
  }

  std::array<char, kBufferSize> buffer{};
  sockaddr_in client_address{};
  socklen_t client_len = sizeof(client_address);

  while (true) {
    std::cout << "Receiving..." << std::endl;

    ssize_t received = recvfrom(sockfd,
                                buffer.data(),
                                kBufferSize,
                                0,
                                reinterpret_cast<sockaddr*>(&client_address),
                                &client_len);

    if (received < 0) {
      std::cerr << "Error in receiving data. Error code: " << errno << std::endl;
      continue;
    }

    char client_ip[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &(client_address.sin_addr), client_ip, INET_ADDRSTRLEN) == nullptr) {
      std::cerr << "Error converting client address to string." << std::endl;
      continue;
    }

    std::cout << "Received data from " << client_ip << ":" << ntohs(client_address.sin_port)
              << std::endl;

    SSL_WrapperPacket packet;
    if (packet.ParseFromArray(buffer.data(), static_cast<int>(received))) {
      if (packet.has_detection()) {
        std::cout << "Received detection packet." << std::endl;
      }

      if (packet.has_geometry()) {
        std::cout << "Received geometry packet." << std::endl;
      }
    }

    // Clear the buffer for the next receive operation
    std::memset(buffer.data(), 0, kBufferSize);
  }

  close(sockfd);
  return 0;
}
