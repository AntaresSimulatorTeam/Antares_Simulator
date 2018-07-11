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
#ifndef __STUDY_RESULT_AGGREGATOR_RESULT_H__
# define __STUDY_RESULT_AGGREGATOR_RESULT_H__

# include <yuni/yuni.h>
# include <yuni/core/string.h>
# include <map>
# include <antares/memory/memory.h>
# include "datafile.h"
# include "studydata.h"


using namespace Yuni;

enum
{
	maxSizePerCell = 42,
};

//! A single cell
typedef char  CellData[maxSizePerCell];


//! A single column
class CellColumnData final
{
public:
	//! \name Constructors & Destructor
	//@{
	/*!
	** \brief Default constructor
	*/
	CellColumnData();
	//! Copy constructor
	CellColumnData(const CellColumnData&);
	//! Destructor
	~CellColumnData();
	//@}

public:
	//! All rows
	Antares::Memory::Stored<CellData>::Type   rows;
	//! The height of the column
	uint height;

}; // class CellColumnData



class ResultMatrix final
{
public:
	//! \name Constructor & Destructor
	//@{
	/*!
	** \brief Default constructor
	*/
	ResultMatrix();
	//! Copy constructor
	ResultMatrix(const ResultMatrix&);
	//! Destructor
	~ResultMatrix();
	//@}

	/*!
	** \brief Resize the pseudo matrix
	*/
	void resize(uint i);

	/*!
	** \brief Export the content of the matrix into a CSV file
	*/
	bool saveToCSVFile(const Yuni::String& filename) const;


public:
	CellColumnData* columns;
	//! Width of the matrix
	uint width;
	//! Valid Height found after aggregation
	uint heightAfterAggregation;

}; // class ResultMatrix




typedef std::vector<ResultMatrix>  ResultsAllVars;

typedef std::map<DataFile::ShortString, ResultsAllVars>  ResultsForAllTimeLevels;
typedef std::map<DataFile::ShortString, ResultsForAllTimeLevels>  ResultsForAllDataLevels;

typedef std::map<StudyData::ShortString512, ResultsForAllDataLevels>  ResultsForAllStudyItems;




#endif // __STUDY_RESULT_AGGREGATOR_RESULT_H__
