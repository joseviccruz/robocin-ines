#include <absl/time/clock.h>
#include <ranges>

#include <absl/random/distributions.h>
#include <absl/random/random.h>

#include "ines/common/publish_subscribe.h"
#include "ines/common/utility.h"
#include "ines/common/zmq_publish_subscribe.h"
#include "ines/services/referee.pb.h"

using ines::ITopicPublisher;
using ines::PubSubMode;
using ines::timestampFromTime;
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
  Command command;

  switch (command_case) {
    case Command::kHalt: command.mutable_halt(); break;
    case Command::kPlay: command.mutable_play(); break;
    case Command::kStop: command.mutable_stop(); break;
    case Command::kHomeBallPlacement: command.mutable_home_ball_placement(); break;
    case Command::kAwayBallPlacement: command.mutable_away_ball_placement(); break;
    case Command::kHomePrepareKickoff: command.mutable_home_prepare_kickoff(); break;
    case Command::kAwayPrepareKickoff: command.mutable_away_prepare_kickoff(); break;
    case Command::kHomeKickoff: command.mutable_home_kickoff(); break;
    case Command::kAwayKickoff: command.mutable_away_kickoff(); break;
    case Command::kHomePreparePenalty: command.mutable_home_prepare_penalty(); break;
    case Command::kAwayPreparePenalty: command.mutable_away_prepare_penalty(); break;
    case Command::kHomePenalty: command.mutable_home_penalty(); break;
    case Command::kAwayPenalty: command.mutable_away_penalty(); break;
    case Command::kHomePrepareDirectFreeKick:
      command.mutable_home_prepare_direct_free_kick();
      break;
    case Command::kAwayPrepareDirectFreeKick:
      command.mutable_away_prepare_direct_free_kick();
      break;
    case Command::kHomeDirectFreeKick: command.mutable_home_direct_free_kick(); break;
    case Command::kAwayDirectFreeKick: command.mutable_away_direct_free_kick(); break;
    case Command::kHomePrepareIndirectFreeKick:
      command.mutable_home_prepare_indirect_free_kick();
      break;
    case Command::kAwayPrepareIndirectFreeKick:
      command.mutable_away_prepare_indirect_free_kick();
      break;
    case Command::kHomeIndirectFreeKick: command.mutable_home_indirect_free_kick(); break;
    case Command::kAwayIndirectFreeKick: command.mutable_away_indirect_free_kick(); break;
    case Command::kHomeTimeout: command.mutable_home_timeout(); break;
    case Command::kAwayTimeout: command.mutable_away_timeout(); break;
    case Command::kInterval: command.mutable_interval(); break;
    default: command.mutable_halt(); break;
  }

  return command;
}

int main() {
  std::unique_ptr<ITopicPublisher> publisher = std::make_unique<ZmqPublisher>();
  publisher->bind("ipc:///tmp/referee.ipc");

  int64_t total = 0;

  while (true) {
    Status status;
    *status.mutable_event_timestamp() = timestampFromTime(absl::Now());
    *status.mutable_home_team() = getMockedTeam("Home");
    *status.mutable_away_team() = getMockedTeam("Away");

    auto command_case = static_cast<Command::CommandCase>(absl::Uniform<int>(bitgen, 0, 24));
    *status.mutable_command() = getMockedCommand(command_case);

    for ([[maybe_unused]] int reps : std::ranges::iota_view(0, 16)) {
      *status.mutable_message_timestamp() = timestampFromTime(absl::Now());

      status.set_id(total);
      status.set_total_commands(total);

      publisher->send("status", PubSubMode::DontWait, status);

      ++total;
      absl::SleepFor(absl::Milliseconds(k60HzInMs));
    }
  }

  return 0;
}
