// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_STORAGE_RAWDATA_H__
#define __SOLVER_VARIABLE_STORAGE_RAWDATA_H__

#include <yuni/yuni.h>

#include <antares/study/study.h>

#include "intermediate.h"

namespace Antares::Solver::Variable::R::AllYears
{
class RawData
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    RawData();
    //! Destructor
    ~RawData();
    //@}

public:
    void initializeFromStudy(const Data::Study& study);
    void reset();
    void merge(unsigned int year, const IntermediateValues& rhs);

public:
    double monthly[MONTHS_PER_YEAR];
    double weekly[WEEKS_PER_YEAR];
    double daily[DAYS_PER_YEAR];
    Antares::Memory::Stored<double>::Type hourly;
    std::vector<double> year;
    mutable double allYears;
    unsigned int nbYearsCapacity;

}; // class RawData

} // namespace Antares::Solver::Variable::R::AllYears

#endif // __SOLVER_VARIABLE_STORAGE_RAWDATA_H__
