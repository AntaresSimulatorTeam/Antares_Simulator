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
#include "version.h"
#include "study.h"
#include "../../../config.h" //used to get versionFromCMake

using namespace Yuni;

namespace Antares::Data
{
const std::vector<StudyVersion> StudyVersion::supportedVersions =
{
    StudyVersion(7,0),
    StudyVersion(7,1),
    StudyVersion(7,2),
    StudyVersion(8,0),
    StudyVersion(8,1),
    StudyVersion(8,2),
    StudyVersion(8,3),
    StudyVersion(8,4),
    StudyVersion(8,5),
    StudyVersion(8,6),
    StudyVersion(8,7),
    StudyVersion(8,8)
};

// Checking version between CMakeLists.txt and Antares'versions
/* static constexpr StudyVersion lastMinorCMake(ANTARES_VERSION_HI, ANTARES_VERSION_HI); */

/* static_assert(lastMinorCMake == StudyVersion::supportedVersions.back()); */

static inline StudyVersion parseLegacyVersion(const std::string& versionStr)
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

static inline StudyVersion parseCurrentVersion(const std::string& s, size_t separator)
{
    unsigned major, minor;

    if (separator == std::string::npos)
        logs.error() << "Invalid version format, exiting";

    try
    {
        major = std::stoul(s.substr(0, separator));
        minor = std::stoul(s.substr(separator + 1));
    }
    catch (std::invalid_argument&)
    {
        major = 0;
        minor = 0;
        logs.error() << "Invalid version format, exiting";
    }
    return StudyVersion(major, minor);
}

bool StudyVersion::fromString(const std::string& versionStr)
{
    // if the string doesn't contains a dot it's legacy format
    if (size_t separator = versionStr.find("."); separator == std::string::npos)
        *this = parseLegacyVersion(versionStr);
    else
        *this = parseCurrentVersion(versionStr, separator);

    if (isSupported(true))
        return true;

    *this = unknown();
    return false;
}

std::string StudyVersion::toString() const
{
    return std::to_string(major) + "." + std::to_string(minor);
}

StudyVersion::StudyVersion(unsigned major_, unsigned minor_) : major(major_), minor(minor_)
{}

StudyVersion StudyVersion::latest()
{
    return supportedVersions.back();
}

StudyVersion StudyVersion::unknown()
{
    return StudyVersion();
}

bool StudyVersion::isSupported(bool verbose) const
{
    if (std::ranges::find(supportedVersions, *this) != supportedVersions.end())
        return true;

    logs.error() << "Version: " << toString() << " not supported";

    if (*this > latest() && verbose)
    {
        logs.error() << "Maximum study version supported: " << supportedVersions.back().toString();
        logs.error() << "Please upgrade the solver to the latest version";
    }

    return false;
}

StudyVersion legacyVersionIntToVersion(unsigned version)
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
    return StudyVersion::unknown();
    }
}
} // namespace Antares::Data
