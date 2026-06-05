// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

namespace Antares::Solver
{

/// Maps a variable name to another name, using a flat `source-name: mapped-name`
/// YAML map. Names absent from the map are returned unchanged.
class VariableNameMapper
{
public:
    //! Default mapper: empty map, every name is returned unchanged
    VariableNameMapper() = default;

    /// Load the mapping for the modeler. The study file
    /// `<studyPath>/input/variable-names.yml` takes precedence; when it is absent the
    /// shipped default file `defaultFilePath` is used instead.
    /// Throws std::runtime_error when neither file exists.
    VariableNameMapper(const std::filesystem::path& studyPath,
                       const std::filesystem::path& defaultFilePath);

    [[nodiscard]] std::string mapOutput(const std::string& name) const;

private:
    void loadFromFile(const std::filesystem::path& filePath);

    std::unordered_map<std::string, std::string> nameMap_;
};

} // namespace Antares::Solver
