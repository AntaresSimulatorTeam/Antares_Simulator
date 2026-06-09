// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#include "antares/io/outputs/ExportableName.h"

#include "antares/logs/logs.h"

namespace Antares::IO::Outputs
{
const static std::string_view forbiddenFirstChars = "$.0123456789";
const static std::string_view forbiddenChars = " /\\";

std::string MakeExportableName(const std::string& name)
{
    bool foundForbiddenChar = name.empty()
                              || forbiddenFirstChars.find(name.front()) != std::string::npos;

    std::string exportable = foundForbiddenChar ? "_" + name : name;

    for (char& c: exportable)
    {
        if (forbiddenChars.find(c) != std::string::npos)
        {
            c = '_';
            foundForbiddenChar = true;
        }
    }
    // if (foundForbiddenChar)
    // {
    //     logs.debug() << "forbidden char(s) found in id '" << name << "', replaced with '_'";
    // }

    return exportable;
}

std::string NameManager::MakeUniqueName(const std::string& name)
{
    std::string result = name;
    int n = lastN_;

    while (!names_.insert(result).second)
    {
        result = name + "_" + std::to_string(n);
        ++n;
    }

    lastN_ = n;
    return result;
}

std::string MakeMpsSafeUniqueName(const std::string& originalName, NameManager& nameManager)
{
    const std::string exportable = MakeExportableName(originalName);

    return nameManager.MakeUniqueName(exportable);
}

} // namespace Antares::IO::Outputs
