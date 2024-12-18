#pragma once

#include <filesystem>
#include <modelerParameters.h>

namespace Antares::Solver
{

ModelerParameters parseModelerParameters(const std::filesystem::path& path);
} // namespace Antares::Solver
