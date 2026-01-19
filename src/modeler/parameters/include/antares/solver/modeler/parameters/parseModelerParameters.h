// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>

#include <antares/solver/modeler/parameters/modelerParameters.h>

namespace Antares::Solver::LoadFiles
{

ModelerParameters parseModelerParameters(const std::string& content);

} // namespace Antares::Solver::LoadFiles
