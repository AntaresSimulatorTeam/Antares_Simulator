// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/output.h"

#include <filesystem>

#include <yuni/io/directory/iterator.h>

namespace fs = std::filesystem;

#define SEP IO::Separator

namespace Antares::Data
{

Output::Output(const AnyString& folder):
    timestamp(0),
    mode(Data::SimulationMode::Economy),
    menuID(-1),
    viewMenuID(-1),
    outputViewerID(-1)
{
    loadFromFolder(folder);
}

bool Output::valid() const
{
    // The outputs as we know them was first introduced in Antares 3.0
    return version <= Data::StudyVersion::latest();
}

bool Output::loadFromFolder(const AnyString& folder)
{
    // reset
    menuID = -1;
    viewMenuID = -1;
    outputViewerID = -1;
    title.clear();
    name.clear();
    path.clear();
    mode = Data::SimulationMode::Unknown;

    // Load the INI file in memory
    IniFile ini;
    // The internal variable path will be use for temporary operations
    path.reserve(folder.size() + 32);
    path << folder << Yuni::SEP << "info.antares-output";

    if (not ini.open(path))
    {
        // Restoring the good value for the variable `path`
        path = folder;
        return false;
    }

    // Restoring the good value for the variable `path`
    path = folder;

    // The section
    auto* section = ini.find("general");
    const IniFile::Property* p = section->firstProperty;
    while (p)
    {
        if (p->key == "version")
        {
            version.fromString(p->value);

            // Early checks about the version
            if (version > Data::StudyVersion::latest())
            {
                logs.warning() << "Study Version greater then supported";
                return false;
            }
        }
        else
        {
            if (p->key == "title")
            {
                title = p->value;
            }
            else
            {
                if (p->key == "mode")
                {
                    StringToSimulationMode(mode, p->value);
                }
                else
                {
                    if (p->key == "timestamp")
                    {
                        timestamp = p->value.to<uint>();
                    }
                    else
                    {
                        if (p->key == "name")
                        {
                            name = p->value;
                        }
                    }
                }
            }
        }
        // Next
        p = p->next;
    }

    // Post-processing about the title
    if (not name.empty())
    {
        title << " - " << name;
    }

    return true;
}

} // namespace Antares::Data
