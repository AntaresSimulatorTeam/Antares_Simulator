// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_STORAGE_AVERAGE_DATA_H__
#define __SOLVER_VARIABLE_STORAGE_AVERAGE_DATA_H__

#include <antares/study/study.h>
#include "antares/solver/variable/storage/intermediate.h"

using HighPrecision = long double;

namespace Antares::Solver::Variable::R::AllYears
{
class AverageData
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    AverageData();
    //! Destructor
    ~AverageData();

    void initializeFromStudy(Data::Study& study);

    void reset();

    void merge(unsigned int year, const IntermediateValues& rhs);

public:
    std::vector<HighPrecision> year;
    std::vector<HighPrecision> monthly{MONTHS_PER_YEAR};
    std::vector<HighPrecision> weekly{WEEKS_PER_YEAR};
    std::vector<HighPrecision> daily{DAYS_PER_YEAR};
    std::vector<HighPrecision> hourly{HOURS_PER_YEAR};
    unsigned int nbYearsCapacity;
    mutable double allYears; // FIX MEEE - Remove the mutable as soon as possible
    std::vector<float> yearsWeight;
    float yearsWeightSum;

}; // class AverageData

} // namespace Antares::Solver::Variable::R::AllYears

#endif // __SOLVER_VARIABLE_STORAGE_AVERAGE_DATA_H__
