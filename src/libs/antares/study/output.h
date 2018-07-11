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
#ifndef __ANTARES_LIBS_STUDY_OUTPUT_H__
# define __ANTARES_LIBS_STUDY_OUTPUT_H__

# include <yuni/yuni.h>
# include <yuni/core/string.h>
# include <map>
# include <functional>
# include "study.h"


namespace Antares
{
namespace Data
{


	class Output final
	{
	public:
		//! Smartptr
		typedef Yuni::SmartPtr<Output> Ptr;
		//! List
		typedef std::list<Ptr> List;
		//! Title
		typedef Yuni::CString<128,false>   Title;
		//! Name
		typedef Yuni::CString<128,false>   Name;

		//! Map of study output info, ordered by their timestamp
		typedef std::map<Yuni::sint64, Ptr>      MapByTimestamp;
		//! Map of study output info, ordered by their timestamp (desc)
		typedef std::map<Yuni::sint64, Ptr, std::greater<Yuni::sint64> >  MapByTimestampDesc;


	public:
		/*!
		** \brief Retrieve the list of all available outputs
		*/
		static void RetrieveListFromStudy(List& out, const Study& study);
		/*!
		** \brief Retrieve the list of all available outputs
		*/
		static void RetrieveListFromStudy(List& out, const Study::Ptr& study);


	public:
		//! \name Constructor
		//@{
		/*!
		** \brief Constructor, with an output folder
		*/
		explicit Output(const AnyString& folder);
		//@}

		/*!
		** \brief Load data from an output folder
		*/
		bool loadFromFolder(const AnyString& folder);

		//! Get if the output folder previously loaded is valid
		bool valid() const;


		//! Operator <
		bool operator < (const Output& rhs) const;


	public:
		//! Version of the solver used for the simulation
		uint version;
		//! Date/time when the simulation has been launched
		yint64 timestamp;
		//! Mode (economy/adequacy/other)
		Data::StudyMode  mode;
		//! Tag for storing a wx menu id
		int menuID;
		//! Tag for storing a wx menu id (from the menu `output`)
		int viewMenuID;
		//! Tag for storing a wx menu id (for the output viewer)
		int outputViewerID;

		//! Title to use for visual representation of the output
		Title title;
		//! Name
		Name  name;
		//! Output folder
		YString path;

	}; // class Output






} // namespace Data
} // namespace Antares

# include "output.hxx"

#endif // __ANTARES_STUDY_LIBS_OUTPUT_H__
