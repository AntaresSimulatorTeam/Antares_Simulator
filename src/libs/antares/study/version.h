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
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_LIBS_STUDY_VERSION_H__
#define __ANTARES_LIBS_STUDY_VERSION_H__

#include <yuni/yuni.h>

namespace Antares::Data
{
/*!
** \brief Version of a study
**
** \ingroup study
** \see CHANGELOG.txt
*/
enum Version
{
    //! Unknown version of a study
    versionUnknown = 0,
    //! Study version 7.0
    version700 = 700,
    //! Study version 7.1
    version710 = 710,
    //! Study version 7.2
    version720 = 720,
    //! Study version 8.0
    version800 = 800,
    //! Study version 8.1
    version810 = 810,
    //! Study version 8.2
    version820 = 820,
    //! Study version 8.3
    version830 = 830,
    //! Study version 8.4
    version840 = 840,
    //! Study version 8.5
    version850 = 850,
    //! Study version 8.6
    version860 = 860,
    //! Study version 8.7
    version870 = 870,
    //! Study version 8.8
    version880 = 880,

    // Constants
    //! A more recent version that we can't handle
    versionFutur = 99999,

};

constexpr Version versionLatest = version880;

/*!
** \brief Try to determine the version of a study
** \ingroup study
**
** \param folder The folder where data are located
** \param checkFor1x True to check for an old 1.x study
** \return The version of the study. `VersionUnknown` of not found
*/
Version StudyTryToFindTheVersion(const AnyString& folder);

/*!
** \brief Get the human readable version of the version of a study
**
** \ingroup study
** \param v The study version to convert
** \return A CString
*/
const char* VersionToCStr(const Version v);

/*!
** \brief Get the human readable version of the version of a study (wide char)
**
** \ingroup study
** \param v The study version to convert
** \return A wide string
*/
const wchar_t* VersionToWStr(const Version v);

/*!
** \brief Convert a mere integer into an enum `Version`
*/
Version VersionIntToVersion(uint version);

class StudyVersion {
public:
    [[nodiscard]] bool isStudyLatestVersion(std::string studyFolder) const;
};
} // namespace Antares::Data


#endif // __ANTARES_LIBS_STUDY_VERSION_H__
