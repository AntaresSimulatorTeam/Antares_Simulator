/*** Copyright 2007-2023 RTE
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
#include <string>
#include "study.h"
#include "version.h"
#include "../../../config.h"

using namespace Yuni;

#define SEP IO::Separator

// Checking version between CMakeLists.txt and Antares'versions
static auto versionFromCMake = Antares::Data::VersionStruct(ANTARES_VERSION_HI, ANTARES_VERSION_LO);

/* static_assert(versionFromCMake == Antares::Data::versionLatest); */

namespace Antares::Data
{

static inline VersionStruct legacyStudyFormatCheck(const std::string& versionStr)
{
    unsigned versionNumber = 0;
    try
    {
        versionNumber = std::stoul(versionStr);
    }
    catch (std::invalid_argument&)
    {
        logs.error() << "Invalid version number: " << versionStr;
    }
    return legacyVersionIntToVersion(versionNumber);
}

VersionStruct VersionStruct::buildVersionLegacyOrCurrent(const std::string& versionStr)
{
    // if the string doesn't contains a dot it's legacy format
    if (versionStr.find(".") == std::string::npos)
        return legacyStudyFormatCheck(versionStr);

    if (VersionStruct::isVersionSupported(versionStr))
        return VersionStruct(versionStr);

    return VersionStruct(0, 0);
}

VersionStruct VersionStruct::studyFormatCheck(const std::string& headerFilePath)
{
    // The raw version number
    const std::string& versionStr = StudyHeader::ReadVersionFromFile(headerFilePath);
    return buildVersionLegacyOrCurrent(versionStr);
}

std::string VersionStruct::toString() const
{
    return std::to_string(major) + "." + std::to_string(minor);
}

VersionStruct::VersionStruct(const std::string& s)
{
    unsigned separator = s.find('.');
    if (separator == std::string::npos)
        logs.error() << "Invalid version format, exiting";

    try
    {
        major = std::stoul(s.substr(0, separator));
        minor = std::stoul(s.substr(separator + 1));
    }
    catch (std::invalid_argument&)
    {
        logs.error() << "Invalid version format, exiting";
    }
}

VersionStruct::VersionStruct(unsigned major_, unsigned minor_) : major(major_), minor(minor_)
{}

VersionStruct VersionStruct::versionLatest()
{
    return VersionStruct(supportedVersions.back());
}

VersionStruct VersionStruct::versionUnknown()
{
    return VersionStruct(0, 0);
}

VersionStruct StudyTryToFindTheVersion(const AnyString& folder)
{
    if (folder.empty()) // trivial check
        return VersionStruct::versionUnknown();

    // foldernormalization
    String abspath, directory;
    IO::MakeAbsolute(abspath, folder);
    IO::Normalize(directory, abspath);

    if (not directory.empty() and IO::Directory::Exists(directory))
    {
        abspath.reserve(directory.size() + 20);
        abspath.clear() << directory << SEP << "study.antares";
        if (IO::File::Exists(abspath))
            return VersionStruct::studyFormatCheck(abspath);
    }
    return VersionStruct::versionUnknown();
}

bool StudyVersion::isStudyLatestVersion(std::string studyFolder) const {
    return StudyTryToFindTheVersion(studyFolder) == VersionStruct::versionLatest();
}

bool VersionStruct::isVersionSupported(const std::string& version)
{
    auto found = std::find(supportedVersions.begin(), supportedVersions.end(), version);
    if (found != supportedVersions.end())
        return true;

    logs.error() << "Version: " << version << " not supported";

    if (VersionStruct(version) > versionLatest())
    {
        logs.error() << "Maximum study version supported: " << supportedVersions.back();
        logs.error() << "Please upgrade the solver to the latest version";
    }

    return false;
}

VersionStruct legacyVersionIntToVersion(uint version)
{
    // The list should remain ordered in the reverse order for performance reasons
    switch (version)
    {
    case 880:
        return VersionStruct(8, 8);
    case 870:
        return VersionStruct(8, 7);
    case 860:
        return VersionStruct(8, 6);
    case 850:
        return VersionStruct(8, 5);
    case 840:
        return VersionStruct(8, 4);
    case 830:
        return VersionStruct(8, 3);
    case 820:
        return VersionStruct(8, 2);
    case 810:
        return VersionStruct(8, 1);
    case 800:
        return VersionStruct(8, 0);

    // older versions
    case 720:
        return VersionStruct(7, 2);
    case 710:
        return VersionStruct(7, 1);
    case 700:
        return VersionStruct(7, 0);
    default:
        logs.error() << "Study version " << version << " is not supported by this version of "
            "antares-solver";

        logs.error() << "Studies in version <7.0 are no longer supported. Please upgrade it first"
            << " if it's the case";
    return VersionStruct(0, 0);
    }
}
} // namespace Antares::Data

