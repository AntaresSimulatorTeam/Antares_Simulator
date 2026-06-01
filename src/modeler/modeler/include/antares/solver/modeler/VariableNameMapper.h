// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

namespace Antares::Solver
{

/// Maps a variable name to another name, using the optional study file
/// `<studyPath>/input/variable-names.yml` (a flat `source-name: mapped-name` map).
/// Names absent from the map are returned unchanged.
class VariableNameMapper
{
public:
    //! Default mapper: empty map, every name is returned unchanged
    VariableNameMapper() = default;
    //! Load the map from `<studyPath>/input/variable-names.yml` (empty if the file is absent)
    explicit VariableNameMapper(const std::filesystem::path& studyPath);

    [[nodiscard]] std::string mapOutput(const std::string& name) const;

private:
    std::unordered_map<std::string, std::string> nameMap_;
};

} // namespace Antares::Solver
