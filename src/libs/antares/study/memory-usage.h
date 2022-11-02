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
#ifndef __ANTARES_LIBS_STUDY_MEMORYUSAGE_H__
#define __ANTARES_LIBS_STUDY_MEMORYUSAGE_H__

#include <yuni/yuni.h>
#include "fwd.h"
#include "../array/matrix.h"

namespace Antares
{
namespace Data
{
/*!
** \brief Estimate the amount of memory (RAM, disk) required for a simulation
*/
class StudyMemoryUsage final
{
public:
    //! \name Constructor
    //@{
    //! Default constructor
    StudyMemoryUsage(const Study& study);
    //! Destructor
    ~StudyMemoryUsage();
    //@}

    /*!
    ** \brief Estimate the amount memory required
    **
    ** The real amount of memory required to launch this study
    ** should be less than the returned value, but in the worst case
    ** it can be equal (or nearly).
    */
    void estimate();

    /*!
    ** \brief Consider in the estimation of the required disk space (output) the overhead produced
    *by a timeseries
    */
    void takeIntoConsiderationANewTimeserieForDiskOutput(bool withIDs = false);

    /*!
    ** \brief Consider in the estimation the overhead produced by a single area or link
    */
    void overheadDiskSpaceForSingleAreaOrLink();

    void overheadDiskSpaceForSingleBindConstraint();

public:
    //! \name Input data
    //@{
    //! Study mode (economy / adequacy / other)
    StudyMode mode;
    //! For matrices
    bool gatheringInformationsForInput;
    //@}

    //! \name Output data
    //@{
    //! Total Amount of memory required for a simulation (input+output)
    Yuni::uint64 requiredMemory;
    //! Amount of memory required by the input data for a simulation
    Yuni::uint64 requiredMemoryForInput;
    //! Amount of memory required by the output data for a simulation
    Yuni::uint64 requiredMemoryForOutput;

    //! Total Amount of disk space required for a simulation
    Yuni::uint64 requiredDiskSpace;
    //! Amount of disk space required by the output for a simulation
    Yuni::uint64 requiredDiskSpaceForOutput;
    //@}

    //! Reference to the study
    const Study& study;

    //! The total number of MC years
    uint years;

    //! Number max of parallel years to actually run in a set
    uint nbYearsParallel;

    //! A temporary buffer, sometimes used by matrices
    Matrix<>::BufferType* buffer;

    StudyMemoryUsage& operator+=(const StudyMemoryUsage& rhs);

    //! The current area
    const Area* area;

private:
    //! The number of simulation hours for a year
    uint pNbHours;
    //! The number of days per year
    uint pNbDays;
    //! The number of weeks for a year
    uint pNbWeeks;
    //! The number of months for a year
    uint pNbMonths;
    uint pNbMaxDigitForYear;

}; // class StudyMemoryUsage

} // namespace Data
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_MEMORYUSAGE_H__
