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
#ifndef __STUDY_RESULT_AGGREGATOR_RESULT_H__
#define __STUDY_RESULT_AGGREGATOR_RESULT_H__

#include <map>

#include <yuni/yuni.h>
#include <yuni/core/string.h>

#include <antares/memory/memory.h>
#include "antares/solver/ts-generator/xcast/studydata.h"

#include "datafile.h"
#include "studydata.h"

using namespace Yuni;

enum
{
    maxSizePerCell = 42,
};

//! A single cell
using CellData = char[maxSizePerCell];

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
    Antares::Memory::Stored<CellData>::Type rows;
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

using ResultsAllVars = std::vector<ResultMatrix>;

using ResultsForAllTimeLevels = std::map<DataFile::ShortString, ResultsAllVars>;
using ResultsForAllDataLevels = std::map<DataFile::ShortString, ResultsForAllTimeLevels>;

using ResultsForAllStudyItems = std::map<StudyData::ShortString512, ResultsForAllDataLevels>;

#endif // __STUDY_RESULT_AGGREGATOR_RESULT_H__
