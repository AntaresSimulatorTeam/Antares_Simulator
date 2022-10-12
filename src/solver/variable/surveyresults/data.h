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
#ifndef __SOLVER_VARIABLE_SURVEY_RESULTS_DATA_H__
#define __SOLVER_VARIABLE_SURVEY_RESULTS_DATA_H__

#include <yuni/yuni.h>
#include "../constants.h"
#include <yuni/core/string.h>
#include <antares/study.h>
#include <i_writer.h>

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Private
{
class SurveyResultsData
{
public:
    //! \name Constructor
    //@{
    SurveyResultsData(const Data::Study& s, const Yuni::String& o);
    //@}

    void initialize(unsigned int maxVariables);

    /*!
    ** \brief Export informations about the current study
    **
    ** It is composed by several files to completely describe the system
    ** and provide a good support for Excel macros.
    */
    void exportGridInfos(IResultWriter::Ptr writer);

    void exportGridInfosAreas(IResultWriter::Ptr writer);

public:
    //! The current column index
    unsigned int columnIndex;

    // Output variable non applicable :
    //	Column indices related to non applicable output variables
    //	Useful to print "N/A" in output files for these variables
    std::vector<unsigned int> nonApplicableColIdx;

    //! Current thermal cluster
    const Data::ThermalCluster* thermalCluster;
    //! Current area
    const Data::Area* area;
    //! Current link
    const Data::AreaLink* link;
    //! The index for the current set of areas
    unsigned int setOfAreasIndex;

    //! The current study
    const Data::Study& study;
    //! The number of MC years
    unsigned int nbYears;
    //! The effective number of MC years (user's playlist)
    unsigned int effectiveNbYears;

    //! The folder output where to write the results
    Yuni::String output;
    //! The filename to use for the current report
    Yuni::String filename;
    //!
    const Yuni::String& originalOutput;

    //! Captions for rows
    Yuni::String::Vector rowCaptions;

    //! A multi-purposes matrix (mainly used for the digest)
    Matrix<double, double> matrix;

    unsigned int rowIndex;

    Yuni::Clob fileBuffer;

}; // class SurveyResultsData

/*!
** \brief Append the data of a matrix (about links variables) to the digest file
*/
void InternalExportDigestLinksMatrix(const Data::Study& study,
                                     Yuni::String& output,
                                     const char* title,
                                     std::string& fileBuffer,
                                     const Matrix<>& matrix);
} // namespace Private
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_SURVEY_RESULTS_DATA_H__
