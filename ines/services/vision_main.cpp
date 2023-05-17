#include <chrono>
#include <numbers>
#include <ranges>

#include <absl/random/distributions.h>
#include <absl/random/random.h>
#include <absl/time/clock.h>
#include <absl/time/time.h>

#include <google/protobuf/timestamp.pb.h>

#include "ines/common/publish_subscribe.h"
#include "ines/common/utility.h"
#include "ines/common/zmq_publish_subscribe.h"
#include "ines/services/common.pb.h"
#include "ines/services/vision.pb.h"

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

absl::BitGen bitgen;

float getRandomX() {
  static constexpr float kHalfFieldLength = kFieldLength / 2.0F;
  return absl::Uniform(bitgen, -kHalfFieldLength, kHalfFieldLength);
}

float getRandomY() {
  static constexpr float kHalfFieldWidth = kFieldWidth / 2.0F;
  return absl::Uniform(bitgen, -kHalfFieldWidth, kHalfFieldWidth);
}

float getRandomAngle() {
  static constexpr float kPi = std::numbers::pi_v<float>;

  return absl::Uniform(bitgen, -kPi, kPi);
}

Field getDivBField(uint64_t id) {
  Field result;
  result.set_id(id);
  *result.mutable_timestamp() = timestampFromNanos(absl::GetCurrentTimeNanos());

  result.set_length(kFieldLength);
  result.set_width(kFieldWidth);
  result.set_goal_depth(kGoalDepth);
  result.set_goal_width(kGoalWidth);
  result.set_penalty_area_depth(kPenaltyAreaDepth);
  result.set_penalty_area_width(kPenaltyAreaWidth);
  result.set_goal_center_to_penalty_mark(kGoalCenterToPenaltyMark);

  return result;
}

Ball getMockedBall() {
  Ball result;

  result.mutable_position()->set_x(getRandomX());
  result.mutable_position()->set_y(getRandomY());

  result.mutable_velocity()->set_x(0.0F);
  result.mutable_velocity()->set_y(0.0F);

  return result;
}

Robot getMockedRobot(int id) {
  Robot result;
  result.set_id(id);

  result.mutable_position()->set_x(getRandomX());
  result.mutable_position()->set_y(getRandomY());

  result.set_angle(getRandomAngle());

  result.mutable_velocity()->set_x(0.0F);
  result.mutable_velocity()->set_y(0.0F);

  result.set_angular_velocity(0.0F);

  return result;
}

Frame getMockedFrame(uint64_t id) {
  Frame result;
  result.set_id(id);
  *result.mutable_timestamp() = timestampFromNanos(absl::GetCurrentTimeNanos());

  // result.add_source_ids("robot_0_ir");
  // result.add_source_ids("camera_1");
  // result.add_source_ids("camera_2");
  // result.add_source_ids("camera_3");
  // result.add_source_ids("camera_4");

  *result.mutable_ball() = getMockedBall();
  for (int i : std::ranges::iota_view(0, 6)) {
    *result.add_teammates() = getMockedRobot(i);
    *result.add_opponents() = getMockedRobot(i);
  }

  return result;
}

int main() {
  std::unique_ptr<ITopicPublisher> publisher = std::make_unique<ZmqPublisher>();
  publisher->bind("ipc:///tmp/vision.ipc");

  for (uint64_t frame_id : std::ranges::iota_view<uint64_t>(0)) {
    Frame frame = getMockedFrame(frame_id);
    // *frame.mutable_timestamp() = getNow();
    publisher->send("vision", PubSubMode::DontWait, frame);

    if (frame_id % 120 == 0) { // Send field every 120 frames
      Field field = getDivBField(frame_id / 120);
      //*field.mutable_timestamp() = getNow();
      publisher->send("field", PubSubMode::DontWait, field);
    }

    absl::SleepFor(absl::Milliseconds(k240HzInMs));
  }

  return 0;
}
