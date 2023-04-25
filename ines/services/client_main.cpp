#include <ranges>
#include <thread>

#include <absl/synchronization/mutex.h>
#include <absl/time/clock.h>
#include <absl/time/time.h>

#include "ines/common/publish_subscribe.h"
#include "ines/common/zmq_publish_subscribe.h"
#include "ines/services/common.pb.h"
#include "ines/services/vision.pb.h"

#include "google/protobuf/util/time_util.h"

using ines::ITopicSubscriber;
using ines::PubSubMode;
using ines::ZmqSubscriber;
using ines::vision::Field;
using ines::vision::Frame;

absl::Time fromTimestamp(const google::protobuf::Timestamp& timestamp) {
  return absl::FromUnixNanos(timestamp.seconds() * 1'000'000'000L + timestamp.nanos());
}

int main() {
  std::cout.setf(std::ios::fixed);
  std::cout.precision(3);

  absl::Mutex mutex;

  std::thread vision_thread([&]() {
    std::unique_ptr<ITopicSubscriber> vision_subscriber = std::make_unique<ZmqSubscriber>("vision");
    vision_subscriber->connect("ipc:///tmp/vision.ipc");

    for (auto reps : std::ranges::iota_view<int64_t>{1}) {
      Frame frame;
      if (vision_subscriber->receive(PubSubMode::DontWait, frame)) {
        absl::MutexLock lock(&mutex);

        auto duration = absl::Now() - fromTimestamp(frame.timestamp());
        int64_t current_ns = absl::ToInt64Nanoseconds(duration);
        std::cout << "vision delay: " << current_ns / 1e6 << std::endl;
        // std::cout << "Received frame: " << frame.DebugString();

        absl::SleepFor(absl::Milliseconds(2));
      }
    }
  });

  std::thread field_thread([&]() {
    std::unique_ptr<ITopicSubscriber> field_subscriber = std::make_unique<ZmqSubscriber>("field");
    field_subscriber->connect("ipc:///tmp/vision.ipc");

    for (auto reps : std::ranges::iota_view<int64_t>{1}) {
      Field field;
      if (field_subscriber->receive(PubSubMode::DontWait, field)) {
        absl::MutexLock lock(&mutex);

        auto duration = absl::Now() - fromTimestamp(field.timestamp());
        int64_t current_ns = absl::ToInt64Nanoseconds(duration);
        std::cout << "field delay: " << std::endl;
        // std::cout << "field: " << field.DebugString();

        absl::SleepFor(absl::Milliseconds(2));
      }
    }
  });

  vision_thread.join();
  field_thread.join();

  return 0;
}
