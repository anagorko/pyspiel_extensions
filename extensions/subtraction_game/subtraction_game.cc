#include "subtraction_game.h"

#include <memory>

#include "open_spiel/spiel_utils.h"

namespace pyspiel_extensions {
namespace subtraction_game {

namespace {

// Facts about the game.
const open_spiel::GameType kGameType {
    /*short_name=*/"subtraction_game",
    /*long_name=*/"Subtraction Game",
    open_spiel::GameType::Dynamics::kSequential,
    open_spiel::GameType::ChanceMode::kDeterministic,
    open_spiel::GameType::Information::kPerfectInformation,
    open_spiel::GameType::Utility::kZeroSum,
    open_spiel::GameType::RewardModel::kTerminal,
    /*max_num_players=*/2,
    /*min_num_players=*/2,
    /*provides_information_state_string=*/false,
    /*provides_information_state_tensor=*/false,
    /*provides_observation_string=*/false,
    /*provides_observation_tensor=*/false,
    /*parameter_specification=*/
    {
        {"pile_size", open_spiel::GameParameter(std::string("21"))},
        {"max_removel", open_spiel::GameParameter(3)}
    }
};

std::shared_ptr<const open_spiel::Game> Factory(const open_spiel::GameParameters& params) {
  return std::shared_ptr<const open_spiel::Game>(new SubtractionGame(params));
}

open_spiel::REGISTER_SPIEL_GAME(kGameType, Factory);

}  // namespace

SubtractionGameState::SubtractionGameState(std::shared_ptr<const open_spiel::Game> game, int initial_pile_size, int max_removal) :
    open_spiel::State(game),
    pile(initial_pile_size),
    max_removal_(max_removal) {}

std::string SubtractionGameState::ActionToString(open_spiel::Player player,
                                                 open_spiel::Action action_id) const {
  return game_->ActionToString(player, action_id);
}

std::string SubtractionGameState::ToString() const {
    return absl::StrCat(pile);
}

bool SubtractionGameState::IsTerminal() const {
    return pile == 0;
}

std::vector<double> SubtractionGameState::Returns() const {
    if (pile == 0) {
        if (current_player_ == 0) {
            return { 1.0, -1.0 };
        } else {
            return { -1.0, 1.0 };
        }
    }
    return { 0.0, 0.0 };;
}

void SubtractionGameState::UndoAction(open_spiel::Player player, open_spiel::Action move) {
  current_player_ = player;
  history_.pop_back();
  pile += move;
  --move_number_;
}

std::vector<open_spiel::Action> SubtractionGameState::LegalActions() const {
    std::vector<open_spiel::Action> actions(max_removal_);
    std::generate(actions.begin(), actions.end(), [n = 1] () mutable { return n++; });
    return actions;
}

void SubtractionGameState::DoApplyAction(open_spiel::Action move) {
    pile -= move;
    current_player_ = 1 - current_player_;
}

SubtractionGame::SubtractionGame(const open_spiel::GameParameters& params) : open_spiel::Game(kGameType, params),
      pile_size_(ParameterValue<int>("pile_size", 21)),
      max_removal_(ParameterValue<int>("max_removal", 3)) {}

std::string SubtractionGame::ActionToString(open_spiel::Player player,
                                            open_spiel::Action action_id) const {
    return absl::StrCat(player, "/", action_id);
}

}  // namespace graph_game
}  // namespace open_spiel
