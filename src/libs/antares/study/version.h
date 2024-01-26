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
#pragma once

#include <string>
#include <map>

namespace Antares::Data
{

/*!
** \brief Version of a study
**
** \ingroup study
** \see CHANGELOG.txt
*/

class StudyVersion
{
public:
    auto operator<=>(const StudyVersion&) const = default;

    StudyVersion() = default;
    StudyVersion(const std::string&);
    StudyVersion(unsigned, unsigned);
    ~StudyVersion() = default;

    std::string toString() const;

    static StudyVersion studyFormatCheck(const std::string& headerFilePath);
    static StudyVersion buildVersionLegacyOrCurrent(const std::string& versionStr);

    static StudyVersion versionLatest();
    static StudyVersion versionUnknown();

private:
    unsigned major = 0;
    unsigned minor = 0;

    static bool isVersionSupported(const std::string& version);
    static const std::vector<std::string> supportedVersions;
};

/*!
** \brief Try to determine the version of a study
** \ingroup study
**
** \param folder The folder where data are located
** \param checkFor1x True to check for an old 1.x study
** \return The version of the study. `VersionUnknown` of not found
*/
StudyVersion StudyTryToFindTheVersion(const AnyString& folder);

/*!
** \brief Convert a mere integer into an enum `Version`
*/
StudyVersion legacyVersionIntToVersion(uint version);

} // namespace Antares::Data
