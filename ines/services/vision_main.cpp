#include <absl/time/clock.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string_view>
#include <sys/socket.h>

#include <absl/random/distributions.h>
#include <absl/random/random.h>
#include <type_traits>

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

static constexpr double k240HzInMs = 1000.0 / 240.0;

using ines::ITopicPublisher;
using ines::PubSubMode;
using ines::timestampFromNanos;
using ines::ZmqPublisher;
using ines::vision::Ball;
using ines::vision::Field;
using ines::vision::Frame;
using ines::vision::Robot;

constexpr int kBufferSize = 2048;

// Define the multicast group and port
constexpr int kMulticastPort = 10006;
constexpr std::string_view kMulticastGroup = "224.5.23.2";

// Define the network interface information
constexpr std::string_view kInterfaceAddress = "192.168.1.18";
constexpr std::string_view kInterfaceName = "wlp60s0";

int main() {
  std::unique_ptr<ITopicPublisher> publisher = std::make_unique<ZmqPublisher>();
  publisher->bind("ipc:///tmp/vision.ipc");

  // Create a UDP socket
  int sock = socket(AF_INET, SOCK_DGRAM, 0);

  // Set the socket options to enable multicast and specify the network interface
  int reuse_addr = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));

  struct sockaddr_in bind_address {};
  memset(&bind_address, 0, sizeof(bind_address));
  bind_address.sin_family = AF_INET;
  bind_address.sin_port = htons(kMulticastPort);
  bind_address.sin_addr.s_addr = inet_addr(kMulticastGroup.data());
  bind(sock, reinterpret_cast<struct sockaddr*>(&bind_address), sizeof(bind_address));

  struct ip_mreqn mreq {};
  memset(&mreq, 0, sizeof(mreq));
  mreq.imr_multiaddr.s_addr = inet_addr(kMulticastGroup.data());
  mreq.imr_address.s_addr = inet_addr(kInterfaceAddress.data());
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

    SSL_WrapperPacket packet;
    if (packet.ParseFromArray(buffer.data(), static_cast<int>(received))) {
      Frame frame;
      *frame.mutable_timestamp() = timestampFromNanos(absl::GetCurrentTimeNanos());

      for (const auto& robot_yellow : packet.detection().robots_yellow()) {
        Robot robot;
        robot.set_id(robot_yellow.robot_id());
        robot.set_angle(robot_yellow.orientation());
        robot.mutable_position()->set_x(robot_yellow.x());
        robot.mutable_position()->set_y(robot_yellow.y());
        *frame.add_teammates() = robot;
      }

      for (const auto& robot_blue : packet.detection().robots_blue()) {
        Robot robot;
        robot.set_id(robot_blue.robot_id());
        robot.set_angle(robot_blue.orientation());
        robot.mutable_position()->set_x(robot_blue.x());
        robot.mutable_position()->set_y(robot_blue.y());
        *frame.add_opponents() = robot;
      }

      for (const auto& ball : packet.detection().balls()) {
        Ball ball_msg;
        ball_msg.mutable_position()->set_x(ball.x());
        ball_msg.mutable_position()->set_y(ball.y());
        *frame.mutable_ball() = ball_msg;
      }
      publisher->send("vision", PubSubMode::DontWait, frame);

      if (packet.has_geometry()) {
        SSL_GeometryFieldSize field_size = packet.geometry().field();
        Field field;
        field.set_length(field_size.field_length());
        field.set_width(field_size.field_width());
        field.set_goal_depth(field_size.goal_depth());
        field.set_goal_width(field_size.goal_width());
        field.set_penalty_area_depth(field_size.penalty_area_depth());
        field.set_penalty_area_width(field_size.penalty_area_width());
        field.set_goal_center_to_penalty_mark(field_size.goal_center_to_penalty_mark());
        *field.mutable_timestamp() = timestampFromNanos(absl::GetCurrentTimeNanos());
        publisher->send("field", PubSubMode::DontWait, field);
      }
    } else {
      std::cout << "Failed to parse packet." << std::endl;
    }

    // absl::SleepFor(absl::Milliseconds(k240HzInMs));
  }

  close(sock);

  return 0;
}
