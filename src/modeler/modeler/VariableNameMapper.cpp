// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/modeler/VariableNameMapper.h"

#include <stdexcept>
#include <yaml-cpp/yaml.h>

#include <antares/io/file.h>
#include <antares/logs/logs.h>

namespace fs = std::filesystem;

namespace Antares::Solver
{

VariableNameMapper::VariableNameMapper(const fs::path& studyPath, const fs::path& defaultFilePath)
{
    const fs::path studyFilePath = studyPath / "input" / "variable-names.yml";
    if (fs::exists(studyFilePath))
    {
        logs.info() << "Using variable names file from study: " << studyFilePath;
        loadFromFile(studyFilePath);
        return;
    }

    if (fs::exists(defaultFilePath))
    {
        logs.info() << "No variable names file in study, using default: " << defaultFilePath;
        loadFromFile(defaultFilePath);
        return;
    }

    throw std::runtime_error("No variable names file found: neither in the study ("
                             + studyFilePath.string() + ") nor as a default ("
                             + defaultFilePath.string() + ")");
}

void VariableNameMapper::loadFromFile(const fs::path& filePath)
{
    std::string content;
    try
    {
        content = IO::readFile(filePath);
    }
    catch (const std::runtime_error&)
    {
        logs.error() << "Error while trying to read file " << filePath;
        throw;
    }

    try
    {
        const YAML::Node root = YAML::Load(content);
        for (const auto& entry: root)
        {
            nameMap_.emplace(entry.first.as<std::string>(), entry.second.as<std::string>());
        }
    }
    catch (const YAML::Exception& e)
    {
        logs.error() << "Error while parsing file " << filePath << ": " << e.what();
        throw std::runtime_error(e.what());
    }
}

std::string VariableNameMapper::mapOutput(const std::string& name) const
{
    if (const auto it = nameMap_.find(name); it != nameMap_.end())
    {
        return it->second;
    }
    return name;
}

} // namespace Antares::Solver
