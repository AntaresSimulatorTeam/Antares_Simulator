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
#ifndef __ANTARES_LIBS_STUDY_HEADER_H__
# define __ANTARES_LIBS_STUDY_HEADER_H__

# include <yuni/yuni.h>
# include <yuni/core/string.h>
# include <time.h>
# include "../inifile.h"


//! Default author
# define STUDYHEADER_DEFAULT_AUTHOR      "Unknown"
//! Default caption
# define STUDYHEADER_DEFAULT_CAPTION     "No title"



namespace Antares
{
namespace Data
{


	/*!
	** \brief Header of a study
	** \ingroup study
	*/
	class StudyHeader final
	{
	public:
		/*!
		** \brief Get the version of a header
		**
		** \param filename The filename to read
		** \return The version of the study, or 0 if unknown (invalid header)
		*/
		static uint ReadVersionFromFile(const AnyString& filename);


	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		StudyHeader();
		/*!
		** \brief Destructor
		*/
		~StudyHeader();
		//@}

		/*!
		** \brief Reset the values
		*/
		void reset();

		/*!
		** \brief Load a study header from a file
		**
		** \param filename The target filename
		** \param warnings Enable warnings/errors
		** \return True if the operation succeeded, false otherwise
		*/
		bool loadFromFile(const AnyString& filename, bool warnings = true);

		/*!
		** \brief Save a study header into a file
		**
		** The property `version` will be upgraded to the last available
		** and `dateLastSave` as well.
		**
		** \param filename The target filename
		** \return True if the operation succeeded, false otherwise
		*/
		bool saveToFile(const AnyString& filename, bool upgradeVersion = true);

	public:
		//! Caption of the study
		Yuni::String caption;

		//! Format version
		uint version;

		//! Date: Creation (timestamp)
		time_t dateCreated;
		//! Date: Last save (timestamp)
		time_t dateLastSave;

		//! Author
		Yuni::String author;

	private:
		//! Load settings from an INI file
		bool internalLoadFromINIFile(const IniFile& ini, bool warnings);

		//! Get the version written in an header file
		static uint internalFindVersionFromFile(const IniFile& ini);

		//! Copy the internal settings into an INI structure
		void internalCopySettingsToIni(IniFile& ini, bool upgradeVersion);

	}; // class StudyHeader;






} // namespace Data
} // namespace Antares

# include "header.hxx"

#endif /* __ANTARES_LIBS_STUDY_HEADER_H__ */
