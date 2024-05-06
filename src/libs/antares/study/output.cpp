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

#include "antares/study/output.h"

#include <filesystem>

#include <yuni/io/directory/iterator.h>

using namespace Yuni;

namespace fs = std::filesystem;

#define SEP IO::Separator

namespace Antares::Data
{
namespace // anonymous
{
class OutputFolderIterator: public IO::Directory::IIterator<true>
{
public:
    using IteratorType = IO::Directory::IIterator<true>;
    using Flow = IO::Flow;

public:
    OutputFolderIterator(Data::Output::List& list):
        pList(list)
    {
    }

    virtual ~OutputFolderIterator()
    {
        // For code robustness and to avoid corrupt vtable
        stop();
    }

protected:
    virtual bool onStart(const String&)
    {
        pList.clear();
        return true;
    }

    virtual Flow onFile(const String& /*filename*/,
                        const String& parent,
                        const String& name,
                        uint64_t)
    {
        pExtension.clear();
        IO::ExtractExtension(pExtension, name);
        pExtension.toLower();

        if (pExtension == ".antares-output")
        {
            auto info = std::make_shared<Data::Output>(parent);
            if (info->valid())
            {
                pList.push_back(info);
            }
            return IO::flowSkip;
        }
        return IO::flowContinue;
    }

private:
    CString<20, false> pExtension;
    Output::List& pList;

}; // class OutputFolderIterator

} // anonymous namespace

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
    path << folder << SEP << "info.antares-output";

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

void Output::RetrieveListFromStudy(List& out, const Study& study)
{
    out.clear();

    if (not study.folder.empty())
    {
        fs::path folder = fs::path(study.folder.c_str()) / "output";

        if (fs::exists(folder))
        {
            OutputFolderIterator iterator(out);
            iterator.add(folder.string());
            iterator.start();
            iterator.wait(15000); // 15s - arbitrary
        }
        else
        {
            // No output at all, it is quite useless to start a new thread
            // for iterating into a non-existing folder
        }
    }
    else
    {
        // The variable folder is empty, meaning that the study is still
        // in memory and does not come from the disk
    }
}

} // namespace Antares::Data
