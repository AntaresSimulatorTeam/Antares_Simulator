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

#include <yuni/yuni.h>
#include <antares/study/study.h>
#include "max-mrg.h"
#include <antares/study/area/scratchpad.h>

using namespace Yuni;

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Economy
{
template<bool WithSimplexT>
struct SpillageSelector
{
    template<class U>
    static auto Value(const State&, const U& weeklyResults, uint)
      -> decltype(weeklyResults.ValeursHorairesDeDefaillanceNegative)
    {
        return weeklyResults.ValeursHorairesDeDefaillanceNegative;
    }
};

template<>
struct SpillageSelector<false>
{
    template<class U>
    static auto Value(const State& state, const U&, uint index) -> decltype(state.resSpilled[index])
    {
        return state.resSpilled[index];
    }
};

MaxMRGinput dataToComputeMaxMRG(const State& state, unsigned int numSpace)
{
    auto& area = *state.area;
    auto index = area.index;
    auto& problem = *state.problemeHebdo;
    auto& weeklyResults = problem.ResultatsHoraires[index];

    MaxMRGinput maxMrGinput;

    // Spillage
    if (state.simplexRunNeeded)
    {
        maxMrGinput.spillage = weeklyResults.ValeursHorairesDeDefaillanceNegative.data();
    }
    else
    {
        maxMrGinput.spillage = state.resSpilled[index];
    }

    maxMrGinput.dens = weeklyResults.ValeursHorairesDeDefaillancePositive.data();

    maxMrGinput.hydroGeneration = weeklyResults.TurbinageHoraire.data();
    maxMrGinput.hydroMaxPower = area.hydro.maxPower[Data::PartHydro::genMaxP];
    maxMrGinput.dtgMargin = area.scratchpad[numSpace].dispatchableGenerationMargin;
    maxMrGinput.hourInYear = state.hourInTheYear;
    maxMrGinput.calendar = &state.study.calendar;
    maxMrGinput.areaName = area.name.c_str();

    return maxMrGinput;
}

void computeMaxMRG(double* opmrg, MaxMRGinput& in)
{
    const unsigned int nbHoursInWeeks = 168;
    assert(nbHoursInWeeks + state.hourInTheYear <= HOURS_PER_YEAR);
    assert(opmrg && "Invalid OP.MRG target");

    double OI[168];

    const double* H = in.hydroGeneration;

    // Energie turbinee de la semaine
    {
        const double* M = in.dtgMargin;

        double WH = 0.;
        {
            for (uint i = 0; i != nbHoursInWeeks; ++i)
                WH += H[i];
        }

        if (Math::Zero(WH)) // no hydro
        {
            for (uint i = 0; i != nbHoursInWeeks; ++i)
                opmrg[i] = +in.spillage[i] + M[i] - in.dens[i];
            return;
        }

        // initialisation
        for (uint i = 0; i != nbHoursInWeeks; ++i)
            OI[i] = +in.spillage[i] + M[i] - in.dens[i];
    }

    double bottom = +std::numeric_limits<double>::max();
    double top = 0;

    for (uint i = 0; i != nbHoursInWeeks; ++i)
    {
        double oii = OI[i];
        if (oii > top)
            top = oii;
        if (oii < bottom)
            bottom = oii;
    }

    double ecart = 1.;
    uint loop = 100; // arbitrary - maximum number of iterations

    // Pmax
    const double* P = in.hydroMaxPower;

    do
    {
        double niveau = (top + bottom) * 0.5;
        double SP = 0; // S+
        double SM = 0; // S-

        for (uint i = 0; i != nbHoursInWeeks; ++i)
        {
            assert(i < HOURS_PER_YEAR && "calendar overflow");
            if (niveau > OI[i])
            {
                uint dayYear = in.calendar->hours[i + in.hourInYear].dayYear;
                opmrg[i] = Math::Min(niveau, OI[i] + P[dayYear] - H[i]);
                SM += opmrg[i] - OI[i];
            }
            else
            {
                opmrg[i] = Math::Max(niveau, OI[i] - H[i]);
                SP += OI[i] - opmrg[i];
            }
        }

        ecart = SP - SM;
        if (ecart > 0)
            bottom = niveau;
        else
            top = niveau;

        if (!--loop)
        {
            logs.error() << "OP.MRG: " << in.areaName
                         << ": infinite loop detected. please check input data";
            return;
        }
    } while (ecart * ecart > 0.25);
}

template<bool WithSimplexT>
inline void PrepareMaxMRGFor(const State& state, double* opmrg, uint numSpace)
{
    const unsigned int nbHoursInWeeks = 168;
    assert(nbHoursInWeeks + state.hourInTheYear <= HOURS_PER_YEAR);
    assert(opmrg && "Invalid OP.MRG target");

    // current area
    auto& area = *state.area;
    // index of the current area
    auto index = area.index;

    // current problem
    auto& problem = *state.problemeHebdo;
    // Weekly results from solver for the current area
    auto& weeklyResults = problem.ResultatsHoraires[index];
    // Unsupplied enery for the current area
    auto& D = weeklyResults.ValeursHorairesDeDefaillancePositive;
    // Spillage
    auto S = SpillageSelector<WithSimplexT>::Value(state, weeklyResults, area.index);

    double OI[168];

    const std::vector<double>& H = weeklyResults.TurbinageHoraire;

    // Energie turbinee de la semaine
    {
        const double* M = area.scratchpad[numSpace].dispatchableGenerationMargin;

        double WH = 0.;
        {
            for (uint i = 0; i != nbHoursInWeeks; ++i)
                WH += H[i];
        }

        if (Math::Zero(WH)) // no hydro
        {
            for (uint i = 0; i != nbHoursInWeeks; ++i)
                opmrg[i] = +S[i] + M[i] - D[i];
            return;
        }

        // initialisation
        for (uint i = 0; i != nbHoursInWeeks; ++i)
            OI[i] = +S[i] + M[i] - D[i];
    }

    double bottom = +std::numeric_limits<double>::max();
    double top = 0;

    for (uint i = 0; i != nbHoursInWeeks; ++i)
    {
        double oii = OI[i];
        if (oii > top)
            top = oii;
        if (oii < bottom)
            bottom = oii;
    }

    double ecart = 1.;
    uint loop = 100; // arbitrary - maximum number of iterations

    // ref to the study calendar
    auto& calendar = state.study.calendar;
    // Pmax
    const auto& P = area.hydro.maxPower[Data::PartHydro::genMaxP];

    do
    {
        double niveau = (top + bottom) * 0.5;
        double SP = 0; // S+
        double SM = 0; // S-

        for (uint i = 0; i != nbHoursInWeeks; ++i)
        {
            assert(i < HOURS_PER_YEAR && "calendar overflow");
            if (niveau > OI[i])
            {
                uint dayYear = calendar.hours[i + state.hourInTheYear].dayYear;
                opmrg[i] = Math::Min(niveau, OI[i] + P[dayYear] - H[i]);
                SM += opmrg[i] - OI[i];
            }
            else
            {
                opmrg[i] = Math::Max(niveau, OI[i] - H[i]);
                SP += OI[i] - opmrg[i];
            }
        }

        ecart = SP - SM;
        if (ecart > 0)
            bottom = niveau;
        else
            top = niveau;

        if (!--loop)
        {
            logs.error() << "OP.MRG: " << area.name
                         << ": infinite loop detected. please check input data";
            return;
        }
    } while (ecart * ecart > 0.25);
}

void PrepareMaxMRG(const State& state, double* opmrg, uint numSpace)
{
    if (state.simplexRunNeeded)
        PrepareMaxMRGFor<true>(state, opmrg, numSpace);
    else
        PrepareMaxMRGFor<false>(state, opmrg, numSpace);
}

} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares
