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

#include "antares/solver/hydro/management/management.h"

#include <cmath>
#include <limits>

#include <antares/antares/fatal-error.h>
#include <antares/study/area/scratchpad.h>
#include <antares/study/parts/hydro/container.h>
#include <antares/study/study.h>

namespace Antares
{
namespace Solver
{

double randomReservoirLevel(double min, double avg, double max, MersenneTwister& random)
{
    auto equals = [](double a, double b) { return std::abs(a - b) < 1.e-6; };

    if (equals(min, max))
    {
        return avg;
    }
    if (equals(avg, min) || equals(avg, max))
    {
        return avg;
    }

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

double GammaVariable(double r, MersenneTwister& random)
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
        {
            break;
        }

        w *= 4.;
        v *= w;
        double z = w * v * v;

        assert(std::abs(s) > 1e-12);
        assert(z > 0.);
        assert(z / s > 0.);
        if (log(z) <= 2. * (s * log(x / s) - y))
        {
            break;
        }
    } while (true);
    return x;
}

double BetaVariable(double a, double b, MersenneTwister& random)
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
                                 Solver::IResultWriter& resultWriter):
    areas_(areas),
    calendar_(calendar),
    parameters_(params),
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

void HydroManagement::prepareNetDemand(uint year,
                                       Data::SimulationMode mode,
                                       const Antares::Data::Area::ScratchMap& scratchmap,
                                       HydroSpecificMap& hydro_specific_map)
{
    areas_.each(
      [this, &year, &scratchmap, &mode, &hydro_specific_map](Data::Area& area)
      {
          const auto& scratchpad = scratchmap.at(&area);

          const auto& rormatrix = area.hydro.series->ror;
          const auto* ror = rormatrix.getColumn(year);

          auto& data = area.hydro.managementData[year];
          auto& hydro_specific = hydro_specific_map[&area];
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
                  netdemand = +loadSeries[hour] - windSeries[hour] - scratchpad.miscGenSum[hour]
                              - solarSeries[hour] - ror[hour]
                              - ((mode != Data::SimulationMode::Adequacy)
                                   ? scratchpad.mustrunSum[hour]
                                   : scratchpad.originalMustrunSum[hour]);
              }

              // Renewable clusters, if enabled
              else if (parameters_.renewableGeneration.isClusters())
              {
                  netdemand = loadSeries[hour] - scratchpad.miscGenSum[hour] - ror[hour]
                              - ((mode != Data::SimulationMode::Adequacy)
                                   ? scratchpad.mustrunSum[hour]
                                   : scratchpad.originalMustrunSum[hour]);

                  for (auto& c: area.renewable.list.each_enabled())
                  {
                      netdemand -= c->valueAtTimeStep(year, hour);
                  }
              }

              assert(!std::isnan(netdemand)
                     && "hydro management: NaN detected when calculating the net demande");
              hydro_specific.daily[dayYear].DLN += netdemand;
          }
      });
}

void HydroManagement::prepareEffectiveDemand(uint year, HydroSpecificMap& hydro_specific_map) const
{
    areas_.each(
      [this, &year, &hydro_specific_map](Data::Area& area)
      {
          auto& data = area.hydro.managementData[year];
          auto& hydro_specific = hydro_specific_map[&area];

          for (uint day = 0; day != 365; ++day)
          {
              auto month = calendar_.days[day].month;
              assert(month < 12 && "Invalid month index");
              auto realmonth = calendar_.months[month].realmonth;

              double effectiveDemand = 0;
              // area.hydro.allocation is indexed by area index
              area.hydro.allocation.eachNonNull(
                [this, &effectiveDemand, &day, &hydro_specific_map](unsigned areaIndex,
                                                                    double value)
                {
                    const auto* area = areas_.byIndex[areaIndex];
                    effectiveDemand += hydro_specific_map[area].daily[day].DLN * value;
                });

              assert(!std::isnan(effectiveDemand) && "nan value detected for effectiveDemand");
              hydro_specific.daily[day].DLE += effectiveDemand;
              hydro_specific.monthly[realmonth].MLE += effectiveDemand;

              assert(not std::isnan(hydro_specific.daily[day].DLE) && "nan value detected for DLE");
              assert(not std::isnan(hydro_specific.monthly[realmonth].MLE)
                     && "nan value detected for DLE");
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
                  if (hydro_specific.daily[dYear].DLE < minimumMonth)
                  {
                      minimumMonth = hydro_specific.daily[dYear].DLE;
                  }
              }

              if (minimumMonth < 0.)
              {
                  for (uint d = 0; d != daysPerMonth; ++d)
                  {
                      hydro_specific.daily[dayYear + d].DLE -= minimumMonth - 1e-4;
                  }
              }

              if (hydro_specific.monthly[realmonth].MLE < minimumYear)
              {
                  minimumYear = hydro_specific.monthly[realmonth].MLE;
              }

              dayYear += daysPerMonth;
          }

          if (minimumYear < 0.)
          {
              for (uint realmonth = 0; realmonth != 12; ++realmonth)
              {
                  hydro_specific.monthly[realmonth].MLE -= minimumYear - 1e-4;
              }
          }
      });
}

void HydroManagement::makeVentilation(double* randomReservoirLevel,
                                      uint y,
                                      Antares::Data::Area::ScratchMap& scratchmap)
{
    HydroSpecificMap hydro_specific_map;
    prepareNetDemand(y, parameters_.mode, scratchmap, hydro_specific_map);
    prepareEffectiveDemand(y, hydro_specific_map);

    prepareMonthlyOptimalGenerations(randomReservoirLevel, y, hydro_specific_map);
    prepareDailyOptimalGenerations(y, scratchmap, hydro_specific_map);
}

} // namespace Antares
