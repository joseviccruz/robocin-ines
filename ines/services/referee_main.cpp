#include <absl/time/clock.h>
#include <memory>
#include <ranges>

#include <absl/random/distributions.h>
#include <absl/random/random.h>
#include <type_traits>

#include "ines/common/publish_subscribe.h"
#include "ines/common/utility.h"
#include "ines/common/zmq_publish_subscribe.h"
#include "ines/services/referee.pb.h"

using ines::ITopicPublisher;
using ines::PubSubMode;
using ines::timestampFromNanos;
using ines::ZmqPublisher;
using ines::referee::Command;
using ines::referee::Status;
using ines::referee::Team;

static constexpr double k60HzInMs = 1000.0 / 60.0;

absl::BitGen bitgen;

Team getMockedTeam(std::string_view name) {
  Team team;
  team.set_name(name.data());
  team.set_score(absl::Uniform<int>(bitgen, 0, 10));
  team.set_yellow_cards(absl::Uniform<int>(bitgen, 0, 10));
  team.set_red_cards(absl::Uniform<int>(bitgen, 0, 10));
  team.set_timeouts(absl::Uniform<int>(bitgen, 0, 10));

  team.set_goalkeeper_id(absl::Uniform<int>(bitgen, 0, 5));

  return team;
}

Command getMockedCommand(Command::CommandCase command_case) {
  if (command_case == Command::kHalt) {
    Command command;
    command.mutable_halt();
    return command;
  }
  if (command_case == Command::kPlay) {
    Command command;
    command.mutable_play();
    return command;
  }
  if (command_case == Command::kStop) {
    Command command;
    command.mutable_stop();
    return command;
  }
  if (command_case == Command::kHomeBallPlacement) {
    Command command;
    command.mutable_home_ball_placement();
    return command;
  }
  if (command_case == Command::kAwayBallPlacement) {
    Command command;
    command.mutable_away_ball_placement();
    return command;
  }
  if (command_case == Command::kHomePrepareKickoff) {
    Command command;
    command.mutable_home_prepare_kickoff();
    return command;
  }
  if (command_case == Command::kAwayPrepareKickoff) {
    Command command;
    command.mutable_away_prepare_kickoff();
    return command;
  }
  if (command_case == Command::kHomeKickoff) {
    Command command;
    command.mutable_home_kickoff();
    return command;
  }
  if (command_case == Command::kAwayKickoff) {
    Command command;
    command.mutable_away_kickoff();
    return command;
  }
  if (command_case == Command::kHomePreparePenalty) {
    Command command;
    command.mutable_home_prepare_penalty();
    return command;
  }
  if (command_case == Command::kAwayPreparePenalty) {
    Command command;
    command.mutable_away_prepare_penalty();
    return command;
  }
  if (command_case == Command::kHomePenalty) {
    Command command;
    command.mutable_home_penalty();
    return command;
  }
  if (command_case == Command::kAwayPenalty) {
    Command command;
    command.mutable_away_penalty();
    return command;
  }
  if (command_case == Command::kHomePrepareDirectFreeKick) {
    Command command;
    command.mutable_home_prepare_direct_free_kick();
    return command;
  }
  if (command_case == Command::kAwayPrepareDirectFreeKick) {
    Command command;
    command.mutable_away_prepare_direct_free_kick();
    return command;
  }
  if (command_case == Command::kHomeDirectFreeKick) {
    Command command;
    command.mutable_home_direct_free_kick();
    return command;
  }
  if (command_case == Command::kAwayDirectFreeKick) {
    Command command;
    command.mutable_away_direct_free_kick();
    return command;
  }
  if (command_case == Command::kHomePrepareIndirectFreeKick) {
    Command command;
    command.mutable_home_prepare_indirect_free_kick();
    return command;
  }
  if (command_case == Command::kAwayPrepareIndirectFreeKick) {
    Command command;
    command.mutable_away_prepare_indirect_free_kick();
    return command;
  }
  if (command_case == Command::kHomeIndirectFreeKick) {
    Command command;
    command.mutable_home_indirect_free_kick();
    return command;
  }
  if (command_case == Command::kAwayIndirectFreeKick) {
    Command command;
    command.mutable_away_indirect_free_kick();
    return command;
  }
  if (command_case == Command::kHomeTimeout) {
    Command command;
    command.mutable_home_timeout();
    return command;
  }
  if (command_case == Command::kAwayTimeout) {
    Command command;
    command.mutable_away_timeout();
    return command;
  }
  if (command_case == Command::kInterval) {
    Command command;
    command.mutable_interval();
    return command;
  }

  Command command;
  command.mutable_halt();
  return command;
}

int main() {
  std::unique_ptr<ITopicPublisher> publisher = std::make_unique<ZmqPublisher>();
  publisher->bind("ipc:///tmp/referee.ipc");

  int64_t total = 0;

  while (true) {
    Status status;
    *status.mutable_event_timestamp() = timestampFromNanos(absl::GetCurrentTimeNanos());
    *status.mutable_home_team() = getMockedTeam("Home");
    *status.mutable_away_team() = getMockedTeam("Away");

    auto command_case = static_cast<Command::CommandCase>(absl::Uniform<int>(bitgen, 0, 24));
    *status.mutable_command() = getMockedCommand(command_case);

    for ([[maybe_unused]] int reps : std::ranges::iota_view(0, 16)) {
      *status.mutable_timestamp() = timestampFromNanos(absl::GetCurrentTimeNanos());

      status.set_id(total);
      status.set_total_commands(total);

      publisher->send("status", PubSubMode::DontWait, status);

      ++total;
      absl::SleepFor(absl::Milliseconds(k60HzInMs));
    }
  }

  return 0;
}
