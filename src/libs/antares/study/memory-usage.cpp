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

#include "study.h"
#include "memory-usage.h"
#include "../date.h"

using namespace Yuni;

namespace Antares
{
namespace Data
{
// gp : voir l'impact de la selection des variables ici
StudyMemoryUsage::StudyMemoryUsage(const Study& s) :
 mode(s.parameters.mode),
 swappingSupport(false),
 gatheringInformationsForInput(false),
 requiredMemory(),
 requiredMemoryForInput(),
 requiredMemoryForOutput(),
 requiredDiskSpace(),
 requiredDiskSpaceForSwap(),
 requiredDiskSpaceForOutput(),
 study(s),
 years(s.parameters.nbYears),
 nbYearsParallel(s.maxNbYearsInParallel),
 buffer(nullptr),
 area(nullptr)
{
    // alias to parameters
    auto& parameters = study.parameters;

    // playlist
    if (parameters.userPlaylist and not parameters.yearsFilter.empty())
    {
        uint y = 0;
        for (uint i = 0; i != years; ++i)
        {
            if (parameters.yearsFilter[i])
                ++y;
        }
        years = y;
    }

    if (parameters.simulationDays.end > parameters.simulationDays.first)
    {
        pNbDays = parameters.simulationDays.end - parameters.simulationDays.first;
        pNbHours = pNbDays * 24;
        pNbWeeks = pNbDays / 7;
        pNbMonths = 1 + study.calendar.days[parameters.simulationDays.end - 1].month
                    - study.calendar.days[parameters.simulationDays.first].month;
    }
    else
    {
        pNbHours = 0;
        pNbDays = 0;
        pNbWeeks = 0;
        pNbMonths = 0;
    }

    pNbMaxDigitForYear = 1;

    uint y = years;
    while ((y /= 10) != 0)
        ++pNbMaxDigitForYear;
}

StudyMemoryUsage::~StudyMemoryUsage()
{
    delete buffer;
}

StudyMemoryUsage& StudyMemoryUsage::operator+=(const StudyMemoryUsage& rhs)
{
    requiredMemory += rhs.requiredMemory;
    requiredMemoryForInput += rhs.requiredMemoryForInput;
    requiredMemoryForOutput += rhs.requiredMemoryForOutput;
    requiredDiskSpace += rhs.requiredDiskSpace;
    requiredDiskSpaceForSwap += rhs.requiredDiskSpaceForSwap;
    requiredDiskSpaceForOutput += rhs.requiredDiskSpaceForOutput;
    return *this;
}

void StudyMemoryUsage::estimate()
{
    study.estimateMemoryUsageForInput(*this);
    study.estimateMemoryUsageForOutput(*this);

    if (requiredDiskSpaceForSwap != 0)
    {
        Yuni::uint64 swap = 0;
        do
        {
            swap += Antares::Memory::swapSize;
        } while (swap < requiredDiskSpaceForSwap);

        requiredDiskSpaceForSwap = swap + Antares::Memory::swapSize;
    }

    // Post-operations
    requiredMemory = requiredMemoryForInput + requiredMemoryForOutput;
    requiredDiskSpace = requiredDiskSpaceForSwap + requiredDiskSpaceForOutput;
}

void StudyMemoryUsage::takeIntoConsiderationANewTimeserieForDiskOutput(bool withIDs)
{
    enum
    {
        o = 7 /*line feed*/ + 8 /*text*/
    };

    // The number of digits is an estimation
    // values hourly
    requiredDiskSpaceForOutput += (4 /*digits*/ + 1 /*tab*/) * pNbHours + o;
    // values daily
    requiredDiskSpaceForOutput += (4 /*digits*/ + 1 /*tab*/) * pNbDays + o;
    // values weekly
    requiredDiskSpaceForOutput += (6 /*digits*/ + 1 /*tab*/) * pNbWeeks + o;
    // values monthly
    requiredDiskSpaceForOutput += (6 /*digits*/ + 1 /*tab*/) * pNbMonths + o;
    // values annual
    requiredDiskSpaceForOutput += (8 /*digits*/ + 1 /*tab*/) * 1 + o;

    if (withIDs)
    {
        // ID hourly
        requiredDiskSpaceForOutput += (pNbMaxDigitForYear + 1 /*tab*/) * pNbHours + o;
        // ID daily
        requiredDiskSpaceForOutput += (pNbMaxDigitForYear + 1 /*tab*/) * pNbDays + o;
        // ID weekly
        requiredDiskSpaceForOutput += (pNbMaxDigitForYear + 1 /*tab*/) * pNbWeeks + o;
        // ID monthly
        requiredDiskSpaceForOutput += (pNbMaxDigitForYear + 1 /*tab*/) * pNbMonths + o;
        // ID annual
        requiredDiskSpaceForOutput += (pNbMaxDigitForYear + 1 /*tab*/) * 1 + o;
    }
}

void StudyMemoryUsage::overheadDiskSpaceForSingleAreaOrLink()
{
    // x2 : values + IDs
    // hourly
    requiredDiskSpaceForOutput += 2 * 160 * 1024;
    // daily
    requiredDiskSpaceForOutput += 2 * 6 * 1024;
    // weekly
    requiredDiskSpaceForOutput += 2 * 6 * 1024;
    // monthly
    requiredDiskSpaceForOutput += 2 * 2 * 1024;
    // annual
    requiredDiskSpaceForOutput += 2 * 1 * 1024;
}

} // namespace Data
} // namespace Antares
