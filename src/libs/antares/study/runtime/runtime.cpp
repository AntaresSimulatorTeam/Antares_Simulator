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

#include "runtime.h"
#include "antares/fatal-error.h"

#include "../area/scratchpad.h"

using namespace Yuni;

namespace Antares::Data
{
static void StudyRuntimeInfosInitializeAllAreas(Study& study, StudyRuntimeInfos& r)
{
    uint areaCount = study.areas.size();
    uint nbYearsInParallel = study.maxNbYearsInParallel;

    // For each area
    for (uint a = 0; a != areaCount; ++a)
    {
        // alias to the current area
        auto& area = *(study.areas.byIndex[a]);

        // Precache allocation correlation coefficients
        area.hydro.allocation.prepareForSolver(study.areas);

        // Hydro TS Generator: log(expectation) ; log(stddeviation)
        if (area.hydro.prepro)
        {
            auto& m = area.hydro.prepro->data;
            auto& e = m[PreproHydro::expectation];
            auto& s = m[PreproHydro::stdDeviation];
            double nE, nS;
            for (uint i = 0; i != 12; ++i)
            {
                if (!Math::Zero(e[i]))
                {
                    // E' = ln(e) - 0.5 * ln(1 + (s*s) / (e*e))
                    // S' = sqrt(ln(1 + (s*s) / (e*e)))
                    nE = log(e[i]) - 0.5 * log(1. + (s[i] * s[i]) / (e[i] * e[i]));
                    nS = sqrt(log(1. + (s[i] * s[i]) / (e[i] * e[i])));

                    // asserts
                    assert(!Math::NaN(nE)
                           && "Hydro: NaN value detected for hydro prepro expectation");
                    assert(!Math::NaN(nS)
                           && "Hydro: NaN value detected for hydro prepro expectation");

                    e[i] = nE;
                    s[i] = nS;
                }
                else
                {
                    e[i] = 0.;
                    s[i] = 0.;
                }
            }
        }

        // Spinning - Economic Only - If no prepro
        if (!(timeSeriesThermal & study.parameters.timeSeriesToRefresh))
        {
            // Calculation of the spinning
            area.thermal.list.calculationOfSpinning();
            // We should not forget the list of clusters in 'must-run' mode
            area.thermal.mustrunList.calculationOfSpinning();
        }

        area.scratchpad.reserve(nbYearsInParallel);
        for (uint numSpace = 0; numSpace < nbYearsInParallel; numSpace++)
            area.scratchpad.emplace_back(r, area);

        // statistics
        r.thermalPlantTotalCount += area.thermal.list.size();
        r.thermalPlantTotalCountMustRun += area.thermal.mustrunList.size();

        r.shortTermStorageCount += area.shortTermStorage.count();
    }
}

static void StudyRuntimeInfosInitializeAreaLinks(Study& study, StudyRuntimeInfos& r)
{
    r.areaLink.resize(study.areas.areaLinkCount());

    uint indx = 0;

    study.areas.each([&](Data::Area& area) {
        area.buildLinksIndexes();

        auto end = area.links.end();
        for (auto i = area.links.begin(); i != end; ++i)
        {
            auto* link = i->second;

            r.areaLink[indx] = link;
            link->index = indx;
            ++indx;
        }
    });
}

void StudyRuntimeInfos::initializeRangeLimits(const Study& study, StudyRangeLimits& limits)
{
    // Hour
    uint a = study.calendar.days[study.parameters.simulationDays.first].hours.first;
    uint b = study.calendar.days[study.parameters.simulationDays.end - 1].hours.end - 1;

    if (b < a) // normalize
    {
        // This should never happen but...
        b = a;
    }
    else
    {
        // In Economy mode, we must deal with an integral number of weeks
        // A week : 168 hours
        if ((b - a + 1) % 168)
        {
            // We have here too much hours, the interval will be reduced
            // Log Entry
            logs.info() << "    Partial week detected. Not allowed in "
                        << SimulationModeToCString(study.parameters.mode);
            logs.info() << "    Time interval that has been requested: " << (1 + a) << ".."
                        << (1 + b);
            // Reducing
            while (b > a and 0 != ((b - a + 1) % 168))
                --b;
        }
    }

    // Getting informations about the given hours
    auto& ca = study.calendar.hours[a];
    auto& cb = study.calendar.hours[b];

    assert(ca.dayYear < 400 and "Trivial check failed");
    assert(cb.dayYear < 400 and "Trivial check failed");

    // Hour
    limits.hour[rangeBegin] = a;
    limits.hour[rangeEnd] = b;
    limits.hour[rangeCount] = b - a + 1;
    // Day
    limits.day[rangeBegin] = (uint)ca.dayYear;
    limits.day[rangeEnd] = (uint)cb.dayYear;
    limits.day[rangeCount] = limits.day[rangeEnd] - limits.day[rangeBegin] + 1;
    // week
    limits.week[rangeBegin] = (uint)ca.week;
    limits.week[rangeEnd] = (uint)cb.week;
    limits.week[rangeCount] = limits.week[rangeEnd] - limits.week[rangeBegin] + 1;
    // month
    limits.month[rangeBegin] = (uint)ca.month;
    limits.month[rangeEnd] = (uint)cb.month;
    limits.month[rangeCount] = limits.month[rangeEnd] - limits.month[rangeBegin] + 1;
    // year
    limits.year[rangeBegin] = 0;
    limits.year[rangeEnd] = study.parameters.nbYears - 1;
    limits.year[rangeCount] = study.parameters.effectiveNbYears;

    limits.checkIntegrity();

    // Logs
    // Example (extracted from the logs)
    // Calendar: hours:1..8736, days:1..364, weeks:1..52, months:1..12, years:1..20
    // In memory : (-1)
    // Calendar: hours:0..8735, days:0..363, weeks:0..51, months:0..11, years:0..19
    //
    logs.info() << "Calendar: hours:" << (1 + a) << ".." << (1 + b)
                << ", days:" << (1 + limits.day[rangeBegin]) << ".." << (1 + limits.day[rangeEnd])
                << ", weeks:" << (1 + limits.week[rangeBegin]) << ".."
                << (1 + limits.week[rangeEnd]) << ", months:" << (1 + limits.month[rangeBegin])
                << ".." << (1 + limits.month[rangeEnd])
                << ", years:" << (1 + limits.year[rangeBegin]) << ".."
                << (1 + limits.year[rangeEnd]);
    if (study.parameters.leapYear)
        logs.info() << "leap year: enabled";

    // Number of simulation days per month
    for (uint i = 0; i != 12; ++i)
        simulationDaysPerMonth[i] = 0;
    if (ca.month == cb.month)
    {
        simulationDaysPerMonth[(uint)ca.month] = (uint)(cb.dayYear - ca.dayYear + 1);
        if (simulationDaysPerMonth[(uint)ca.month] > study.calendar.months[(uint)ca.month].days)
        {
            throw FatalError("Internal error when preparing the calendar");
        }
    }
    else
    {
        simulationDaysPerMonth[(uint)ca.month]
          = study.calendar.months[(uint)ca.month].days - ca.dayMonth;
        simulationDaysPerMonth[(uint)cb.month] = cb.dayMonth + 1;
        for (uint i = ca.month + 1; i < cb.month; ++i)
            simulationDaysPerMonth[i] = study.calendar.months[i].days;
    }
    {
        CString<50, false> s;
        for (uint i = 0; i != 12; ++i)
        {
            if (i)
                s << ", ";
            s << simulationDaysPerMonth[i];
        }
        logs.info() << "Simulation days per month : " << s;
    }

    // Number of simulation days per week
    for (uint i = 0; i != 53; ++i)
        simulationDaysPerWeek[i] = 0;

    for (uint d = limits.day[rangeBegin]; d <= limits.day[rangeEnd]; d++)
        simulationDaysPerWeek[study.calendar.days[d].week]++;

    // We make the test on the field 'hour' because the field 'week' might be equals to 0
    // (Example: 1 week: from 0 to 0 and it is valid)
    // As the number of hours has already been normalized to stick to a integral number of
    // weeks, this value must be greater than or equal to 168
    if (limits.hour[rangeCount] < 168)
    {
        throw FatalError("At least one week is required to run a simulation.");
    }
}

StudyRuntimeInfos::StudyRuntimeInfos() :
    nbYears(0),
    thermalPlantTotalCount(0),
    thermalPlantTotalCountMustRun(0),
    quadraticOptimizationHasFailed(false)
{
}

void StudyRuntimeInfos::checkThermalTSGeneration(Study& study)
{
    const auto& gd = study.parameters;
    bool globalThermalTSgeneration = gd.timeSeriesToGenerate & timeSeriesThermal;
    thermalTSRefresh = globalThermalTSgeneration;

    study.areas.each([this, globalThermalTSgeneration](Data::Area& area) {
        area.thermal.list.each(
          [this, globalThermalTSgeneration](const Data::ThermalCluster& cluster) {
              thermalTSRefresh
                = thermalTSRefresh || cluster.doWeGenerateTS(globalThermalTSgeneration);
          });
    });
}

bool StudyRuntimeInfos::loadFromStudy(Study& study)
{
    auto& gd = study.parameters;

    nbYears = gd.nbYears;
    mode = gd.mode;
    thermalPlantTotalCount = 0;
    thermalPlantTotalCountMustRun = 0;
    // Calendar
    logs.info() << "Generating calendar informations";
    if (study.usedByTheSolver)
    {
        study.calendar.reset({gd.dayOfThe1stJanuary, gd.firstWeekday, gd.firstMonthInYear, false});
    }
    else
    {
        study.calendar.reset({gd.dayOfThe1stJanuary, gd.firstWeekday, gd.firstMonthInYear, gd.leapYear});
    }
    logs.debug() << "  :: generating calendar dedicated to the output";
    study.calendarOutput.reset({gd.dayOfThe1stJanuary, gd.firstWeekday, gd.firstMonthInYear, gd.leapYear});
    initializeRangeLimits(study, rangeLimits);

    // Removing disabled thermal clusters from solver computations
    removeDisabledThermalClustersFromSolverComputations(study);

    switch (gd.renewableGeneration())
    {
    case rgClusters:
        // Removing disabled renewable clusters from solver computations
        removeDisabledRenewableClustersFromSolverComputations(study);
        break;
    case rgAggregated:
        // Removing all renewable clusters from solver computations
        removeAllRenewableClustersFromSolverComputations(study);
        break;
    case rgUnknown:
    default:
        logs.warning() << "Invalid value for renewable generation";
        break;
    }

    // Must-run mode
    initializeThermalClustersInMustRunMode(study);

    // Areas
    StudyRuntimeInfosInitializeAllAreas(study, *this);

    // Area links
    StudyRuntimeInfosInitializeAreaLinks(study, *this);

    // Check if some clusters request TS generation
    checkThermalTSGeneration(study);

    if (not gd.geographicTrimming)
        disableAllFilters(study);

    logs.info();
    logs.info() << "Summary";
    logs.info() << "     areas: " << study.areas.size();
    logs.info() << "     links: " << interconnectionsCount();
    logs.info() << "     thermal clusters: " << thermalPlantTotalCount;
    logs.info() << "     thermal clusters (must-run): " << thermalPlantTotalCountMustRun;
    logs.info() << "     short-term storages: " << shortTermStorageCount;
    logs.info() << "     binding constraints: " << study.bindingConstraints.activeContraints().size();
    logs.info() << "     geographic trimming:" << (gd.geographicTrimming ? "true" : "false");
    logs.info() << "     memory : " << ((study.memoryUsage()) / 1024 / 1024) << "Mo";
    logs.info();

    return true;
}

uint StudyRuntimeInfos::interconnectionsCount() const
{
    return static_cast<uint>(areaLink.size());
}

void StudyRuntimeInfos::initializeThermalClustersInMustRunMode(Study& study) const
{
    logs.info();
    logs.info() << "Optimizing the thermal clusters in 'must-run' mode...";

    // The number of thermal clusters in 'must-run' mode
    uint count = 0;

    // each area...
    for (uint a = 0; a != study.areas.size(); ++a)
    {
        Area& area = *(study.areas.byIndex[a]);
        area.thermal.prepareAreaWideIndexes();
        count += area.thermal.prepareClustersInMustRunMode();
    }

    switch (count)
    {
    case 0:
        logs.info() << "No thermal cluster in 'must-run' mode";
        break;
    case 1:
        logs.info() << "Found 1 thermal cluster in 'must-run' mode";
        break;
    default:
        logs.info() << "Found " << count << " thermal clusters in 'must-run' mode";
    }
    // space
    logs.info();
}

static void removeClusters(Study& study,
                           const char* type,
                           std::function<uint(Area&)> eachArea,
                           bool verbose = true)
{
    if (verbose)
    {
        logs.info();
        logs.info() << "Removing disabled " << type << " clusters in from solver computations...";
    }
    uint count = 0;
    // each area...
    for (uint a = 0; a != study.areas.size(); ++a)
    {
        Area& area = *(study.areas.byIndex[a]);
        count += eachArea(area);
    }

    if (verbose)
    {
        switch (count)
        {
        case 0:
            logs.info() << "No disabled " << type << " cluster removed before solver computations";
            break;
        default:
            logs.info() << "Found " << count << " disabled " << type
                        << " clusters and removed them before solver computations";
        }
    }
}

void StudyRuntimeInfos::removeDisabledThermalClustersFromSolverComputations(Study& study)
{
    removeClusters(
      study, "thermal", [](Area& area) { return area.thermal.removeDisabledClusters(); });
}

void StudyRuntimeInfos::removeDisabledRenewableClustersFromSolverComputations(Study& study)
{
    removeClusters(study, "renewable", [](Area& area) {
        uint ret = area.renewable.removeDisabledClusters();
        if (ret > 0)
            area.renewable.prepareAreaWideIndexes();
        return ret;
    });
}

void StudyRuntimeInfos::removeAllRenewableClustersFromSolverComputations(Study& study)
{
    removeClusters(
      study,
      "renewable",
      [](Area& area) {
          area.renewable.reset();
          return 0;
      },
      false);
}

StudyRuntimeInfos::~StudyRuntimeInfos()
{
    logs.debug() << "Releasing runtime data";
}

#ifndef NDEBUG
void StudyRangeLimits::checkIntegrity() const
{
    assert(this != nullptr);
    assert(hour[rangeBegin] <= hour[rangeEnd]);
    assert(day[rangeBegin] <= day[rangeEnd]);
    assert(hour[rangeBegin] < 9000); // arbitrary value
    assert(hour[rangeEnd] < 9000);   // arbitrary value
    assert(day[rangeBegin] < 367);
    assert(day[rangeEnd] < 367);
}
#endif

void StudyRuntimeInfos::disableAllFilters(Study& study)
{
    study.areas.each([&](Data::Area& area) {
        area.filterSynthesis = filterAll;
        area.filterYearByYear = filterAll;

        auto end = area.links.end();
        for (auto i = area.links.begin(); i != end; ++i)
        {
            auto& link = *(i->second);
            link.filterSynthesis = filterAll;
            link.filterYearByYear = filterAll;
        }
    });
}

} // namespace Antares
