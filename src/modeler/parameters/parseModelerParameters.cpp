// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <filesystem>

#include <antares/io/file.h>
#include <antares/solver/modeler/parameters/parseModelerParameters.h>

#include "encoder.hxx"

namespace Antares::Solver::LoadFiles
{

ModelerParameters parseModelerParameters(const std::string& content)
{
    YAML::Node root = YAML::Load(content);
    return root.as<ModelerParameters>();
}

} // namespace Antares::Solver::LoadFiles
