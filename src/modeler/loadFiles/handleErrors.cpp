// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <sstream>

#include <antares/logs/logs.h>
#include "antares/solver/modeler/loadFiles/loadFiles.h"

namespace Antares::Solver::LoadFiles
{

void handleYamlError(const YAML::Exception& e, const std::string& context)
{
    logs.error() << "Error while parsing the yaml file: " << context;
    if (!e.mark.is_null())
    {
        logs.error() << "Line " << e.mark.line << " column " << e.mark.column;
    }
    logs.error() << e.what();
}

std::string markYamlError(const YAML::Exception& e, const std::string& context)
{
    std::ostringstream ss;
    ss << "Error while parsing the yaml file: " << context;
    if (!e.mark.is_null())
    {
        ss << "\nLine " << e.mark.line << " column " << e.mark.column;
    }
    ss << "\n" << e.what();
    return ss.str();
}
} // namespace Antares::Solver::LoadFiles
