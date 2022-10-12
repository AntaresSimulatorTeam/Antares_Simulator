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

#include "../../sys/mem-wrapper.h"
#include "runtime.h"
#include "../parameters.h"
#include "../../date.h"
#include <algorithm>
#include <limits>
#include <functional>
#include "../../emergency.h"
#include "../memory-usage.h"
#include "../../config.h"
#include "../filter.h"
#include "../area/constants.h"
#include "../area/scratchpad.h"

using namespace Yuni;

namespace Antares
{
namespace Data
{
static void StudyRuntimeInfosInitializeAllAreas(Study& study, StudyRuntimeInfos& r)
{
    uint areaCount = study.areas.size();

    // For each area
    for (uint a = 0; a != areaCount; ++a)
    {
        // alias to the current area
        auto& area = *(study.areas.byIndex[a]);

        // Precache allocation correlation coefficients
        area.hydro.allocation.prepareForSolver(study.areas);

        // Creating runtime-data for the area

        // alias to the simulation mode
        auto mode = r.mode;

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
            // reduce the memory footprint
            m.flush();
        }

        // Spinning - Economic Only - If no prepro
        if (mode != stdmAdequacyDraft && !(timeSeriesThermal & r.parameters->timeSeriesToRefresh))
        {
            // Calculation of the spinning
            area.thermal.list.calculationOfSpinning();
            // We should not forget the list of clusters in 'must-run' mode
            area.thermal.mustrunList.calculationOfSpinning();
        }

        area.scratchpad = new AreaScratchpad*[area.nbYearsInParallel];
        for (uint numSpace = 0; numSpace < area.nbYearsInParallel; numSpace++)
            area.scratchpad[numSpace] = new AreaScratchpad(r, area);

        if (mode == Data::stdmAdequacy)
            area.reserves.flush();
        // reduce a bit the memory footprint
        area.miscGen.flush();

        // hydroHasMod
        if (mode != stdmAdequacyDraft)
        {
            if (!area.hydro.prepro) // not in prepro mode
                area.hydro.series->storage.flush();
            else
            {
                auto& m = area.hydro.prepro->data;
                // reduce memory footprint
                m.flush();
            }
        }

        area.reserves.flush();

        // statistics
        r.thermalPlantTotalCount += area.thermal.list.size();
        r.thermalPlantTotalCountMustRun += area.thermal.mustrunList.size();
    }
}

static void StudyRuntimeInfosInitializeAreaLinks(Study& study, StudyRuntimeInfos& r)
{
    r.interconnectionsCount = study.areas.areaLinkCount();
    using AreaLinkPointer = AreaLink*;
    r.areaLink = new AreaLinkPointer[r.interconnectionsCount];

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

template<enum BindingConstraint::Column C>
static void CopyBCData(BindingConstraintRTI& rti, const BindingConstraint& b)
{
    switch (C)
    {
    case BindingConstraint::columnInferior:
        rti.operatorType = '<';
        break;
    case BindingConstraint::columnSuperior:
        rti.operatorType = '>';
        break;
    case BindingConstraint::columnEquality:
        rti.operatorType = '=';
        break;
    case BindingConstraint::columnMax:
        rti.operatorType = '?';
        break;
    }
    logs.debug() << "copying constraint " << rti.operatorType << ' ' << b.name();
    rti.name = b.name().c_str();
    rti.filterYearByYear_ = b.yearByYearFilter();
    rti.filterSynthesis_ = b.synthesisFilter();
    rti.linkCount = b.linkCount();
    rti.clusterCount = b.enabledClusterCount();
    assert(rti.linkCount < 50000000 and "Seems a bit large...");    // arbitrary value
    assert(rti.clusterCount < 50000000 and "Seems a bit large..."); // arbitrary value
    rti.bounds.resize(1, b.matrix().height);
    rti.bounds.pasteToColumn(0, b.matrix()[C]);

    rti.linkWeight = new double[rti.linkCount];
    rti.linkOffset = new int[rti.linkCount];
    rti.linkIndex = new long[rti.linkCount];

    rti.clusterWeight = new double[rti.clusterCount];
    rti.clusterOffset = new int[rti.clusterCount];
    rti.clusterIndex = new long[rti.clusterCount];
    rti.clustersAreaIndex = new long[rti.clusterCount];

    b.initLinkArrays(rti.linkWeight,
                     rti.clusterWeight,
                     rti.linkOffset,
                     rti.clusterOffset,
                     rti.linkIndex,
                     rti.clusterIndex,
                     rti.clustersAreaIndex);

    // reduce the memory footprint
    rti.bounds.flush();
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
        if (stdmAdequacyDraft != study.parameters.mode)
        {
            // In Economy mode, we must deal with an integral number of weeks
            // A week : 168 hours
            if ((b - a + 1) % 168)
            {
                // We have here too much hours, the interval will be reduced
                // Log Entry
                logs.info() << "    Partial week detected. Not allowed in "
                            << StudyModeToCString(study.parameters.mode);
                logs.info() << "    Time interval that has been requested: " << (1 + a) << ".."
                            << (1 + b);
                // Reducing
                while (b > a and 0 != ((b - a + 1) % 168))
                    --b;
            }
        }
    }

    // Getting informations about the given hours
    auto& ca = study.calendar.hours[a]; // Antares::Date::StudyHourlyCalendar[a];
    auto& cb = study.calendar.hours[b]; // Antares::Date::StudyHourlyCalendar[b];

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
            logs.fatal() << "Internal error when preparing the calendar";
            AntaresSolverEmergencyShutdown(); // will never return
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
    if (not study.parameters.adequacyDraft() and limits.hour[rangeCount] < 168)
    {
        logs.info();
        logs.fatal() << "At least one week is required to run a simulation.";
        // Since this method is only called by the solver, we will abort now.
        // However, we have to release all locks held by the study before to avoid
        // a timeout for a future use of the study
        AntaresSolverEmergencyShutdown(); // will never return
    }
}

BindingConstraintRTI::BindingConstraintRTI() :
 linkWeight(nullptr),
 linkOffset(nullptr),
 linkIndex(nullptr),
 clusterWeight(nullptr),
 clusterOffset(nullptr),
 clusterIndex(nullptr)
{
}

BindingConstraintRTI::~BindingConstraintRTI()
{
    delete[] linkWeight;
    delete[] linkOffset;
    delete[] linkIndex;
    delete[] clusterWeight;
    delete[] clusterOffset;
    delete[] clusterIndex;
}

void StudyRuntimeInfos::initializeBindingConstraints(BindConstList& list)
{
    // Calculating the total number of binding constraints
    bindingConstraintCount = 0;

    list.eachEnabled([&](const BindingConstraint& constraint) {
        bindingConstraintCount
          += ((constraint.operatorType() == BindingConstraint::opBoth) ? 2 : 1);
    });

    switch (bindingConstraintCount)
    {
    case 0:
        logs.info() << "  No binding constraint to consider";
        return;
    case 1:
        logs.info() << "Optimizing 1 binding constraint";
        break;
    default:
        logs.info() << "Optimizing " << bindingConstraintCount << " binding constraints";
    }

    bindingConstraint = new BindingConstraintRTI[bindingConstraintCount];

    uint index = 0;
    list.eachEnabled([&](const BindingConstraint& constraint) {
        assert(index < bindingConstraintCount and "Not enough slots for binding constraints");

        auto& rti = bindingConstraint[index];
        rti.type = constraint.type();
        switch (constraint.operatorType())
        {
        case BindingConstraint::opEquality:
        {
            CopyBCData<BindingConstraint::columnEquality>(rti, constraint);
            break;
        }
        case BindingConstraint::opLess:
        {
            CopyBCData<BindingConstraint::columnInferior>(rti, constraint);
            break;
        }
        case BindingConstraint::opGreater:
        {
            CopyBCData<BindingConstraint::columnSuperior>(rti, constraint);
            break;
        }
        case BindingConstraint::opBoth:
        {
            CopyBCData<BindingConstraint::columnInferior>(rti, constraint);
            ++index;
            bindingConstraint[index].type = constraint.type();
            CopyBCData<BindingConstraint::columnSuperior>(bindingConstraint[index], constraint);
            break;
        }
        case BindingConstraint::opUnknown:
        {
            rti.operatorType = '?';
            rti.linkCount = 0;
            rti.bounds.clear();
            break;
        }
        case BindingConstraint::opMax:
            break;
        }
        ++index;
    });

    logs.debug() << "Releasing " << (list.memoryUsage() / 1024) << "Ko unused";
    list.clear();
}

StudyRuntimeInfos::StudyRuntimeInfos(uint nbYearsParallel) :
 nbYears(0),
 nbHoursPerYear(0),
 nbDaysPerYear(0),
 nbMonthsPerYear(0),
 parameters(nullptr),
 interconnectionsCount(0),
 areaLink(nullptr),
 timeseriesNumberYear(nullptr),
 bindingConstraintCount(0),
 bindingConstraint(nullptr),
 thermalPlantTotalCount(0),
 thermalPlantTotalCountMustRun(0),
#ifdef ANTARES_USE_GLOBAL_MAXIMUM_COST
 hydroCostByAreaShouldBeInfinite(nullptr),
 globalMaximumCost(0.),
#endif
 quadraticOptimizationHasFailed(false),
 weekInTheYear(nullptr),
 currentYear(nullptr)
{
    currentYear = new uint[nbYearsParallel];
    weekInTheYear = new uint[nbYearsParallel];
    // Evite les confusions de numeros de TS entre AMC
    timeseriesNumberYear = new uint[nbYearsParallel];
    for (uint numSpace = 0; numSpace < nbYearsParallel; numSpace++)
    {
        currentYear[numSpace] = 999999;
        weekInTheYear[numSpace] = 999999;
        timeseriesNumberYear[numSpace] = 999999;
    }
}

void StudyRuntimeInfos::checkThermalTSGeneration(Study& study)
{
    const auto& gd = study.parameters;
    bool globalThermalTSgeneration = gd.timeSeriesToGenerate & timeSeriesThermal;
    thermalTSRefresh = globalThermalTSgeneration;
    
    study.areas.each([this, globalThermalTSgeneration](Data::Area& area) {
        area.thermal.list.each([this, globalThermalTSgeneration](const Data::ThermalCluster& cluster) {
            thermalTSRefresh = thermalTSRefresh || cluster.doWeGenerateTS(globalThermalTSgeneration);
        });
    });
}

bool StudyRuntimeInfos::loadFromStudy(Study& study)
{
    auto& gd = study.parameters;

    nbYears = gd.nbYears;
    nbHoursPerYear = 8760;
    nbDaysPerYear = 365;
    nbMonthsPerYear = 12;
    parameters = &study.parameters;
    mode = gd.mode;
    thermalPlantTotalCount = 0;
    thermalPlantTotalCountMustRun = 0;

    // Calendar
    logs.info() << "Generating calendar informations";
    if (study.usedByTheSolver)
    {
        study.calendar.reset(gd, false);
    }
    else
    {
        study.calendar.reset(gd);
    }
    logs.debug() << "  :: generating calendar dedicated to the output";
    study.calendarOutput.reset(gd);
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

    // Max number of thermal/renewable clusters
    initializeMaxClusters(study);

    // Areas
    StudyRuntimeInfosInitializeAllAreas(study, *this);

    // Area links
    StudyRuntimeInfosInitializeAreaLinks(study, *this);

    // Binding constraints
    initializeBindingConstraints(study.bindingConstraints);

    // Check if some clusters request TS generation
    checkThermalTSGeneration(study);

#ifdef ANTARES_USE_GLOBAL_MAXIMUM_COST
    // Hydro cost - Infinite
    hydroCostByAreaShouldBeInfinite = new bool[study.areas.size()];

    // Looking for the maximum global cost
    {
        double m = 0.;
        double tmp;
        double maxModCost;
        // each area...
        study.areas.each([&](Data::Area& area) {
            if (area.thermal.unsuppliedEnergyCost > m)
                m = area.thermal.unsuppliedEnergyCost;

            // each thermal cluster...
            auto end = area.thermal.list.end();
            for (auto i = area.thermal.list.begin(); i != end; ++i)
            {
                auto& cluster = *(i->second);

                tmp = cluster.marketBidCost + (cluster.spreadCost * 2.);
                // Looking for the max. value of the modulation cost
                maxModCost = 0.;
                for (uint j = 0; j < cluster.modulation.height; ++j)
                {
                    if (cluster.modulation[0][j] < maxModCost)
                        maxModCost = cluster.modulation[0][j];
                }
                tmp *= maxModCost;
                if (tmp > m)
                    m = tmp;

                // reduce the memory footprint
                cluster.modulation.flush();
            }
        });
        m *= 1.1;
        globalMaximumCost = m;
        logs.info() << "  Global Maximum cost: " << m;
    }
#endif

    if (not gd.geographicTrimming)
        disableAllFilters(study);

    logs.info();
    logs.info() << "Summary";
    logs.info() << "     areas: " << study.areas.size();
    logs.info() << "     links: " << interconnectionsCount;
    logs.info() << "     thermal clusters: " << thermalPlantTotalCount;
    logs.info() << "     thermal clusters (must-run): " << thermalPlantTotalCountMustRun;
    logs.info() << "     binding constraints: " << bindingConstraintCount;
    logs.info() << "     geographic trimming:" << (gd.geographicTrimming ? "true" : "false");
    logs.info() << "     memory : " << ((study.memoryUsage()) / 1024 / 1024) << "Mo";
    logs.info();

    return true;
}

namespace CompareAreasByNumberOfClusters
{
// Compare areas by number of thermal clusters
struct thermal
{
    bool operator()(const AreaList::value_type& a, const AreaList::value_type& b) const
    {
        assert(a.second);
        assert(b.second);
        return a.second->thermal.clusterCount() < b.second->thermal.clusterCount();
    }
    size_t getNbClusters(const Area* area) const
    {
        assert(area);
        return area->thermal.clusterCount();
    }
};
// Compare areas by number of renewable clusters
struct renewable
{
    bool operator()(const AreaList::value_type& a, const AreaList::value_type& b) const
    {
        assert(a.second);
        assert(b.second);
        return a.second->renewable.clusterCount() < b.second->renewable.clusterCount();
    }
    size_t getNbClusters(const Area* area) const
    {
        assert(area);
        return area->renewable.clusterCount();
    }
};
} // namespace CompareAreasByNumberOfClusters

template<class CompareGetT>
static size_t maxNumberOfClusters(const Study& study)
{
    CompareGetT cmp;
    auto pairWithMostClusters = std::max_element(study.areas.begin(), study.areas.end(), cmp);
    if (pairWithMostClusters != study.areas.end())
    {
        auto area = pairWithMostClusters->second;
        return cmp.getNbClusters(area);
    }
    return 0;
}

void StudyRuntimeInfos::initializeMaxClusters(const Study& study)
{
    this->maxThermalClustersForSingleArea
      = maxNumberOfClusters<CompareAreasByNumberOfClusters::thermal>(study);
    this->maxRenewableClustersForSingleArea
      = maxNumberOfClusters<CompareAreasByNumberOfClusters::renewable>(study);
}

static bool isBindingConstraintTypeInequality(const Data::BindingConstraintRTI& bc)
{
    return bc.operatorType == '<' || bc.operatorType == '>';
}

uint StudyRuntimeInfos::getNumberOfInequalityBindingConstraints() const
{
    const auto* firstBC = this->bindingConstraint;
    const auto* lastBC = firstBC + this->bindingConstraintCount;
    return static_cast<uint>(std::count_if(firstBC, lastBC, isBindingConstraintTypeInequality));
}

std::vector<uint> StudyRuntimeInfos::getIndicesForInequalityBindingConstraints() const
{
    const auto* firstBC = this->bindingConstraint;
    const auto* lastBC = firstBC + this->bindingConstraintCount;

    std::vector<uint> indices;
    for (auto bc = firstBC; bc < lastBC; bc++)
    {
        if (isBindingConstraintTypeInequality(*bc))
        {
            auto index = static_cast<uint>(std::distance(firstBC, bc));
            indices.push_back(index);
        }
    }
    return indices;
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
        if (mode != stdmAdequacyDraft)
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

    delete[] weekInTheYear;
    delete[] currentYear;
    delete[] timeseriesNumberYear;
    delete[] areaLink;
    delete[] bindingConstraint;
#ifdef ANTARES_USE_GLOBAL_MAXIMUM_COST
    delete[] hydroCostByAreaShouldBeInfinite;
#endif
}

Yuni::uint64 StudyRuntimeInfosMemoryUsage(StudyRuntimeInfos* r)
{
    if (r)
    {
        return sizeof(StudyRuntimeInfos) + sizeof(AreaLink*) * r->interconnectionsCount
               + sizeof(BindingConstraint*) * r->bindingConstraintCount;
    }
    return 0;
}

void StudyRuntimeInfosEstimateMemoryUsage(StudyMemoryUsage& u)
{
    u.requiredMemoryForInput += sizeof(StudyRuntimeInfos);
#ifdef ANTARES_USE_GLOBAL_MAXIMUM_COST
    u.requiredMemoryForInput += sizeof(bool) * u.study.areas.size();
#endif

    u.study.areas.each([&](const Data::Area& area) {
        u.requiredMemoryForInput += sizeof(AreaLink*) * area.links.size();
    });

    // Binding constraints
    // see BindConstList::estimateMemoryUsage
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

} // namespace Data
} // namespace Antares
