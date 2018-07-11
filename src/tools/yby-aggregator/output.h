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
#ifndef __STUDY_OUTPUT_AGGREGATOR_OUTPUT_H__
# define __STUDY_OUTPUT_AGGREGATOR_OUTPUT_H__

# include <yuni/yuni.h>
# include <yuni/core/string.h>
# include <set>
# include "result.h"


class Output final
{
public:
	//! The most suitable smart pointer
	typedef Yuni::SmartPtr<Output>  Ptr;
	//! A folder name (short length)
	typedef Yuni::CString<10, false>  FolderName;
	//! Column name
	typedef Yuni::CString<128, false> ColumnName;
	//! Vector
	typedef std::vector<Ptr> Vector;

public:
	Output(const YString& target, const YString::Vector& cols) :
		path(target),
		columns(cols)
	{
	}


	bool canContinue() const
	{
		return (errors < 100);
	}

	void incrementError();


public:
	//! The minimum value for the years
	uint minYear;
	//! The maximum value for the years
	uint maxYear;
	//! The total number of years
	uint nbYears;
	//! The study output directory
	const Yuni::String path;
	//! All columns to extract
	const Yuni::String::Vector columns;
	//! The number of errors
	Yuni::Atomic::Int<>  errors;

	//! Results
	ResultsForAllStudyItems  results;

}; // class Output




#endif // __STUDY_OUTPUT_AGGREGATOR_OUTPUT_H__
