// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
