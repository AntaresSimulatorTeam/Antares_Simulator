// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once
#include <string>
#include <unordered_set>

namespace Antares::IO::Outputs
{
std::string MakeExportableName(const std::string& name,
                               const std::string& forbiddenFirstChars,
                               const std::string& forbiddenChars,
                               bool* foundForbiddenChar);

class NameManager
{
public:
    std::string MakeUniqueName(const std::string& name);

private:
    std::unordered_set<std::string> names_;
    int lastN_ = 1;
};

std::string MakeMpsSafeUniqueName(const std::string& originalName, NameManager& nameManager);

} // namespace Antares::IO::Outputs
