#pragma once
#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace pyspiel_extensions {
namespace backward_induction {

// Declares the function that will add the backward_induction bindings.
void init_backward_induction_bindings(py::module_& m);

}  // namespace backward_induction
}  // namespace pyspiel_extensions
