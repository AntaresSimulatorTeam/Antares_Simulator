/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "header.h"
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <antares/logs/logs.h>
#include "version.h"

using namespace Yuni;

#define SEP IO::Separator

namespace Antares::Data
{
// TODO this method should be removed - use Yuni::String instead
static inline int ConvertCStrToTimeT(const char* s, time_t* var)
{
    assert(s);
    assert(var);
    char* pend;
    *var = (time_t)strtoul(s, &pend, 10);
    return (pend && '\0' == *pend) ? 1 : 0;
}

void StudyHeader::reset()
{
    // Caption
    caption = STUDYHEADER_DEFAULT_CAPTION;
    // Version
    version = Data::StudyVersion::latest();
    // Date
    dateCreated = ::time(nullptr);
    dateLastSave = dateCreated;
    // Author
    author = STUDYHEADER_DEFAULT_AUTHOR;
}

void StudyHeader::CopySettingsToIni(IniFile& ini, bool upgradeVersion)
{
    // New section
    IniFile::Section* sect = ini.addSection("antares");

    // Version
    // For performance reasons, the version should be in written first to
    // be able to quickly check the version of the study when calling
    // StudyHeader::ReadVersionFromFile().
    if (upgradeVersion)
        version = Data::StudyVersion::latest();
    sect->add("version", Data::StudyVersion::latest().toString());

    // Caption
    sect->add("caption", caption);

    // Date
    sect->add("created", (int64_t)dateCreated);
    // Last save
    dateLastSave = ::time(nullptr);
    sect->add("lastSave", (int64_t)dateLastSave);

    // The author
    sect->add("author", author);
}

bool StudyHeader::internalFindVersionFromFile(const IniFile& ini, std::string& version)
{
    const IniFile::Section* sect = ini.find("antares");
    if (sect)
    {
        for (const IniFile::Property* p = sect->firstProperty; p; p = p->next)
        {
            // Version
            if (p->key == "version")
            {
                version = p->value;
                return true;
            }
        }
    }

    logs.error() << "Couldn't find a version number in study.antares";
    return false;
}

bool StudyHeader::internalLoadFromINIFile(const IniFile& ini, bool warnings)
{
    const IniFile::Section* sect = ini.find("antares");
    if (sect)
    {
        for (const IniFile::Property* p = sect->firstProperty; p; p = p->next)
        {
            // Caption
            if (p->key == "caption")
            {
                caption = p->value;
                continue;
            }
            // Author
            if (p->key == "author")
            {
                author = p->value;
                continue;
            }
            // Version
            if (p->key == "version")
            {
                version.fromString(p->value);
                continue;
            }

            // Date : Creation
            if (p->key == "created")
            {
                if (ConvertCStrToTimeT(p->value.c_str(), &dateCreated) && !dateCreated)
                {
                    if (warnings)
                        logs.error() << "Study header: The date of creation is invalid";
                    dateCreated = ::time(nullptr);
                }
                continue;
            }
            if (p->key == "lastsave")
            {
                if (ConvertCStrToTimeT(p->value.c_str(), &dateLastSave) && !dateLastSave)
                {
                    if (warnings)
                        logs.error() << "Study header: The date of the last save is invalid";
                    dateLastSave = ::time(nullptr);
                }
                continue;
            }
        }
    }
    else
    {
        if (warnings)
            logs.error() << "The main section has not been found. The study seems invalid.";
    }

    if (version >= StudyVersion(7, 0))
    {
        if (version > Data::StudyVersion::latest())
        {
            if (warnings)
            {
                logs.error() << "Header: This version is not supported (version found:"
                             << version.toString() << ", expected: <="
                             << Data::StudyVersion::latest().toString() << ')';
            }
            return false;
        }
        return true;
    }

    if (warnings)
        logs.error() << "Study header: Invalid format";
    return false;
}

bool StudyHeader::loadFromFile(const AnyString& filename, bool warnings)
{
    // (Re)Initialize the internal settings
    reset();

    // Loading the INI file
    IniFile ini;
    return (ini.open(filename, warnings)) ? internalLoadFromINIFile(ini, warnings) : false;
}

bool StudyHeader::saveToFile(const AnyString& filename, bool upgradeVersion)
{
    IniFile ini;
    CopySettingsToIni(ini, upgradeVersion);
    return ini.save(filename);
}

StudyVersion StudyHeader::tryToFindTheVersion(const AnyString& folder)
{
    if (folder.empty()) // trivial check
        return StudyVersion::unknown();

    // foldernormalization
    String abspath, directory;
    IO::MakeAbsolute(abspath, folder);
    IO::Normalize(directory, abspath);

    if (not directory.empty() and IO::Directory::Exists(directory))
    {
        abspath.reserve(directory.size() + 20);
        abspath.clear() << directory << SEP << "study.antares";
        if (IO::File::Exists(abspath))
        {
            // The raw version number
            std::string versionStr;
            if (!ReadVersionFromFile(abspath, versionStr))
                return StudyVersion::unknown();

            StudyVersion v;
            v.fromString(versionStr);
            return v;
        }
    }
    return StudyVersion::unknown();
}

bool StudyHeader::ReadVersionFromFile(const AnyString& filename, std::string& version)
{
    IniFile ini;
    if (ini.open(filename))
        return internalFindVersionFromFile(ini, version);

    logs.error() << "Couldn't open study.antares to find the version number";
    return false;
}

} // namespace Antares::Data

