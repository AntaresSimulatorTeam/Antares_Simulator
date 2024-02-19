#include <pybind11/pybind11.h>
#include <antares/mersenne-twister/mersenne-twister.h>

namespace py = pybind11;

PYBIND11_MODULE(mersenne_twister_pybind11, m) {
    py::class_<Antares::MersenneTwister>(m, "mersenne_twister")
        .def(py::init<>()) // default constructor
        .def("reset", py::overload_cast<uint>(&Antares::MersenneTwister::reset))
        .def("next", &Antares::MersenneTwister::next)
        .def_static("min", &Antares::MersenneTwister::min)
        .def_static("max", &Antares::MersenneTwister::max);
}

