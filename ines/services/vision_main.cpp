#include <chrono>
#include <numbers>
#include <ranges>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <absl/random/distributions.h>
#include <absl/random/random.h>
#include <absl/time/clock.h>
#include <absl/time/time.h>

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
using ines::PubSubMode;
using ines::timestampFromNanos;
using ines::ZmqPublisher;
using ines::vision::Ball;
using ines::vision::Field;
using ines::vision::Frame;
using ines::vision::Robot;

static constexpr float kFieldLength = 9000.0F;
static constexpr float kFieldWidth = 6000.0F;
static constexpr float kGoalDepth = 180.0F;
static constexpr float kGoalWidth = 1000.0F;
static constexpr float kPenaltyAreaDepth = 1000.0F;
static constexpr float kPenaltyAreaWidth = 2000.0F;
static constexpr float kGoalCenterToPenaltyMark = 6000.0F;

static constexpr double k240HzInMs = 1000.0 / 240.0;

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
  server_address.sin_addr.s_addr = inet_addr("192.168.1.18");
  server_address.sin_port = htons(kPort);

  if (bind(sockfd,
           static_cast<sockaddr*>(static_cast<void*>(&server_address)),
           sizeof(server_address)) < 0) {
    std::cerr << "Failed to bind socket." << std::endl;
    return 1;
  }

  std::array<char, kBufferSize> buffer{};
  sockaddr_in client_address{};
  socklen_t client_len = sizeof(client_address);

  while (true) {
    ssize_t received = recvfrom(sockfd,
                                &buffer,
                                kBufferSize,
                                0,
                                static_cast<sockaddr*>(static_cast<void*>(&client_address)),
                                &client_len);
    if (received < 0) {
      std::cerr << "Error in receiving data." << std::endl;
      continue;
    }

    SSL_WrapperPacket packet;
    if (!packet.ParseFromArray(buffer, received)) {
      std::cerr << "Failed to parse SSL_WrapperPacket." << std::endl;
      continue;
    }

    std::cout << "Number of detected robots: " << packet.detection().robots().size() << std::endl;
  }

  close(sockfd);
  return 0;
}
