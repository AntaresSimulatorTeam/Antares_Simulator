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

#include "antares/study/version.h"

#include <antares/config/config.h>
#include "antares/study/study.h"

using namespace Yuni;
using namespace Antares::Data;

namespace
{
constexpr auto supportedVersions = std::to_array({
  StudyVersion(7, 0),
  StudyVersion(7, 1),
  StudyVersion(7, 2),
  StudyVersion(8, 0),
  StudyVersion(8, 1),
  StudyVersion(8, 2),
  StudyVersion(8, 3),
  StudyVersion(8, 4),
  StudyVersion(8, 5),
  StudyVersion(8, 6),
  StudyVersion(8, 7),
  StudyVersion(8, 8),
  StudyVersion(9, 0),
  StudyVersion(9, 1),
  StudyVersion(9, 2)
  // Add new versions here
});

/// Convert a unsigned into a StudyVersion, used for legacy version format (ex: 720)
StudyVersion legacyVersionIntToVersion(unsigned version)
{
    // It's not necessary to add anything here, since legacy versions should not be created

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
        logs.error() << "Study version " << version
                     << " is not supported by this version of "
                        "antares-solver";

        logs.error() << "Studies in version <7.0 are no longer supported. Please upgrade it first"
                     << " if it's the case";
        return StudyVersion::unknown();
    }
}

StudyVersion parseLegacyVersion(const std::string& versionStr)
{
    unsigned versionNumber = 0;
    try
    {
        versionNumber = std::stoul(versionStr);
    }
    catch (std::invalid_argument&)
    {
        logs.error() << "Invalid version number: " << versionStr;
        return StudyVersion::unknown();
    }
    return ::legacyVersionIntToVersion(versionNumber);
}

StudyVersion parseCurrentVersion(const std::string& s, size_t separator)
{
    unsigned major, minor;

    if (separator == std::string::npos)
    {
        logs.error() << "Invalid version format, exiting";
    }

    try
    {
        major = std::stoul(s.substr(0, separator));
        minor = std::stoul(s.substr(separator + 1));
    }
    catch (std::invalid_argument&)
    {
        logs.error() << "Invalid version format, exiting";
        return StudyVersion::unknown();
    }
    return StudyVersion(major, minor);
}

} // namespace

// Checking version between CMakeLists.txt and Antares'versions
static_assert(StudyVersion(ANTARES_VERSION_HI, ANTARES_VERSION_LO) == ::supportedVersions.back(),
              "Please check that CMake's version and version.cpp's version match");

namespace Antares::Data
{
bool StudyVersion::fromString(const std::string& versionStr)
{
    // if the string doesn't contains a dot it's legacy format
    if (size_t separator = versionStr.find("."); separator == std::string::npos)
    {
        *this = parseLegacyVersion(versionStr);
    }
    else
    {
        *this = parseCurrentVersion(versionStr, separator);
    }

    if (isSupported(true))
    {
        return true;
    }

    *this = unknown();
    return false;
}

std::string StudyVersion::toString() const
{
    return std::to_string(major_) + "." + std::to_string(minor_);
}

StudyVersion StudyVersion::latest()
{
    return ::supportedVersions.back();
}

StudyVersion StudyVersion::unknown()
{
    return StudyVersion();
}

bool StudyVersion::isSupported(bool verbose) const
{
    if (std::ranges::find(::supportedVersions, *this) != ::supportedVersions.end())
    {
        return true;
    }

    if (*this > latest() && verbose)
    {
        logs.error() << "Maximum study version supported: "
                     << ::supportedVersions.back().toString();
        logs.error() << "Please upgrade the solver to the latest version";
    }

    return false;
}
} // namespace Antares::Data
