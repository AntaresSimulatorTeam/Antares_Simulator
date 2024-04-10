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

#include <yuni/yuni.h>
#include <antares/study/study.h>
#include <antares/study/area/scratchpad.h>
#include <antares/antares/fatal-error.h>
#include "antares/solver/hydro/management/management.h"
#include "antares/solver/simulation/sim_extern_variables_globales.h"
#include <limits>
#include <antares/study/parts/hydro/container.h>
#include <numeric>
#include <cmath>

namespace Antares
{
namespace Solver
{

double randomReservoirLevel(double min, double avg, double max, MersenneTwister& random)
{
    auto equals = [] (double a, double b) { return std::abs(a - b) < 1.e-6; };

    if (equals(min, max))
        return avg;
    if (equals(avg, min) || equals(avg, max))
        return avg;

    double e = (avg - min) / (max - min);
    double re = 1. - e;

    assert(std::abs(1. + e) > 1e-12);
    assert(std::abs(2. - e) > 1e-12);

    double v1 = (e * e) * re / (1. + e);
    double v2 = e * re * re / (2. - e);
    double v = std::min(v1, v2) * .5;

    assert(std::abs(v) > 1e-12);

    double a = e * (e * re / v - 1.);
    double b = re * (e * re / v - 1.);

    double x = BetaVariable(a, b, random);
    return x * max + (1. - x) * min;
}

double GammaVariable(double r, MersenneTwister &random)
{
    double x = 0.;
    do
    {
        double s = r - 1.;
        double u = random();
        double v = random();
        double w = u * (1. - u);
        assert(std::abs(w) > 1e-12);
        assert(3. * (r - 0.25) / w > 0.);
        double y = std::sqrt(3. * (r - 0.25) / w) * (u - 0.5);

        x = y + s;
        if (v < 1e-12)
            break;

        w *= 4.;
        v *= w;
        double z = w * v * v;

        assert(std::abs(s) > 1e-12);
        assert(z > 0.);
        assert(z / s > 0.);
        if (log(z) <= 2. * (s * log(x / s) - y))
            break;
    } while (true);
    return x;
}

double BetaVariable(double a, double b, MersenneTwister &random)
{
    double y = GammaVariable(a, random);
    double z = GammaVariable(b, random);
    assert(std::abs(y + z) > 1e-12);
    return y / (y + z);
}

} // namespace Solver

HydroManagement::HydroManagement(const Data::AreaList& areas,
                                 const Data::Parameters& params,
                                 const Date::Calendar& calendar,
                                 unsigned int maxNbYearsInParallel,
                                 Solver::IResultWriter& resultWriter) :
    areas_(areas),
    calendar_(calendar),
    parameters_(params),
    maxNbYearsInParallel_(maxNbYearsInParallel),
    resultWriter_(resultWriter)
{
    // Ventilation results memory allocation
    uint nbDaysPerYear = 365;
    ventilationResults_.resize(areas_.size());
    for (uint areaIndex = 0; areaIndex < areas_.size(); ++areaIndex)
    {
        auto* area = areas_.byIndex[areaIndex];
        auto& ventilationResults = ventilationResults_[areaIndex];

        ventilationResults.HydrauliqueModulableQuotidien.assign(nbDaysPerYear, 0);
        if (area->hydro.reservoirManagement)
        {
            ventilationResults.NiveauxReservoirsDebutJours.assign(nbDaysPerYear, 0.);
            ventilationResults.NiveauxReservoirsFinJours.assign(nbDaysPerYear, 0.);
        }
    }
}

void HydroManagement::prepareInflowsScaling(uint year)
{
    areas_.each([&](const Data::Area& area)
      {
          auto const& srcinflows = area.hydro.series->storage.getColumn(year);

          auto& data = tmpDataByArea_[&area];
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
                      assert(!std::isnan(data.inflows[month]) && "nan value detect in inflows");
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

void HydroManagement::minGenerationScaling(uint year)
{
    areas_.each([this, &year](const Data::Area& area)
      {
          auto const& srcmingen =  area.hydro.series->mingen.getColumn(year);

          auto& data = tmpDataByArea_[&area];
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
                      assert(!std::isnan(data.mingens[month]) && "nan value detect in mingen");
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

bool HydroManagement::checkMonthlyMinGeneration(uint year, const Data::Area& area) const
{
    const auto& data = tmpDataByArea_.at(&area);
    for (uint month = 0; month != 12; ++month)
    {
        uint realmonth = calendar_.months[month].realmonth;
        // Monthly minimum generation <= Monthly inflows for each month
        if (data.totalMonthMingen[realmonth] > data.totalMonthInflows[realmonth])
        {
            logs.error() << "In Area " << area.name << " the minimum generation of "
                         << data.totalMonthMingen[realmonth] << " MW in month " << month + 1
                         << " of TS-" << area.hydro.series->mingen.getSeriesIndex(year) + 1
                         << " is incompatible with the inflows of "
                         << data.totalMonthInflows[realmonth] << " MW.";
            return false;
        }
    }
    return true;
}

bool HydroManagement::checkYearlyMinGeneration(uint year, const Data::Area& area) const
{
    const auto& data = tmpDataByArea_.at(&area);
    if (data.totalYearMingen > data.totalYearInflows)
    {
        // Yearly minimum generation <= Yearly inflows
        logs.error() << "In Area " << area.name << " the minimum generation of "
                     << data.totalYearMingen << " MW of TS-"
                     << area.hydro.series->mingen.getSeriesIndex(year) + 1
                     << " is incompatible with the inflows of " << data.totalYearInflows << " MW.";
        return false;
    }
    return true;
}

bool HydroManagement::checkWeeklyMinGeneration(uint year, const Data::Area& area) const
{
    auto const& srcinflows =  area.hydro.series->storage.getColumn(year);
    auto const& srcmingen = area.hydro.series->mingen.getColumn(year);
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
                         << area.hydro.series->mingen.getSeriesIndex(year) + 1
                         << " is incompatible with the inflows of "
                         << totalWeekInflows << " MW.";
            return false;
        }
    }
    return true;
}

bool HydroManagement::checkGenerationPowerConsistency(uint year) const
{
    bool ret = true;

    areas_.each(
      [&ret, &year](const Data::Area& area)
      {

          auto const& srcmingen = area.hydro.series->mingen.getColumn(year);
          auto const& srcmaxgen = area.hydro.series->maxHourlyGenPower.getColumn(year);

          uint const tsIndexMin = area.hydro.series->mingen.getSeriesIndex(year);
          uint const tsIndexMax = area.hydro.series->maxHourlyGenPower.getSeriesIndex(year);

          for (uint h = 0; h < HOURS_PER_YEAR; ++h)
          {
              const auto& min = srcmingen[h];
              const auto& max = srcmaxgen[h];

              if (max < min)
              {
                  logs.error() << "In area: " << area.name << " [hourly] minimum generation of "
                               << min << " MW in timestep " << h + 1 << " of TS-" << tsIndexMin + 1
                               << " is incompatible with the maximum generation of " << max
                               << " MW in timestep " << h + 1 << " of TS-" << tsIndexMax + 1 << " MW.";
                  ret = false;
                  return;
              }
          }
      });

    return ret;
}

bool HydroManagement::checkMinGeneration(uint year) const
{
    bool ret = true;
    areas_.each([this, &ret, &year](const Data::Area& area)
    {
        bool useHeuristicTarget = area.hydro.useHeuristicTarget;
        bool followLoadModulations = area.hydro.followLoadModulations;
        bool reservoirManagement = area.hydro.reservoirManagement;

        if (!useHeuristicTarget)
            return;

        if (!followLoadModulations)
        {
            ret = checkWeeklyMinGeneration(year, area) && ret;
            return;
        }

        if (reservoirManagement)
            ret = checkYearlyMinGeneration(year, area) && ret;
        else
            ret = checkMonthlyMinGeneration(year, area) && ret;
    });
    return ret;
}

void HydroManagement::changeInflowsToAccommodateFinalLevels(uint year)
{
    areas_.each([this, &year](Data::Area& area) 
    {
        auto& data = tmpDataByArea_[&area];

        if (!area.hydro.finalLevelInflowsModifier.isApplicable(year))
            return;

        // Must be done before prepareMonthlyTargetGenerations
        double delta = area.hydro.finalLevelInflowsModifier.deltaLevel[year];
        if (delta > 0)
            data.inflows[0] += delta;
        else if (delta < 0)
            data.inflows[11] += delta;
    });
}

void HydroManagement::prepareNetDemand(uint year, Data::SimulationMode mode,
                                       const Antares::Data::Area::ScratchMap& scratchmap)
{
    areas_.each([this, &year, &scratchmap, &mode](const Data::Area& area) {
        const auto& scratchpad = scratchmap.at(&area);

        const auto& rormatrix = area.hydro.series->ror;
        const auto* ror = rormatrix.getColumn(year);

        auto& data = tmpDataByArea_[&area];
        const double* loadSeries = area.load.series.getColumn(year);
        const double* windSeries = area.wind.series.getColumn(year);
        const double* solarSeries = area.solar.series.getColumn(year);

        for (uint hour = 0; hour != HOURS_PER_YEAR; ++hour)
        {
            auto dayYear = calendar_.hours[hour].dayYear;

            double netdemand = 0;

            // Aggregated renewable production: wind & solar
            if (parameters_.renewableGeneration.isAggregated())
            {
                netdemand = + loadSeries[hour]
                            - windSeries[hour] - scratchpad.miscGenSum[hour]
                            - solarSeries[hour] - ror[hour]
                            - ((mode != Data::SimulationMode::Adequacy) ? scratchpad.mustrunSum[hour]
                                                             : scratchpad.originalMustrunSum[hour]);
            }

            // Renewable clusters, if enabled
            else if (parameters_.renewableGeneration.isClusters())
            {
                netdemand = loadSeries[hour]
                            - scratchpad.miscGenSum[hour] - ror[hour]
                            - ((mode != Data::SimulationMode::Adequacy) ? scratchpad.mustrunSum[hour]
                                                             : scratchpad.originalMustrunSum[hour]);

                for (auto& c : area.renewable.list.each_enabled())
                    netdemand -= c->valueAtTimeStep(year, hour);
            }

            assert(!std::isnan(netdemand)
                   && "hydro management: NaN detected when calculating the net demande");
            data.DLN[dayYear] += netdemand;
        }
    });
}

void HydroManagement::prepareEffectiveDemand()
{
    areas_.each([&](Data::Area& area) {
        auto& data = tmpDataByArea_[&area];

        for (uint day = 0; day != 365; ++day)
        {
            auto month = calendar_.days[day].month;
            assert(month < 12 && "Invalid month index");
            auto realmonth = calendar_.months[month].realmonth;

            double effectiveDemand = 0;
            // area.hydro.allocation is indexed by area index
            area.hydro.allocation.eachNonNull([&](unsigned areaIndex, double value) {
                const auto* area = areas_.byIndex[areaIndex];
                effectiveDemand += tmpDataByArea_[area].DLN[day] * value;
            });

            assert(!std::isnan(effectiveDemand) && "nan value detected for effectiveDemand");
            data.DLE[day] += effectiveDemand;
            data.MLE[realmonth] += effectiveDemand;

            assert(not std::isnan(data.DLE[day]) && "nan value detected for DLE");
            assert(not std::isnan(data.MLE[realmonth]) && "nan value detected for DLE");
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

bool HydroManagement::checksOnGenerationPowerBounds(uint year) const
{
    return (checkMinGeneration(year) && checkGenerationPowerConsistency(year)) ? true : false;
}

void HydroManagement::makeVentilation(double* randomReservoirLevel,
                                      Solver::Variable::State& state,
                                      uint y,
                                      Antares::Data::Area::ScratchMap& scratchmap)
{
    prepareInflowsScaling(y);
    minGenerationScaling(y);
    if (!checksOnGenerationPowerBounds(y))
    {
        throw FatalError("hydro management: invalid minimum generation");
    }

    changeInflowsToAccommodateFinalLevels(y);
    prepareNetDemand(y, parameters_.mode, scratchmap);
    prepareEffectiveDemand();

    prepareMonthlyOptimalGenerations(randomReservoirLevel, y);
    prepareDailyOptimalGenerations(state, y, scratchmap);
}

} // namespace Antares
