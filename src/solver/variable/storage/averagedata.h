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
#ifndef __SOLVER_VARIABLE_STORAGE_AVERAGE_DATA_H__
#define __SOLVER_VARIABLE_STORAGE_AVERAGE_DATA_H__

#include <antares/study.h>

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

    Yuni::uint64 dynamicMemoryUsage() const
    {
        return
#ifdef ANTARES_SWAP_SUPPORT
          0
#else
          sizeof(double) * maxHoursInAYear
#endif
          + sizeof(double) * nbYearsCapacity;
    }

public:
    double monthly[maxMonths];
    double weekly[maxWeeksInAYear];
    double daily[maxDaysInAYear];
    Antares::Memory::Stored<double>::Type hourly;
    double* year;
    mutable double allYears; // FIX MEEE - Remove the mutable as soon as possible
    unsigned int nbYearsCapacity;

    std::vector<float> yearsWeight;
    float yearsWeightSum;

}; // class AverageData

} // namespace AllYears
} // namespace R
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_STORAGE_AVERAGE_DATA_H__
