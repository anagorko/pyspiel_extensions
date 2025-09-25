#include <pybind11/pybind11.h>

#include "subtraction_game/subtraction_game_pybind11.h"
#include "backward_induction/backward_induction_pybind11.h"


PYBIND11_MODULE(_core, m) {
    m.doc() = "pybind11 extension for pyspiel additions";

    pyspiel_extensions::subtraction_game::init_subtraction_game_bindings(m);
    pyspiel_extensions::backward_induction::init_backward_induction_bindings(m);
}
