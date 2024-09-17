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

#include "antares/solver/variable/economy/max-mrg.h"

#include <yuni/yuni.h>

#include <antares/study/area/scratchpad.h>
#include <antares/study/study.h>

using namespace Yuni;

namespace Antares::Solver::Variable::Economy
{
template<bool WithSimplexT>
struct SpillageSelector
{
    template<class U>
    static auto Value(const State&,
                      const U& weeklyResults,
                      uint) -> decltype(weeklyResults.ValeursHorairesDeDefaillanceNegative)
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

template<bool WithSimplexT>
inline void PrepareMaxMRGFor(const State& state, double* opmrg, uint numSpace)
{
    assert(168 + state.hourInTheYear <= HOURS_PER_YEAR);
    assert(opmrg && "Invalid OP.MRG target");

    enum
    {
        offset = 0,
        endHour = 168,
    };

    // current area
    auto& area = *state.area;
    // index of the current area
    auto index = area.index;
    assert(area.index < 50000 && "seems invalid");

    // current problem
    auto& problem = *state.problemeHebdo;
    // Weekly results from solver for the current area
    auto& weeklyResults = problem.ResultatsHoraires[index];
    // Unsupplied enery for the current area
    auto& D = weeklyResults.ValeursHorairesDeDefaillancePositive;
    // Spillage
    auto S = SpillageSelector<WithSimplexT>::Value(state, weeklyResults, area.index);

    double OI[168];

    // H.STOR
    auto H = [&weeklyResults](int i) { return weeklyResults.HydroUsage[i].TurbinageHoraire; };

    // energie turbinee de la semaine
    {
        // DTG MRG
        const double* M = area.scratchpad[numSpace].dispatchableGenerationMargin;

        double WH = 0.;
        {
            // H.STOR
            for (uint i = offset; i != endHour; ++i)
            {
                WH += H(i);
            }
        }

        if (Utils::isZero(WH)) // no hydro
        {
            for (uint i = offset; i != endHour; ++i)
            {
                opmrg[i] = +S[i] + M[i] - D[i];
            }
            return;
        }

        // initialisation
        for (uint i = offset; i != endHour; ++i)
        {
            OI[i] = +S[i] + M[i] - D[i];
        }
    }

    double bottom = +std::numeric_limits<double>::max();
    double top = 0;

    for (uint i = offset; i != endHour; ++i)
    {
        double oii = OI[i];
        if (oii > top)
        {
            top = oii;
        }
        if (oii < bottom)
        {
            bottom = oii;
        }
    }

    double ecart = 1.;
    uint loop = 100; // arbitrary - maximum number of iterations

    // Pmax
    const uint y = problem.year;
    const auto& P = area.hydro.series->maxHourlyGenPower;

    do
    {
        double niveau = (top + bottom) * 0.5;
        double SP = 0; // S+
        double SM = 0; // S-

        for (uint i = offset; i != endHour; ++i)
        {
            assert(i < HOURS_PER_YEAR && "calendar overflow");
            if (niveau > OI[i])
            {
                opmrg[i] = std::min(niveau,
                                    OI[i] + P.getCoefficient(y, i + state.hourInTheYear) - H(i));
                SM += opmrg[i] - OI[i];
            }
            else
            {
                opmrg[i] = std::max(niveau, OI[i] - H(i));
                SP += OI[i] - opmrg[i];
            }
        }

        ecart = SP - SM;
        if (ecart > 0)
        {
            bottom = niveau;
        }
        else
        {
            top = niveau;
        }

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
    {
        PrepareMaxMRGFor<true>(state, opmrg, numSpace);
    }
    else
    {
        PrepareMaxMRGFor<false>(state, opmrg, numSpace);
    }
}

} // namespace Antares::Solver::Variable::Economy
