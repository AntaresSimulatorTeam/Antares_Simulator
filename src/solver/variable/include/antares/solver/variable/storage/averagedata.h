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
#ifndef __SOLVER_VARIABLE_STORAGE_AVERAGE_DATA_H__
#define __SOLVER_VARIABLE_STORAGE_AVERAGE_DATA_H__

#include <antares/study/study.h>
#include "antares/solver/variable/storage/intermediate.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace R
{
namespace AllYears
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
    double monthly[MONTHS_PER_YEAR];
    double weekly[WEEKS_PER_YEAR];
    double daily[DAYS_PER_YEAR];
    Antares::Memory::Stored<double>::Type hourly;
    std::vector<double> year;
    unsigned int nbYearsCapacity;
    mutable double allYears; // FIX MEEE - Remove the mutable as soon as possible
    std::vector<float> yearsWeight;
    float yearsWeightSum;

}; // class AverageData

} // namespace AllYears
} // namespace R
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_STORAGE_AVERAGE_DATA_H__
