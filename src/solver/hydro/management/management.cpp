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

#include <yuni/yuni.h>
#include <antares/study/study.h>
#include <antares/study/area/scratchpad.h>
#include "management.h"
#include "../../simulation/sim_extern_variables_globales.h"
#include <yuni/core/math.h>
#include <limits>
#include <antares/study/parts/hydro/container.h>

using namespace Yuni;

namespace Antares
{
double HydroManagement::GammaVariable(double r)
{
    double x = 0.;
    do
    {
        double s = r - 1.;
        double u = random();
        double v = random();
        double w = u * (1. - u);
        assert(Math::Abs(w) > 1e-12);
        assert(3. * (r - 0.25) / w > 0.);
        double y = Math::SquareRootNoCheck(3. * (r - 0.25) / w) * (u - 0.5);

        x = y + s;
        if (v < 1e-12)
            break;

        w *= 4.;
        v *= w;
        double z = w * v * v;

        assert(Math::Abs(s) > 1e-12);
        assert(z > 0.);
        assert(z / s > 0.);
        if (log(z) <= 2. * (s * log(x / s) - y))
            break;
    } while (true);
    return x;
}

inline double HydroManagement::BetaVariable(double a, double b)
{
    double y = GammaVariable(a);
    double z = GammaVariable(b);
    assert(Math::Abs(y + z) > 1e-12);
    return y / (y + z);
}

HydroManagement::HydroManagement(Data::Study& study) : study(study), parameters(study.parameters)
{
    pAreas = new PerArea*[study.maxNbYearsInParallel];
    for (uint numSpace = 0; numSpace < study.maxNbYearsInParallel; numSpace++)
        pAreas[numSpace] = new PerArea[study.areas.size()];

    random.reset(study.parameters.seed[Data::seedHydroManagement]);
}

HydroManagement::~HydroManagement()
{
    for (uint numSpace = 0; numSpace < study.maxNbYearsInParallel; numSpace++)
        delete[] pAreas[numSpace];
    delete[] pAreas;
}

void HydroManagement::prepareInflowsScaling(uint numSpace)
{
    auto& calendar = study.calendar;

    study.areas.each([&](Data::Area& area) {
        uint z = area.index;

        auto& ptchro = *NumeroChroniquesTireesParPays[numSpace][z];

        auto& inflowsmatrix = area.hydro.series->storage; //CR22
        auto& mingenmatrix = area.hydro.series->mingen; //CR22 todo
        assert(inflowsmatrix.width && inflowsmatrix.height);
        auto tsIndex = (uint)ptchro.Hydraulique;
        auto const& srcinflows = inflowsmatrix[tsIndex < inflowsmatrix.width ? tsIndex : 0];
        auto const& srcmingen = mingenmatrix[tsIndex < mingenmatrix.width ? tsIndex : 0]; //CR22 todo

        auto& data = pAreas[numSpace][z];

        for (uint month = 0; month != 12; ++month)
        {
            uint realmonth = calendar.months[month].realmonth;

            double totalMonthInflows = 0.0;

            uint firstDayOfMonth = calendar.months[month].daysYear.first;

            uint firstDayOfNextMonth = calendar.months[month].daysYear.end;

            for (uint d = firstDayOfMonth; d != firstDayOfNextMonth; ++d)
                totalMonthInflows += srcinflows[d]; 

            if (not(area.hydro.reservoirCapacity < 1e-4))
            {
                if (area.hydro.reservoirManagement)
                {
                    data.inflows[realmonth] = totalMonthInflows / (area.hydro.reservoirCapacity);
                    assert(!Math::NaN(data.inflows[month]) && "nan value detect in inflows");
                }
                else
                    data.inflows[realmonth] = totalMonthInflows;
            }
            else
            {
                data.inflows[realmonth] = totalMonthInflows;
            }

            if (area.hydro.followLoadModulations and not area.hydro.reservoirManagement)
            {
                //CR22: Monthly minimum generation <= Monthly inflows for each month
                double totalMonthMingen = 0.0;
                for (uint d = firstDayOfMonth; d != firstDayOfNextMonth; ++d)
                {
                    for (uint h = 0; h < 24; ++h)
                    {
                        totalMonthMingen += srcmingen[d*24 + h];
                    }
                }
                if(totalMonthMingen > totalMonthInflows)
                {
                    logs.error() << "In Area "<< area.name << " the minimum generation of "
                    << totalMonthMingen << " MW in month " << month + 1 << " of TS-" << numSpace << " is incompatible with the inflows of "
                    << totalMonthInflows << " MW.";
                }
            }
        }

        if (area.hydro.followLoadModulations and area.hydro.reservoirManagement)
        {
            //CR22: Yearly minimum generation <= Yearly inflows for each year 
            double totalYearMingen = 0.0;
            double totalYearInflows = 0.0;
            for (uint hour = 0; hour < HOURS_PER_YEAR; ++hour)
            {
                totalYearMingen += srcmingen[hour];
            }
            for (uint day = 0; day < DAYS_PER_YEAR; ++day)
            {
                totalYearInflows += srcinflows[day];
            }
            if(totalYearMingen > totalYearInflows)
            {
                logs.error() << "In Area "<< area.name << " the minimum generation of "
                << totalYearMingen << " MW of TS-" << numSpace << " is incompatible with the inflows of "
                << totalYearInflows << " MW.";
            }
        }

        if (not area.hydro.followLoadModulations)
        {
            //CR22: Weekly minimum generation <= Weekly inflows for each week 
            for (uint week = 0; week < 53; ++week)
            {
                double totalWeekMingen = 0.0;
                double totalWeekInflows = 0.0;
                for(uint hour = calendar.weeks[week].hours.first; hour < calendar.weeks[week].hours.end; ++hour)
                {
                    totalWeekMingen += srcmingen[hour];
                }
                
                for(uint day = calendar.weeks[week].daysYear.first; day < calendar.weeks[week].daysYear.end; ++day)
                {
                    totalWeekInflows += srcinflows[day];
                }
                if(totalWeekMingen > totalWeekInflows)
                {
                    logs.error() << "In Area "<< area.name << " the minimum generation of "
                    << totalWeekMingen << " MW in week " << week + 1 << " of TS-" << numSpace << " is incompatible with the inflows of "
                    << totalWeekInflows << " MW.";
                }                
            }
        }
    });
}

template<enum Data::StudyMode ModeT>
void HydroManagement::prepareNetDemand(uint numSpace)
{
    study.areas.each([&](Data::Area& area) {
        uint z = area.index;

        auto& scratchpad = *(area.scratchpad[numSpace]);

        auto& ptchro = *NumeroChroniquesTireesParPays[numSpace][z];

        auto& rormatrix = area.hydro.series->ror; //CR22
        auto tsIndex = (uint)ptchro.Hydraulique;
        auto& ror = rormatrix[tsIndex < rormatrix.width ? tsIndex : 0];

        auto& data = pAreas[numSpace][z];

        for (uint hour = 0; hour != 8760; ++hour)
        {
            auto dayYear = study.calendar.hours[hour].dayYear;

            double netdemand = 0;

            // Aggregated renewable production: wind & solar
            if (parameters.renewableGeneration.isAggregated())
            {
                netdemand = +scratchpad.ts.load[ptchro.Consommation][hour]
                            - scratchpad.ts.wind[ptchro.Eolien][hour] - scratchpad.miscGenSum[hour]
                            - scratchpad.ts.solar[ptchro.Solar][hour] - ror[hour] //CR22 todo add mingen here or not?
                            - ((ModeT != Data::stdmAdequacy) ? scratchpad.mustrunSum[hour]
                                                             : scratchpad.originalMustrunSum[hour]);
            }

            // Renewable clusters, if enabled
            else if (parameters.renewableGeneration.isClusters())
            {
                netdemand = scratchpad.ts.load[ptchro.Consommation][hour]
                            - scratchpad.miscGenSum[hour] - ror[hour] //CR22 todo add mingen here or not?
                            - ((ModeT != Data::stdmAdequacy) ? scratchpad.mustrunSum[hour]
                                                             : scratchpad.originalMustrunSum[hour]);

                area.renewable.list.each([&](const Antares::Data::RenewableCluster& cluster) {
                    assert(cluster.series->series.jit == NULL && "No JIT data from the solver");
                    netdemand -= cluster.valueAtTimeStep(
                      ptchro.RenouvelableParPalier[cluster.areaWideIndex], hour);
                });
            }

            assert(!Math::NaN(netdemand)
                   && "hydro management: NaN detected when calculating the net demande");
            data.DLN[dayYear] += netdemand;
        }
    });
}

void HydroManagement::prepareEffectiveDemand(uint numSpace)
{
    study.areas.each([&](Data::Area& area) {
        auto z = area.index;

        auto& data = pAreas[numSpace][z];

        for (uint day = 0; day != 365; ++day)
        {
            auto month = study.calendar.days[day].month;
            assert(month < 12 && "Invalid month index");
            auto realmonth = study.calendar.months[month].realmonth;

            double effectiveDemand = 0;
            area.hydro.allocation.eachNonNull([&](unsigned areaindex, double value) {
                effectiveDemand += (pAreas[numSpace][areaindex]).DLN[day] * value;
            });

            assert(!Math::NaN(effectiveDemand) && "nan value detected for effectiveDemand");
            data.DLE[day] += effectiveDemand;
            data.MLE[realmonth] += effectiveDemand;

            assert(not Math::NaN(data.DLE[day]) && "nan value detected for DLE");
            assert(not Math::NaN(data.MLE[realmonth]) && "nan value detected for DLE");
        }

        auto minimumYear = std::numeric_limits<double>::infinity();
        auto dayYear = 0u;

        for (uint month = 0; month != 12; ++month)
        {
            auto minimumMonth = +std::numeric_limits<double>::infinity();
            auto daysPerMonth = study.calendar.months[month].days;
            auto realmonth = study.calendar.months[month].realmonth;

            for (uint d = 0; d != daysPerMonth; ++d)
            {
                auto dYear = d + dayYear;
                if (data.DLE[dYear] < minimumMonth)
                    minimumMonth = data.DLE[dYear];
            }

            if (minimumMonth < 0.)
            {
                for (uint d = 0; d != daysPerMonth; ++d)
                    data.DLE[dayYear + d] -= minimumMonth - 1e-4;
            }

            if (data.MLE[realmonth] < minimumYear)
                minimumYear = data.MLE[realmonth];

            dayYear += daysPerMonth;
        }

        if (minimumYear < 0.)
        {
            for (uint realmonth = 0; realmonth != 12; ++realmonth)
                data.MLE[realmonth] -= minimumYear - 1e-4;
        }
    });
}

double HydroManagement::randomReservoirLevel(double min, double avg, double max)
{
    if (Math::Equals(min, max))
        return avg;
    if (Math::Equals(avg, min) || Math::Equals(avg, max))
        return avg;

    double e = (avg - min) / (max - min);
    double re = 1. - e;

    assert(Math::Abs(1. + e) > 1e-12);
    assert(Math::Abs(2. - e) > 1e-12);

    double v1 = (e * e) * re / (1. + e);
    double v2 = e * re * re / (2. - e);
    double v = Math::Min(v1, v2) * .5;

    assert(Math::Abs(v) > 1e-12);

    double a = e * (e * re / v - 1.);
    double b = re * (e * re / v - 1.);

    double x = BetaVariable(a, b);
    return x * max + (1. - x) * min;
}

void HydroManagement::operator()(double* randomReservoirLevel,
                                 Solver::Variable::State& state,
                                 uint y,
                                 uint numSpace)
{
    memset(pAreas[numSpace], 0, sizeof(PerArea) * study.areas.size());

    prepareInflowsScaling(numSpace);

    if (parameters.adequacy())
        prepareNetDemand<Data::stdmAdequacy>(numSpace);
    else
        prepareNetDemand<Data::stdmEconomy>(numSpace);

    prepareEffectiveDemand(numSpace);

    prepareMonthlyOptimalGenerations(randomReservoirLevel, y, numSpace);
    prepareDailyOptimalGenerations(state, y, numSpace);
}

} // namespace Antares
