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

#include "antares/study/header.h"

#include <cassert>
#include <cstdlib>
#include <ctime>

#include <antares/logs/logs.h>
#include "antares/study/version.h"

using namespace Yuni;

namespace fs = std::filesystem;

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
    // StudyHeader::readVersionFromFile().
    if (upgradeVersion)
    {
        version = Data::StudyVersion::latest();
    }
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
                    {
                        logs.error() << "Study header: The date of creation is invalid";
                    }
                    dateCreated = ::time(nullptr);
                }
                continue;
            }
            if (p->key == "lastsave")
            {
                if (ConvertCStrToTimeT(p->value.c_str(), &dateLastSave) && !dateLastSave)
                {
                    if (warnings)
                    {
                        logs.error() << "Study header: The date of the last save is invalid";
                    }
                    dateLastSave = ::time(nullptr);
                }
                continue;
            }
        }
    }
    else
    {
        if (warnings)
        {
            logs.error() << "The main section has not been found. The study seems invalid.";
        }
    }

    if (version.isSupported(true))
    {
        return true;
    }

    if (warnings)
    {
        logs.error() << "Study header: Invalid format";
    }

    return false;
}

bool StudyHeader::loadFromFile(const fs::path& filename, bool warnings)
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

StudyVersion StudyHeader::tryToFindTheVersion(const std::string& folder)
{
    if (folder.empty()) // trivial check
    {
        return StudyVersion::unknown();
    }

    // folder normalization
    fs::path abspath = fs::absolute(folder);
    abspath = abspath.lexically_normal();

    if (fs::exists(abspath))
    {
        abspath /= "study.antares";
        if (fs::exists(abspath))
        {
            // The raw version number
            std::string versionStr;
            if (!readVersionFromFile(abspath, versionStr))
            {
                return StudyVersion::unknown();
            }

            StudyVersion v;
            v.fromString(versionStr);
            return v;
        }
    }
    return StudyVersion::unknown();
}

bool StudyHeader::readVersionFromFile(const fs::path& filename, std::string& version)
{
    IniFile ini;
    if (ini.open(filename))
    {
        return internalFindVersionFromFile(ini, version);
    }

    logs.error() << "Couldn't open study.antares to find the version number";
    return false;
}

} // namespace Antares::Data
