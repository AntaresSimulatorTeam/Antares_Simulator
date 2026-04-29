/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
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
