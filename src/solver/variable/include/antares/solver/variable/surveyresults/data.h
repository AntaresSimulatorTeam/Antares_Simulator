// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_SURVEY_RESULTS_DATA_H__
#define __SOLVER_VARIABLE_SURVEY_RESULTS_DATA_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>

#include <antares/study/study.h>
#include <antares/writer/i_writer.h>
#include "antares/antares/constants.h"

namespace Antares::Solver::Variable::Private
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
    void exportGridInfos(IResultWriter& writer);

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
                                     const char* title,
                                     std::string& fileBuffer,
                                     const Matrix<>& matrix);
} // namespace Antares::Solver::Variable::Private

#endif // __SOLVER_VARIABLE_SURVEY_RESULTS_DATA_H__
