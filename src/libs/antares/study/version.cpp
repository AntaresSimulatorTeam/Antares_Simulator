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
    versionFromCMake = (ANTARES_VERSION_HI * 100 + ANTARES_VERSION_LO * 10),
};

YUNI_STATIC_ASSERT((uint)versionFromCMake == (uint)Antares::Data::versionLatest,
                   DiscrepancyBetweenCMakeVersionAndAntaresVersion);

namespace Antares
{
namespace Data
{

static inline Version StudyFormatCheck(const String& headerFile)
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
        return ">8.6";
    case version860:
        return "8.6";
    case version850:
        return "8.5";
    case version840:
        return "8.4";
    case version830:
        return "8.3";
    case version820:
        return "8.2";
    case version810:
        return "8.1";
    case version800:
        return "8.0";

    // older versions
    case version720:
        return "7.2";
    case version710:
        return "7.1";
    case version700:
        return "7.0";

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
        return L">8.6";
    case version860:
        return L"8.6";
    case version850:
        return L"8.5";
    case version840:
        return L"8.4";
    case version830:
        return L"8.3";
    case version820:
        return L"8.2";
    case version810:
        return L"8.1";
    case version800:
        return L"8.0";

    // older versions
    case version720:
        return L"7.2";
    case version710:
        return L"7.1";
    case version700:
        return L"7.0";
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
    case 860:
        return version860;
    case 850:
        return version850;
    case 840:
        return version840;
    case 830:
        return version830;
    case 820:
        return version820;
    case 810:
        return version810;
    case 800:
        return version800;

    // older versions
    case 720:
        return version720;
    case 710:
        return version710;
    case 700:
        return version700;
    case versionFutur:
    case versionUnknown:
        return versionUnknown;
    default:
    logs.error() << "Version not found";
    logs.error() << "Studies in version <7.0 are no longer supported. Please upgrade it first"
        << " if it's the case";
    return versionUnknown;
    }
}

Version StudyTryToFindTheVersion(const AnyString& folder)
{
    if (folder.empty()) // trivial check
        return versionUnknown;

    // foldernormalization
    String abspath, directory;
    IO::MakeAbsolute(abspath, folder);
    IO::Normalize(directory, abspath);

    if (not directory.empty() and IO::Directory::Exists(directory))
    {
        abspath.reserve(directory.size() + 20);
        abspath.clear() << directory << SEP << "study.antares";
        if (IO::File::Exists(abspath))
            return StudyFormatCheck(abspath);
    }
    return versionUnknown;
}

} // namespace Data
} // namespace Antares
