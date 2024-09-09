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
#ifndef __ANTARES_LIBS_STUDY_PROGRESSION_PROGRESSION_HXX__
#define __ANTARES_LIBS_STUDY_PROGRESSION_PROGRESSION_HXX__

namespace Antares
{
namespace Solver
{
inline Progression::Meter::Meter():
    nbParallelYears(0),
    logsContainer(nullptr)
{
}

inline void Progression::Meter::allocateLogsContainer(uint nb)
{
    logsContainer = new Yuni::CString<256, false>[nb];
}

inline void Progression::Meter::taskCount(uint n)
{
    (void)n;
}

inline void Progression::add(Section section, int nbTicks)
{
    add((uint)-1, section, nbTicks);
}

inline void Progression::setNumberOfParallelYears(uint nb)
{
    pProgressMeter.nbParallelYears = nb;
    pProgressMeter.allocateLogsContainer(nb);
}

inline Progression::Part& Progression::begin(uint year, Progression::Section section)
{
    // Alias
    Part& part = pProgressMeter.parts[year][section];
    // Reset
    part.tickCount = 0;
    // It is useless to display 0%, so lastTickCount and tickCount can be equals
    part.lastTickCount = 0;
    pProgressMeter.mutex.lock();
    pProgressMeter.inUse.push_front(&part);
    pProgressMeter.mutex.unlock();
    return part;
}

inline const char* Progression::SectionToCStr(Section section)
{
    static const char* const sectName[sectMax] = {"mc",
                                                  "output",
                                                  "load",
                                                  "solar",
                                                  "wind",
                                                  "hydro",
                                                  "thermal",
                                                  "import timeseries"};
    assert((uint)section < (uint)sectMax);
    return sectName[section];
}

} // namespace Solver
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_PROGRESSION_PROGRESSION_HXX__
