#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "open_spiel/spiel.h"

namespace pyspiel_extensions {
namespace subtraction_game {


namespace py = pybind11;

PYBIND11_MODULE(shared_library, m) {
    m.doc() = "pybind11 extension for pyspiel additions";

    m.def("load_game",
          [](const std::string& game_name) {
            return open_spiel::LoadGame(game_name);
          },
          "Loads an OpenSpiel game with default parameters.",
          py::arg("game_name")
    );

    m.def("load_game",
          [](const std::string& game_name,
             const std::map<std::string, open_spiel::GameParameter>& params) {
            return open_spiel::LoadGame(game_name, params);
          },
          "Loads an OpenSpiel game with specified parameters.",
          py::arg("game_name"),
          py::arg("params")
    );
}

}  // namespace subtraction_game
}  // namespace pyspiel_extensions
