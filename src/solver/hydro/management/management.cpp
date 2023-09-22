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
#include <antares/study/area/scratchpad.h>
#include <antares/fatal-error.h>
#include "management.h"
#include "../../simulation/sim_extern_variables_globales.h"
#include <yuni/core/math.h>
#include <limits>
#include <antares/study/parts/hydro/container.h>
#include <numeric>

using namespace Yuni;

namespace Antares
{
double HydroManagement::GammaVariable(double r)
{
    double x = 0.;
    do
    {
        double s = r - 1.;
        double u = random_();
        double v = random_();
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

HydroManagement::HydroManagement(const Data::AreaList& areas,
                                 const Data::Parameters& params,
                                 const Date::Calendar& calendar,
                                 unsigned int maxNbYearsInParallel,
                                 Solver::IResultWriter::Ptr resultWriter) :
    areas_(areas),
    calendar_(calendar),
    parameters_(params),
    maxNbYearsInParallel_(maxNbYearsInParallel),
    resultWriter_(resultWriter)
{
    tmpDataByArea_ = new TmpDataByArea* [maxNbYearsInParallel_];
    for (uint numSpace = 0; numSpace < maxNbYearsInParallel_; numSpace++)
        tmpDataByArea_[numSpace] = new TmpDataByArea[areas_.size()];

    random_.reset(parameters_.seed[Data::seedHydroManagement]);

    // Ventilation results memory allocation
    uint nbDaysPerYear = 365;
    ventilationResults_.resize(maxNbYearsInParallel_);
    for (uint numSpace = 0; numSpace < maxNbYearsInParallel_; numSpace++)
    {
        ventilationResults_[numSpace].resize(areas_.size());
        for (uint areaIndex = 0; areaIndex < areas_.size(); ++areaIndex)
        {
            auto& area = *areas_.byIndex[areaIndex];
            size_t clusterCount = area.thermal.clusterCount();

            ventilationResults_[numSpace][areaIndex].HydrauliqueModulableQuotidien.assign(nbDaysPerYear, 0);

            if (area.hydro.reservoirManagement)
            {
                ventilationResults_[numSpace][areaIndex].NiveauxReservoirsDebutJours.assign(nbDaysPerYear, 0.);
                ventilationResults_[numSpace][areaIndex].NiveauxReservoirsFinJours.assign(nbDaysPerYear, 0.);
            }
        }
    }
}

HydroManagement::~HydroManagement()
{
    for (uint numSpace = 0; numSpace < maxNbYearsInParallel_; numSpace++)
        delete[] tmpDataByArea_[numSpace];
    delete[] tmpDataByArea_;
}

void HydroManagement::prepareInflowsScaling(uint numSpace)
{
    areas_.each([&](Data::Area& area)
      {
          uint z = area.index;

          auto& ptchro = NumeroChroniquesTireesParPays[numSpace][z];

          auto& inflowsmatrix = area.hydro.series->storage;
          assert(inflowsmatrix.width && inflowsmatrix.height);
          auto tsIndex = (uint)ptchro.Hydraulique;
          auto const& srcinflows = inflowsmatrix[tsIndex < inflowsmatrix.width ? tsIndex : 0];

          auto& data = tmpDataByArea_[numSpace][z];
          double totalYearInflows = 0.0;

          for (uint month = 0; month != 12; ++month)
          {
              uint realmonth = calendar_.months[month].realmonth;

              double totalMonthInflows = 0.0;

              uint firstDayOfMonth = calendar_.months[month].daysYear.first;

              uint firstDayOfNextMonth = calendar_.months[month].daysYear.end;

              for (uint d = firstDayOfMonth; d != firstDayOfNextMonth; ++d)
                  totalMonthInflows += srcinflows[d];

              data.totalMonthInflows[realmonth] = totalMonthInflows;
              totalYearInflows += totalMonthInflows;

              if (not(area.hydro.reservoirCapacity < 1e-4))
              {
                  if (area.hydro.reservoirManagement)
                  {
                      data.inflows[realmonth] = totalMonthInflows / (area.hydro.reservoirCapacity);
                      assert(!Math::NaN(data.inflows[month]) && "nan value detect in inflows");
                  }
                  else
                  {
                      data.inflows[realmonth] = totalMonthInflows;
                  }
              }
              else
              {
                  data.inflows[realmonth] = totalMonthInflows;
              }
          }
          data.totalYearInflows = totalYearInflows;
      });
}

void HydroManagement::minGenerationScaling(uint numSpace)
{
    areas_.each([this, &numSpace](Data::Area& area)
      {
          uint z = area.index;

          const auto& ptchro = NumeroChroniquesTireesParPays[numSpace][z];
          auto& mingenmatrix = area.hydro.series->mingen;
          auto tsIndex = (uint)ptchro.Hydraulique;
          auto const& srcmingen = mingenmatrix[tsIndex < mingenmatrix.width ? tsIndex : 0];

          auto& data = tmpDataByArea_[numSpace][z];
          double totalYearMingen = 0.0;

          for (uint month = 0; month != 12; ++month)
          {
              uint realmonth = calendar_.months[month].realmonth;
              uint firstDayOfMonth = calendar_.months[month].daysYear.first;
              uint firstDayOfNextMonth = calendar_.months[month].daysYear.end;

              double totalMonthMingen = std::accumulate(
                srcmingen + firstDayOfMonth * 24, srcmingen + firstDayOfNextMonth * 24, 0.);

              data.totalMonthMingen[realmonth] = totalMonthMingen;
              totalYearMingen += totalMonthMingen;

              if (!(area.hydro.reservoirCapacity < 1e-4))
              {
                  if (area.hydro.reservoirManagement)
                  {
                      // Set monthly mingen, used later for h2o_m
                      data.mingens[realmonth] = totalMonthMingen / (area.hydro.reservoirCapacity);
                      assert(!Math::NaN(data.mingens[month]) && "nan value detect in mingen");
                  }
                  else
                  {
                      data.mingens[realmonth] = totalMonthMingen;
                  }
              }
              else
              {
                  data.mingens[realmonth] = totalMonthMingen;
              }

              // Set daily mingen, used later for h2o_d
              uint simulationMonth = calendar_.mapping.months[realmonth];
              auto daysPerMonth = calendar_.months[simulationMonth].days;
              uint firstDay = calendar_.months[simulationMonth].daysYear.first;
              uint endDay = firstDay + daysPerMonth;

              for (uint day = firstDay; day != endDay; ++day)
              {
                  data.dailyMinGen[day]
                    = std::accumulate(srcmingen + day * 24, srcmingen + day * 24 + 24, 0.);
              }
          }
          data.totalYearMingen = totalYearMingen;
      });
}

bool HydroManagement::checkMonthlyMinGeneration(uint numSpace, uint tsIndex, const Data::Area& area) const
{
    const auto& data = tmpDataByArea_[numSpace][area.index];
    for (uint month = 0; month != 12; ++month)
    {
        uint realmonth = calendar_.months[month].realmonth;
        // Monthly minimum generation <= Monthly inflows for each month
        if (data.totalMonthMingen[realmonth] > data.totalMonthInflows[realmonth])
        {
            logs.error() << "In Area " << area.name << " the minimum generation of "
                         << data.totalMonthMingen[realmonth] << " MW in month " << month + 1
                         << " of TS-" << tsIndex + 1 << " is incompatible with the inflows of "
                         << data.totalMonthInflows[realmonth] << " MW.";
            return false;
        }
    }
    return true;
}

bool HydroManagement::checkYearlyMinGeneration(uint numSpace, uint tsIndex, const Data::Area& area) const
{
    const auto& data = tmpDataByArea_[numSpace][area.index];
    if (data.totalYearMingen > data.totalYearInflows)
    {
        // Yearly minimum generation <= Yearly inflows
        logs.error() << "In Area " << area.name << " the minimum generation of "
                     << data.totalYearMingen << " MW of TS-" << tsIndex + 1
                     << " is incompatible with the inflows of " << data.totalYearInflows << " MW.";
        return false;
    }
    return true;
}

bool HydroManagement::checkWeeklyMinGeneration(uint tsIndex, Data::Area& area) const
{
    auto& inflowsmatrix = area.hydro.series->storage;
    auto& mingenmatrix = area.hydro.series->mingen;
    auto const& srcinflows = inflowsmatrix[tsIndex < inflowsmatrix.width ? tsIndex : 0];
    auto const& srcmingen = mingenmatrix[tsIndex < mingenmatrix.width ? tsIndex : 0];
    // Weekly minimum generation <= Weekly inflows for each week
    for (uint week = 0; week < calendar_.maxWeeksInYear - 1; ++week)
    {
        double totalWeekMingen = 0.0;
        double totalWeekInflows = 0.0;
        for (uint hour = calendar_.weeks[week].hours.first;
             hour < calendar_.weeks[week].hours.end && hour < HOURS_PER_YEAR;
             ++hour)
        {
            totalWeekMingen += srcmingen[hour];
        }

        for (uint day = calendar_.weeks[week].daysYear.first;
             day < calendar_.weeks[week].daysYear.end;
             ++day)
        {
            totalWeekInflows += srcinflows[day];
        }
        if (totalWeekMingen > totalWeekInflows)
        {
            logs.error() << "In Area " << area.name << " the minimum generation of "
                         << totalWeekMingen << " MW in week " << week + 1 << " of TS-"
                         << tsIndex + 1 << " is incompatible with the inflows of "
                         << totalWeekInflows << " MW.";
            return false;
        }
    }
    return true;
}

bool HydroManagement::checkHourlyMinMaxGeneration(uint tsIndex,
                                                  uint tsIndexMaxPower,
                                                  Data::Area& area) const
{
    // Hourly minimum generation <= hourly inflows for each hour
    auto& mingenmatrix = area.hydro.series->mingen;
    auto const& srcmingen = mingenmatrix[tsIndex < mingenmatrix.width ? tsIndex : 0];
    auto& maxgenmatrix = area.hydro.series->maxgen;
    auto const& srcmaxgen
      = maxgenmatrix[tsIndexMaxPower < maxgenmatrix.width ? tsIndexMaxPower : 0];

    for (uint h = 0; h < HOURS_PER_YEAR; ++h)
    {
        const auto& max = srcmaxgen[h];
        const auto& min = srcmingen[h];

        if (max < min)
        {
            logs.error() << "In area: " << area.name << " [hourly] minimum generation of " << min
                         << " MW in timestep " << h + 1 << " of TS-" << tsIndex + 1
                         << " is incompatible with the maximum generation of " << max
                         << " MW in timestep " << h + 1 << " of TS-" << tsIndexMaxPower + 1
                         << " MW.";
            return false;
        }
    }

    return true;
}

bool HydroManagement::checkMinGeneration(uint numSpace)
{
    bool ret = true;
    areas_.each([this, &numSpace, &ret](Data::Area& area)
    {
        uint z = area.index;
        const auto& ptchro = NumeroChroniquesTireesParPays[numSpace][z];
        auto tsIndex = (uint)ptchro.Hydraulique;
        auto tsIndexMaxPower = ptchro.HydrauliqueMaxPower;

        bool useHeuristicTarget = area.hydro.useHeuristicTarget;
        bool followLoadModulations = area.hydro.followLoadModulations;
        bool reservoirManagement = area.hydro.reservoirManagement;

        if (!reservoirManagement)
            ret = checkHourlyMinMaxGeneration(tsIndex, tsIndexMaxPower, area) && ret;

        if (!useHeuristicTarget)
            return;

        if (!followLoadModulations)
        {
            ret = checkWeeklyMinGeneration(tsIndex, area) && ret;
            return;
        }
               
        if (reservoirManagement)
            ret = checkYearlyMinGeneration(numSpace, tsIndex, area) && ret;
        else
            ret = checkMonthlyMinGeneration(numSpace, tsIndex, area) && ret;

    });
    return ret;
}

template<enum Data::StudyMode ModeT>
void HydroManagement::prepareNetDemand(uint numSpace)
{
    areas_.each([&](Data::Area& area) {
        uint z = area.index;

        auto& scratchpad = area.scratchpad[numSpace];

        auto& ptchro = NumeroChroniquesTireesParPays[numSpace][z];

        auto& rormatrix = area.hydro.series->ror;
        auto tsIndex = (uint)ptchro.Hydraulique;
        auto& ror = rormatrix[tsIndex < rormatrix.width ? tsIndex : 0];

        auto& data = tmpDataByArea_[numSpace][z];

        for (uint hour = 0; hour != HOURS_PER_YEAR; ++hour)
        {
            auto dayYear = calendar_.hours[hour].dayYear;

            double netdemand = 0;

            // Aggregated renewable production: wind & solar
            if (parameters_.renewableGeneration.isAggregated())
            {
                netdemand = +scratchpad.ts.load[ptchro.Consommation][hour]
                            - scratchpad.ts.wind[ptchro.Eolien][hour] - scratchpad.miscGenSum[hour]
                            - scratchpad.ts.solar[ptchro.Solar][hour] - ror[hour]
                            - ((ModeT != Data::stdmAdequacy) ? scratchpad.mustrunSum[hour]
                                                             : scratchpad.originalMustrunSum[hour]);
            }

            // Renewable clusters, if enabled
            else if (parameters_.renewableGeneration.isClusters())
            {
                netdemand = scratchpad.ts.load[ptchro.Consommation][hour]
                            - scratchpad.miscGenSum[hour] - ror[hour]
                            - ((ModeT != Data::stdmAdequacy) ? scratchpad.mustrunSum[hour]
                                                             : scratchpad.originalMustrunSum[hour]);

                area.renewable.list.each([&](const Antares::Data::RenewableCluster& cluster) {
                    assert(cluster.series->timeSeries.jit == NULL && "No JIT data from the solver");
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
    areas_.each([&](Data::Area& area) {
        auto z = area.index;

        auto& data = tmpDataByArea_[numSpace][z];

        for (uint day = 0; day != 365; ++day)
        {
            auto month = calendar_.days[day].month;
            assert(month < 12 && "Invalid month index");
            auto realmonth = calendar_.months[month].realmonth;

            double effectiveDemand = 0;
            area.hydro.allocation.eachNonNull([&](unsigned areaindex, double value) {
                effectiveDemand += (tmpDataByArea_[numSpace][areaindex]).DLN[day] * value;
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
            auto daysPerMonth = calendar_.months[month].days;
            auto realmonth = calendar_.months[month].realmonth;

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

void HydroManagement::makeVentilation(double* randomReservoirLevel,
                                      Solver::Variable::State& state,
                                      uint y,
                                      uint numSpace)
{
    memset(tmpDataByArea_[numSpace], 0, sizeof(TmpDataByArea) * areas_.size());

    prepareInflowsScaling(numSpace);
    minGenerationScaling(numSpace);
    if (!checkMinGeneration(numSpace))
    {
        throw FatalError("hydro management: invalid minimum generation");
    }

    if (parameters_.adequacy())
        prepareNetDemand<Data::stdmAdequacy>(numSpace);
    else
        prepareNetDemand<Data::stdmEconomy>(numSpace);

    prepareEffectiveDemand(numSpace);

    prepareMonthlyOptimalGenerations(randomReservoirLevel, y, numSpace);
    prepareDailyOptimalGenerations(state, y, numSpace);
}

} // namespace Antares
