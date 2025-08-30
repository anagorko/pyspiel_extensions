#ifndef SUBTRACTION_GAME_H_
#define SUBTRACTION_GAME_H_

#include <memory>
#include <string>
#include <vector>

//#include "absl/types/span.h"
#include "open_spiel/spiel.h"
//#include "open_spiel/spiel_globals.h"

namespace pyspiel_extensions {
namespace subtraction_game {

// Constants.
inline constexpr int kNumPlayers = 2;

class SubtractionGameState : public open_spiel::State {
private:
    int current_player_;
    int max_removal_;
    int pile;

public:
  SubtractionGameState(std::shared_ptr<const open_spiel::Game> game, int initial_pile_size, int max_removal);
  SubtractionGameState(const SubtractionGameState&) = default;
  SubtractionGameState& operator=(const SubtractionGameState&) = delete;

  open_spiel::Player CurrentPlayer() const override {
      return IsTerminal() ? open_spiel::kTerminalPlayerId : current_player_;
  }

  std::string ActionToString(open_spiel::Player player, open_spiel::Action action_id) const override;
  std::string ToString() const override;
  bool IsTerminal() const override;
  std::vector<double> Returns() const override;
  std::unique_ptr<State> Clone() const override {
      return std::unique_ptr<State>(new SubtractionGameState(*this));
  }
  void UndoAction(open_spiel::Player player, open_spiel::Action move) override;
  std::vector<open_spiel::Action> LegalActions() const override;

 protected:
  void DoApplyAction(open_spiel::Action move) override;
};

// Game object.
class SubtractionGame : public open_spiel::Game {
private:
    int pile_size_;
    int max_removal_;

public:
  explicit SubtractionGame(const open_spiel::GameParameters& params);
  int NumDistinctActions() const override { return max_removal_; }
  std::unique_ptr<open_spiel::State> NewInitialState() const override {
    return std::unique_ptr<open_spiel::State>(new SubtractionGameState(shared_from_this(), pile_size_, max_removal_));
  }
  int NumPlayers() const override { return kNumPlayers; }
  double MinUtility() const override { return -1; }
  absl::optional<double> UtilitySum() const override { return 0; }
  double MaxUtility() const override { return 1; }
  std::vector<int> ObservationTensorShape() const override {
    return {0};
  }
  int MaxGameLength() const override { return 1; }
  std::string ActionToString(open_spiel::Player player, open_spiel::Action action_id) const override;
};

}  // namespace subtraction_game
}  // namespace pyspiel_extensions

#endif  // SUBTRACTION_GAME_H_
