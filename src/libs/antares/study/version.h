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
#ifndef __ANTARES_LIBS_STUDY_VERSION_H__
# define __ANTARES_LIBS_STUDY_VERSION_H__

# include <yuni/yuni.h>


namespace Antares
{
namespace Data
{

	/*!
	** \brief Version of a study
	**
	** \ingroup study
	** \see CHANGELOG.txt
	*/
	typedef enum // Version
	{
		//! Unknown version of a study
		versionUnknown = 0,
		//! The old 1.x Study version
		version1xx = 100,
		//! Study version: 2.0
		version200 = 200,
		//! Study Version 2.1
		version210 = 210,

		//! Study version: 3.0
		version300 = 300,
		//! Study version 3.1
		version310 = 310,
		//! Study version 3.2
		version320 = 320,
		//!Study version 3.3
		version330 = 330,
		//! Study version 3.4
		version340 = 340,
		//! Study version 3.5
		version350 = 350,
		//! Study version 3.6
		version360 = 360,
		//! Study version 3.7
		version370 = 370,
		//! Study version 3.8
		version380 = 380,
		//! Study version 3.9
		version390 = 390,
		//! Study version 4.0
		version400 = 400,
		//! Study version 4.1
		version410 = 410,
		//! Study version 4.2
		version420 = 420,
		//! Study version 4.3
		version430 = 430,
		//! Study version 4.4
		version440 = 440,
		//! Study version 4.5
		version450 = 450,
		//! Study version 5.0
		version500 = 500,
		//! Study version 5.1
		version510 = 510,
		//! Study version 6.0
		version600 = 600,
		//! Study version 6.1
		version610 = 610,
		//! Study version 6.2
		version620 = 620,
		//! Study version 6.3
		version630 = 630,
		//! Study version 6.4
		version640 = 640,
		//! Study version 6.5
		version650 = 650,
		//! Study version 6.5
		version700 = 700,

		// see versionLatest below

		// Constants
		//! A more recent version that we can't handle
		versionFutur = 99999,

	}  Version;

	enum
	{
		//! The latest version
		versionLatest = version700,
	};




	/*!
	** \brief Try to determine the version of a study
	** \ingroup study
	**
	** \param folder The folder where data are located
	** \param checkFor1x True to check for an old 1.x study
	** \return The version of the study. `VersionUnknown` of not found
	*/
	Version StudyTryToFindTheVersion(const AnyString& folder, bool checkFor1x = true);





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




} // namespace Data
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_VERSION_H__
