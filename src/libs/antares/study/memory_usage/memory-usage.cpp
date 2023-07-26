/*
** Copyright 2007-2023 RTE
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

#include "memory-usage.h"
#include <antares/study/study.h>

using namespace Yuni;

namespace Antares
{
namespace Data
{
// gp : voir l'impact de la selection des variables ici
StudyMemoryUsage::StudyMemoryUsage(const Study& s) :
 mode(s.parameters.mode),
 gatheringInformationsForInput(false),
 requiredMemory(),
 requiredMemoryForInput(),
 requiredMemoryForOutput(),
 requiredDiskSpace(),
 requiredDiskSpaceForOutput(),
 study(s),
 years(s.parameters.nbYears),
 nbYearsParallel(s.maxNbYearsInParallel),
 buffer(nullptr),
 area(nullptr)
{
    // alias to parameters
    auto& parameters = study.parameters;
    assert(parameters.yearsFilter.size() == years && "Invalid size for yearsFilter");
    // playlist
    if (parameters.userPlaylist && !parameters.yearsFilter.empty())
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
    requiredDiskSpaceForOutput += rhs.requiredDiskSpaceForOutput;
    return *this;
}

void StudyMemoryUsage::estimate()
{
    study.estimateMemoryUsageForInput(*this);
    study.estimateMemoryUsageForOutput(*this);

    // Post-operations
    requiredMemory = requiredMemoryForInput + requiredMemoryForOutput;
    requiredDiskSpace = requiredDiskSpaceForOutput;
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

Yuni::uint64 computeOverheadDiskSpaceForAnyDataLevelComponent()
{
    // Reminder : a data level can be an area, a link or a binding constraint
    Yuni::uint64 diskSpace = 0;

    diskSpace += 160 * 1024; // hourly
    diskSpace += 6 * 1024;   // daily
    diskSpace += 6 * 1024;   // weekly
    diskSpace += 2 * 1024;   // monthly
    diskSpace += 1024;       // annual

    return diskSpace;
}

void StudyMemoryUsage::overheadDiskSpaceForSingleAreaOrLink()
{
    // x2 : values + IDs
    requiredDiskSpaceForOutput += 2 * computeOverheadDiskSpaceForAnyDataLevelComponent();
}

void StudyMemoryUsage::overheadDiskSpaceForSingleBindConstraint()
{
    requiredDiskSpaceForOutput += computeOverheadDiskSpaceForAnyDataLevelComponent();
}

unsigned int StudyMemoryUsage::NbYearsParallel() {
    return nbYearsParallel ;
}

bool StudyMemoryUsage::GatheringInformationsForInput() {
    return gatheringInformationsForInput;
}

void StudyMemoryUsage::AddRequiredMemoryForInput(unsigned int i) {
    requiredMemoryForInput += i;
}

void StudyMemoryUsage::AddRequiredMemoryForOutput(unsigned int i) {
    requiredMemoryForOutput += i;
}

} // namespace Data
} // namespace Antares
