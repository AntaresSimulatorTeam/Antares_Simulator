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
#include <yuni/core/string.h>
#include "study.h"
#include "version.h"
#include "../../../config.h" //used to get versionFromCMake

using namespace Yuni;

#define SEP IO::Separator

// Checking version between CMakeLists.txt and Antares'versions
static auto versionFromCMake = Antares::Data::StudyVersion(ANTARES_VERSION_HI, ANTARES_VERSION_LO);

/* static_assert(versionFromCMake == Antares::Data::versionLatest); */

namespace Antares::Data
{

const std::vector<std::string> StudyVersion::supportedVersions =
{
    "7.0",
    "7.1",
    "7.2",
    "8.0",
    "8.1",
    "8.2",
    "8.3",
    "8.4",
    "8.5",
    "8.6",
    "8.7",
    "8.8"
};

static inline StudyVersion legacyStudyFormatCheck(const std::string& versionStr)
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

StudyVersion StudyVersion::buildVersionLegacyOrCurrent(const std::string& versionStr)
{
    // if the string doesn't contains a dot it's legacy format
    if (versionStr.find(".") == std::string::npos)
        return legacyStudyFormatCheck(versionStr);

    if (StudyVersion::isVersionSupported(versionStr))
        return StudyVersion(versionStr);

    return StudyVersion(0, 0);
}

StudyVersion StudyVersion::studyFormatCheck(const std::string& headerFilePath)
{
    // The raw version number
    const std::string& versionStr = StudyHeader::ReadVersionFromFile(headerFilePath);
    return buildVersionLegacyOrCurrent(versionStr);
}

std::string StudyVersion::toString() const
{
    return std::to_string(major) + "." + std::to_string(minor);
}

StudyVersion::StudyVersion(const std::string& s)
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

StudyVersion::StudyVersion(unsigned major_, unsigned minor_) : major(major_), minor(minor_)
{}

StudyVersion StudyVersion::versionLatest()
{
    return StudyVersion(supportedVersions.back());
}

StudyVersion StudyVersion::versionUnknown()
{
    return StudyVersion(0, 0);
}

StudyVersion StudyTryToFindTheVersion(const AnyString& folder)
{
    if (folder.empty()) // trivial check
        return StudyVersion::versionUnknown();

    // foldernormalization
    String abspath, directory;
    IO::MakeAbsolute(abspath, folder);
    IO::Normalize(directory, abspath);

    if (not directory.empty() and IO::Directory::Exists(directory))
    {
        abspath.reserve(directory.size() + 20);
        abspath.clear() << directory << SEP << "study.antares";
        if (IO::File::Exists(abspath))
            return StudyVersion::studyFormatCheck(abspath);
    }
    return StudyVersion::versionUnknown();
}

bool StudyVersion::isVersionSupported(const std::string& version)
{
    auto found = std::find(supportedVersions.begin(), supportedVersions.end(), version);
    if (found != supportedVersions.end())
        return true;

    logs.error() << "Version: " << version << " not supported";

    if (StudyVersion(version) > versionLatest())
    {
        logs.error() << "Maximum study version supported: " << supportedVersions.back();
        logs.error() << "Please upgrade the solver to the latest version";
    }

    return false;
}

StudyVersion legacyVersionIntToVersion(uint version)
{
    // The list should remain ordered in the reverse order for performance reasons
    switch (version)
    {
    case 880:
        return StudyVersion(8, 8);
    case 870:
        return StudyVersion(8, 7);
    case 860:
        return StudyVersion(8, 6);
    case 850:
        return StudyVersion(8, 5);
    case 840:
        return StudyVersion(8, 4);
    case 830:
        return StudyVersion(8, 3);
    case 820:
        return StudyVersion(8, 2);
    case 810:
        return StudyVersion(8, 1);
    case 800:
        return StudyVersion(8, 0);

    // older versions
    case 720:
        return StudyVersion(7, 2);
    case 710:
        return StudyVersion(7, 1);
    case 700:
        return StudyVersion(7, 0);
    default:
        logs.error() << "Study version " << version << " is not supported by this version of "
            "antares-solver";

        logs.error() << "Studies in version <7.0 are no longer supported. Please upgrade it first"
            << " if it's the case";
    return StudyVersion(0, 0);
    }
}
} // namespace Antares::Data
