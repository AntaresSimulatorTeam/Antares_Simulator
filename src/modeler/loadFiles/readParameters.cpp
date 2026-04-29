// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <yaml-cpp/yaml.h>

#include <antares/io/file.h>
#include <antares/logs/logs.h>
#include "antares/solver/modeler/loadFiles/loadFiles.h"
#include "antares/solver/modeler/parameters/parseModelerParameters.h"

namespace fs = std::filesystem;

namespace Antares::Solver::LoadFiles
{

ModelerParameters loadParameters(const fs::path& studyPath)
{
    std::string filename = "parameters.yml";
    std::string paramStr;
    try
    {
        paramStr = IO::readFile(studyPath / filename);
    }
    catch (const std::runtime_error& e)
    {
        logs.error() << "Error while trying to read file parameters.yml";
        throw ErrorLoadingYaml(e.what());
    }

    try
    {
        return parseModelerParameters(paramStr);
    }
    catch (const YAML::Exception& e)
    {
        handleYamlError(e, filename);
        throw ErrorLoadingYaml(e.what());
    }
}

} // namespace Antares::Solver::LoadFiles
