/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
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
#include <antares/benchmarking/file_content.h>
#include <antares/inifile/inifile.h>

using namespace std;

namespace Benchmarking
{
FileContent::iterator FileContent::begin()
{
    return sections_.begin();
}

FileContent::iterator FileContent::end()
{
    return sections_.end();
}

void FileContent::addItemToSection(const string& section, const string& key, int value)
{
    std::lock_guard guard(pSectionsMutex);
    sections_[section][key] = to_string(value);
}

void FileContent::addItemToSection(const string& section, const string& key, const string& value)
{
    std::lock_guard guard(pSectionsMutex);
    sections_[section][key] = value;
}

void FileContent::addDurationItem(const string& name, unsigned int duration, int nbCalls)
{
    addItemToSection("durations_ms", name, duration);
    addItemToSection("number_of_calls", name, nbCalls);
}

// TODO should be const
std::string FileContent::saveToBufferAsIni()
{
    Antares::IniFile ini;
    for (const auto& [sectionName, content]: *this)
    {
        // Loop on properties
        auto* section = ini.addSection(sectionName);
        for (const auto& [key, value]: content)
        {
            section->add(key, value);
        }
    }
    return ini.toString();
}
} // namespace Benchmarking
