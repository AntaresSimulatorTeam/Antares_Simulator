#pragma once

#include <filesystem>

#include <antares/solver/modeler/parameters/modelerParameters.h>

namespace Antares::Solver
{

ModelerParameters parseModelerParameters(const std::filesystem::path& path);
} // namespace Antares::Solver
