/*
** Copyright 2007-2018 RTE
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

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include "study.h"
#include "version.h"
#include "../../../config.h"

using namespace Yuni;

#define SEP IO::Separator

// Checking version between CMakeLists.txt and Antares'versions
enum
{
    versionFromCMake = (ANTARES_VERSION_HI * 100 + ANTARES_VERSION_LO * 10 + ANTARES_VERSION_BUILD),
};

YUNI_STATIC_ASSERT((uint)versionFromCMake == (uint)Antares::Data::versionLatest,
                   DiscrepancyBetweenCMakeVersionAndAntaresVersion);

namespace Antares
{
namespace Data
{
static Version StudyFormatCheckForThe1xFormat(String& buffer, const String& folder)
{
    // Checking for the main folders...
    buffer.clear() << folder << SEP << "INPUT";
    if (not IO::Directory::Exists(buffer))
        return versionUnknown;
    buffer.clear() << folder << SEP << "SETTINGS";
    if (not IO::Directory::Exists(buffer))
        return versionUnknown;

    // Checking for a few files...
    buffer.clear() << folder << SEP << "SETTINGS" << SEP << "donnees_generales_prepro.txt";
    if (not IO::File::Exists(buffer))
        return versionUnknown;
    buffer.clear() << folder << SEP << "SETTINGS" << SEP << "donnees_generales_simu.txt";
    if (not IO::File::Exists(buffer))
        return versionUnknown;

    // Seems to be a 1.x format
    return version1xx;
}

static inline Version StudyFormatCheckForThe2xFormat(const String& headerFile)
{
    // The raw version number
    uint version = StudyHeader::ReadVersionFromFile(headerFile);
    // Its equivalent
    Version venum = VersionIntToVersion(version);

    switch (venum)
    {
    // Dealing with special values
    case versionUnknown:
    case versionFutur:
    case version1xx:
    {
        return (version and (uint) version > (uint)versionLatest) ? versionFutur : versionUnknown;
    }
    default:
        return venum;
    }
}

const char* VersionToCStr(const Version v)
{
    // The list should remain ordered in the reverse order for performance reasons
    switch (v)
    {
    case versionFutur:
        return ">8.0";
    case version800:
        return "8.0";
    case version720:
        return "7.2";
    case version710:
        return "7.1";
    case version700:
        return "7.0";

    // older versions
    case version650:
        return "6.5";
    case version640:
        return "6.4";
    case version630:
        return "6.3";
    case version620:
        return "6.2";
    case version610:
        return "6.1";
    case version600:
        return "6.0";
    case version510:
        return "5.1";
    case version500:
        return "5.0";
    case version450:
        return "4.5";
    case version440:
        return "4.4";
    case version430:
        return "4.3";
    case version420:
        return "4.2";
    case version410:
        return "4.1";
    case version400:
        return "4.0";
    case version390:
        return "3.9";
    case version380:
        return "3.8";
    case version370:
        return "3.7";
    case version360:
        return "3.6";
    case version350:
        return "3.5";
    case version340:
        return "3.4";
    case version330:
        return "3.3";
    case version320:
        return "3.2";
    case version310:
        return "3.1";
    case version300:
        return "3.0";
    case version210:
        return "2.1";
    case version200:
        return "2.0";
    case version1xx:
        return "1.0";
    case versionUnknown:
        return "0";
    }
    return "0.0";
}

const wchar_t* VersionToWStr(const Version v)
{
    // The list should remain ordered in the reverse order for performance reasons
    switch (v)
    {
    case versionFutur:
        return L">8.0";
    case version800:
        return L"8.0";
    case version720:
        return L"7.2";
    case version710:
        return L"7.1";
    case version700:
        return L"7.0";

    // older versions
    case version650:
        return L"6.5";
    case version640:
        return L"6.4";
    case version630:
        return L"6.3";
    case version620:
        return L"6.2";
    case version610:
        return L"6.1";
    case version600:
        return L"6.0";
    case version500:
        return L"5.0";
    case version510:
        return L"5.1";
    case version450:
        return L"4.5";
    case version440:
        return L"4.4";
    case version430:
        return L"4.3";
    case version420:
        return L"4.2";
    case version410:
        return L"4.1";
    case version400:
        return L"4.0";
    case version390:
        return L"3.9";
    case version380:
        return L"3.8";
    case version370:
        return L"3.7";
    case version360:
        return L"3.6";
    case version350:
        return L"3.5";
    case version340:
        return L"3.4";
    case version330:
        return L"3.3";
    case version320:
        return L"3.2";
    case version310:
        return L"3.1";
    case version300:
        return L"3.0";
    case version210:
        return L"2.1";
    case version200:
        return L"2.0";
    case version1xx:
        return L"1.0";
    case versionUnknown:
        return L"0";
    }
    return L"0.0";
}

Version VersionIntToVersion(uint version)
{
    // The list should remain ordered in the reverse order for performance reasons
    switch (version)
    {
    case 800:
        return version800;
    case 720:
        return version720;
    case 710:
        return version710;
    case 700:
        return version700;
    case 650:
        return version650;
    case 640:
        return version640;
    case 630:
        return version630;
    case 620:
        return version620;
    case 610:
        return version610;
    case 600:
        return version600;
    case 510:
        return version510;
    case 500:
        return version500;
    case 450:
        return version450;
    case 440:
        return version440;
    case 430:
        return version430;
    case 420:
        return version420;
    case 410:
        return version410;
    case 400:
        return version400;
    case 390:
        return version390;
    case 380:
        return version380;
    case 370:
        return version370;
    case 360:
        return version360;
    case 350:
        return version350;
    case 340:
        return version340;
    case 330:
        return version330;
    case 320:
        return version320;
    case 310:
        return version310;
    case 300:
        return version300;
    case 210:
        return version210;
    case 200:
        return version200;
    case 100:
        return version1xx;
    case versionFutur:
    case versionUnknown:
        return versionUnknown;
    }
#ifndef NDEBUG
    if (version > 100 and version <= (uint)versionLatest)
    {
        assert(false and "missing switch entry");
    }
#endif
    return versionUnknown;
}

Version StudyTryToFindTheVersion(const AnyString& folder, bool checkFor1x)
{
    if (folder.empty()) // trivial check
        return versionUnknown;

    // foldernormalization
    String abspath, directory;
    IO::MakeAbsolute(abspath, folder);
    IO::Normalize(directory, abspath);

    if (not directory.empty() and IO::Directory::Exists(directory))
    {
        // Since antares 2.x, any study folder contain a "study.antares" file.
        // if it is not the case, it might be an old 1.x, but most of the time
        // it is nothing for Antares.
        abspath.reserve(directory.size() + 20);
        abspath.clear() << directory << SEP << "study.antares";
        if (IO::File::Exists(abspath))
        {
            // Should be a 2.x version
            return StudyFormatCheckForThe2xFormat(abspath);
        }
        // It may be a very old fashion 1.x version...
        if (checkFor1x)
            return StudyFormatCheckForThe1xFormat(abspath, directory);
    }
    return versionUnknown;
}

} // namespace Data
} // namespace Antares
