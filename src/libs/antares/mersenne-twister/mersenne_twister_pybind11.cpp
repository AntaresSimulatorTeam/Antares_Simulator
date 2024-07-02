#include <pybind11/pybind11.h>

#include <antares/mersenne-twister/mersenne-twister.h>

namespace py = pybind11;

PYBIND11_MODULE(mersenne_twister_pybind11, m)
{
    using namespace Antares;
    py::class_<Antares::MersenneTwister>(m, "mersenne_twister")
      .def(py::init<>()) // default constructor
      .def("reset", py::overload_cast<uint>(&MersenneTwister::reset))
      .def("next", &MersenneTwister::next)
      .def_static("min", &MersenneTwister::min)
      .def_static("max", &MersenneTwister::max);
}
