#pragma once
#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace pyspiel_extensions {
namespace subtraction_game {

// Declares the function that will add the subtraction_game bindings.
void init_subtraction_game_bindings(py::module_& m);

}  // namespace subtraction_game
}  // namespace pyspiel_extensions
