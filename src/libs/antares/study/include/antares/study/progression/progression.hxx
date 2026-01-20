// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_PROGRESSION_PROGRESSION_HXX__
#define __ANTARES_LIBS_STUDY_PROGRESSION_PROGRESSION_HXX__

namespace Antares::Solver
{
inline Progression::Meter::Meter():
    nbParallelYears(0)
{
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
    pProgressMeter.logsContainer.resize(nb);
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

} // namespace Antares::Solver

#endif // __ANTARES_LIBS_STUDY_PROGRESSION_PROGRESSION_HXX__
