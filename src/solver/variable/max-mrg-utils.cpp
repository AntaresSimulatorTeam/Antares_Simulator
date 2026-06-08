// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <yuni/yuni.h>

#include <antares/solver/variable/economy/max-mrg-utils.h>
#include <antares/study/area/scratchpad.h>
#include <antares/study/study.h>

using namespace Yuni;

const unsigned int nbHoursInWeek = 168;

namespace Antares::Solver::Variable::Economy
{

MaxMrgDataFactory::MaxMrgDataFactory(const State& state, unsigned int numSpace):
    state_(state),
    numSpace_(numSpace),
    weeklyResults_(state.problemeHebdo->ResultatsHoraires[state.area->index])
{
    maxMRGinput_.hydroGeneration = weeklyResults_.TurbinageHoraire.data();
    maxMRGinput_.maxHourlyGenPower = &state_.area->hydro.series->maxHourlyGenPower;
    maxMRGinput_.dtgMargin = state_.area->scratchpad[numSpace].dispatchableGenerationMargin;
    maxMRGinput_.hourInYear = state.hourInTheYear;
    maxMRGinput_.year = state.problemeHebdo->year;
    maxMRGinput_.calendar = &state.study.calendar;
    maxMRGinput_.areaName = state_.area->name.c_str();
}

MaxMRGinput MaxMrgUsualDataFactory::data()
{
    if (state_.simplexRunNeeded)
    {
        maxMRGinput_.spillage = weeklyResults_.ValeursHorairesDeDefaillanceNegative.data();
    }
    else
    {
        maxMRGinput_.spillage = state_.resSpilled[state_.area->index];
    }

    maxMRGinput_.dens = weeklyResults_.ValeursHorairesDeDefaillancePositive.data();
    return maxMRGinput_;
}

MaxMRGinput MaxMrgCSRdataFactory::data()
{
    maxMRGinput_.spillage = weeklyResults_.ValeursHorairesDeDefaillanceNegative.data();
    maxMRGinput_.dens = weeklyResults_.ValeursHorairesDeDefaillancePositiveCSR.data();
    return maxMRGinput_;
}

void computeMaxMRG(double* maxMrgOut, const MaxMRGinput& in)
{
    assert(maxMrgOut && "Invalid OP.MRG target");

    // Following block could be replaced with :
    // double weekHydroGen = std::accumulate(in.hydroGeneration, in.hydroGeneration + nbHoursInWeek,
    // 0.);
    double weekHydroGen = 0.;
    for (uint h = 0; h != nbHoursInWeek; ++h)
    {
        weekHydroGen += in.hydroGeneration[h];
    }

    if (Yuni::Math::Zero(weekHydroGen))
    {
        for (uint h = 0; h != nbHoursInWeek; ++h)
        {
            maxMrgOut[h] = in.spillage[h] + in.dtgMargin[h] - in.dens[h];
        }
        return;
    }

    // Initialisation
    std::vector<double> OI(nbHoursInWeek);
    for (uint h = 0; h != nbHoursInWeek; ++h)
    {
        OI[h] = in.spillage[h] + in.dtgMargin[h] - in.dens[h];
    }

    // Following block could be replaced with :
    // double bottom = *std::min_element(OI.begin(), OI.end());
    // double top = *std::max_element(OI.begin(), OI.end());
    double bottom = +std::numeric_limits<double>::max();
    double top = 0;
    for (uint i = 0; i != nbHoursInWeek; ++i)
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

    do
    {
        double niveau = (top + bottom) * 0.5;
        double SP = 0; // S+
        double SM = 0; // S-

        for (uint h = 0; h != nbHoursInWeek; ++h)
        {
            assert(h < HOURS_PER_YEAR && "calendar overflow");
            if (niveau > OI[h])
            {
                maxMrgOut[h] = Math::Min(niveau,
                                         OI[h]
                                           + in.maxHourlyGenPower->getCoefficient(in.year,
                                                                                  h + in.hourInYear)
                                           - in.hydroGeneration[h]);
                SM += maxMrgOut[h] - OI[h];
            }
            else
            {
                maxMrgOut[h] = Math::Max(niveau, OI[h] - in.hydroGeneration[h]);
                SP += OI[h] - maxMrgOut[h];
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
            logs.error() << "OP.MRG: " << in.areaName
                         << ": infinite loop detected. please check input data";
            return;
        }
    } while (ecart * ecart > 0.25);
}

} // namespace Antares::Solver::Variable::Economy
