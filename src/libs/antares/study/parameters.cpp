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
#include <stdio.h>
#include <ctype.h>
#include <algorithm>
#include <iostream>

#include <sstream>

#ifndef YUNI_OS_MSVC
#include <unistd.h>
#endif
#include "../constants.h"
#include "parameters.h"
#include "../inifile.h"
#include "../logs.h"
#include "load-options.h"
#include <limits.h>
#include <antares/study/memory-usage.h>
#include "../solver/variable/economy/all.h"

#include <antares/exception/AssertionError.hpp>
#include <antares/Enum.hxx>

using namespace Yuni;

namespace Antares
{
namespace Data
{
enum
{
    //! Hard coded maximum number of MC years
    maximumMCYears = 100000,
};

static bool ConvertCStrToListTimeSeries(const String& value, uint& v)
{
    v = 0;
    if (!value)
        return true;

    value.words(" ,;\t\r\n", [&](const AnyString& element) -> bool {
        ShortString16 word(element);
        word.toLower();
        if (word == "load")
            v |= timeSeriesLoad;
        else if (word == "wind")
            v |= timeSeriesWind;
        else if (word == "hydro")
            v |= timeSeriesHydro;
        else if (word == "thermal")
            v |= timeSeriesThermal;
        else if (word == "solar")
            v |= timeSeriesSolar;
        else if (word == "renewables")
            v |= timeSeriesRenewable;
        else if (word == "ntc")
            v |= timeSeriesTransmissionCapacities;
        return true;
    });
    return true;
}

static bool ConvertStringToRenewableGenerationModelling(const AnyString& text,
                                                        RenewableGenerationModelling& out)
{
    CString<24, false> s = text;
    s.trim();
    s.toLower();
    if (s == "aggregated")
    {
        out = rgAggregated;
        return true;
    }
    if (s == "clusters") // Using renewable clusters
    {
        out = rgClusters;
        return true;
    }

    logs.warning() << "parameters: invalid renewable generation modelling. Got '" << text << "'";
    out = rgUnknown;

    return false;
}

bool StringToStudyMode(StudyMode& mode, CString<20, false> text)
{
    if (!text)
        return false;
    if (text.size() == 1)
    {
        // Compatibility with v2.x
        if ('0' == text[0])
        {
            mode = stdmAdequacy;
            return true;
        }
        if ('1' == text[0])
        {
            mode = stdmEconomy;
            return true;
        }
        return false;
    }

    // Converting into lowercase
    text.toLower();

    // Economy
    if (text == "economy" || text == "economic")
    {
        // The term `economic` was mis-used in previous versions of antares (<3.4)
        mode = stdmEconomy;
        return true;
    }
    // Adequacy
    if (text == "adequacy")
    {
        mode = stdmAdequacy;
        return true;
    }
    if (text == "draft" || text == "adequacy-draft")
    {
        mode = stdmAdequacyDraft;
        return true;
    }
    // Expansion
    if (text == "expansion")
    {
        mode = stdmExpansion;
        return true;
    }
    return false;
}

const char* StudyModeToCString(StudyMode mode)
{
    switch (mode)
    {
    case stdmEconomy:
        return "Economy";
    case stdmAdequacy:
        return "Adequacy";
    case stdmAdequacyDraft:
        return "draft";
    case stdmMax:
    case stdmExpansion:
    case stdmUnknown:
        return "Unknown";
    }
    return "Unknown";
}

Parameters::Parameters() : yearsFilter(nullptr), noOutput(false)
{
}

Parameters::~Parameters()
{
    delete[] yearsFilter;
}

void Parameters::resetSeeds()
{
    // Initialize all seeds
    // For retro-compatibility, the wind ts-generator should produce the
    // same results than before 3.8.
    // It must have the same seed than before
    auto increment = (unsigned)antaresSeedIncrement;
    auto s = (unsigned)antaresSeedDefaultValue;

    seed[seedTsGenWind] = s;
    // The same way for all others
    for (auto i = (uint)seedTsGenLoad; i != seedMax; ++i)
        seed[i] = (s += increment);
}

void Parameters::reset()
{
    // Mode
    mode = stdmEconomy;
    // Expansion
    expansion = false;
    // Calendar
    horizon = nullptr;

    // Reset output variables print info tool
    variablesPrintInfo.clear();
    variablePrintInfoCollector collector(&variablesPrintInfo);
    Antares::Solver::Variable::Economy::AllVariables::RetrieveVariableList(collector);
    variablesPrintInfo.resetInfoIterator();
    thematicTrimming = false;

    nbYears = 1;
    delete[] yearsFilter;
    yearsFilter = nullptr;

    resetYearsWeigth();

    yearByYear = false;
    derated = false;
    useCustomScenario = false;
    userPlaylist = false;
    geographicTrimming = false;
    simulationDays.first = 0;
    simulationDays.end = 365;
    dayOfThe1stJanuary = monday;
    firstWeekday = monday;
    firstMonthInYear = january;
    leapYear = false;

    effectiveNbYears = 0;

    // TimeSeries
    nbTimeSeriesLoad = 1;
    nbTimeSeriesSolar = 1;
    nbTimeSeriesHydro = 1;
    nbTimeSeriesWind = 1;
    nbTimeSeriesThermal = 1;
    // Time-series refresh
    timeSeriesToRefresh = 0; // None
    refreshIntervalLoad = 100;
    refreshIntervalSolar = 100;
    refreshIntervalHydro = 100;
    refreshIntervalWind = 100;
    refreshIntervalThermal = 100;
    // Archive
    timeSeriesToArchive = 0; // None
    // Pre-Processor
    timeSeriesToGenerate = 0; // None
    // Import
    timeSeriesToImport = 0; // None
    // Same selection
    intraModal = 0; // None
    interModal = 0; // None

    // timeseries numbers
    storeTimeseriesNumbers = false;
    // readonly
    readonly = false;
    synthesis = true;

    // Initial reservoir levels
    initialReservoirLevels.iniLevels = irlColdStart;

    // Hydro heuristic policy
    hydroHeuristicPolicy.hhPolicy = hhpAccommodateRuleCurves;

    // Hydro pricing
    hydroPricing.hpMode = hpHeuristic;
    allSetsHaveSameSize = true;

    // Shedding strategies
    power.fluctuations = lssFreeModulations;
    shedding.strategy = shsShareMargins;
    shedding.policy = shpShavePeaks;

    unitCommitment.ucMode = ucHeuristic;
    nbCores.ncMode = ncAvg;
    renewableGeneration.rgModelling = rgAggregated;
    reserveManagement.daMode = daGlobal;

    // Misc
    improveUnitsStartup = false;
    // Adequacy block size (adequacy-draft)
    adequacyBlockSize = 100;

    include.constraints = true;
    include.hurdleCosts = true;
    transmissionCapacities = tncEnabled;
    include.thermal.minStablePower = true;
    include.thermal.minUPTime = true;

    include.reserve.dayAhead = true;
    include.reserve.strategic = true;
    include.reserve.spinning = true;
    include.reserve.primary = true;
    simplexOptimizationRange = sorWeek;

    include.exportMPS = false;
    include.adequacyPatch = false;
    setToZeroNTCfromOutToIn_AdqPatch = true;
    setToZeroNTCfromOutToOut_AdqPatch = true;
    include.exportStructure = false;

    include.unfeasibleProblemBehavior = UnfeasibleProblemBehavior::ERROR_MPS;

    timeSeriesAccuracyOnCorrelation = 0;

    activeRulesScenario.clear();

    ortoolsUsed = false;
    ortoolsEnumUsed = OrtoolsSolver::sirius;

    // Initialize all seeds
    resetSeeds();
}

static void ParametersSaveTimeSeries(IniFile::Section* s, const char* name, uint value)
{
    CString<60, false> v;

    if (value & timeSeriesLoad)
        v += "load";
    if (value & timeSeriesHydro)
    {
        if (not v.empty())
            v += ", ";
        v += "hydro";
    }
    if (value & timeSeriesWind)
    {
        if (not v.empty())
            v += ", ";
        v += "wind";
    }
    if (value & timeSeriesThermal)
    {
        if (not v.empty())
            v += ", ";
        v += "thermal";
    }
    if (value & timeSeriesSolar)
    {
        if (not v.empty())
            v += ", ";
        v += "solar";
    }
    if (value & timeSeriesRenewable)
    {
        if (not v.empty())
            v += ", ";
        v += "renewables";
    }
    if (value & timeSeriesTransmissionCapacities)
    {
        if (!v.empty())
            v += ", ";
        v += "ntc";
    }
    s->add(name, v);
}

static bool SGDIntLoadFamily_General(Parameters& d,
                                     const String& key,
                                     const String& value,
                                     const String& rawvalue,
                                     uint)
{
    if (key == "active-rules-scenario")
    {
        d.activeRulesScenario = value;
        return true;
    }
    if (key == "custom-scenario")
        return value.to<bool>(d.useCustomScenario);

    if (key == "derated")
        return value.to<bool>(d.derated);

    if (key == "first-month-in-year")
        return Date::StringToMonth(d.firstMonthInYear, value);

    if (key == "first.weekday")
        return Date::StringToDayOfTheWeek(d.firstWeekday, value);

    if (key == "geographic-trimming")
        return value.to<bool>(d.geographicTrimming);

    if (key == "generate")
        return ConvertCStrToListTimeSeries(value, d.timeSeriesToGenerate);

    if (key == "horizon")
    {
        d.horizon = rawvalue;
        d.horizon.trim(" \t\n\r");
        return true;
    }

    // Same time-series
    if (key == "intra-modal")
        return ConvertCStrToListTimeSeries(value, d.intraModal);
    // Same time-series
    if (key == "inter-modal")
        return ConvertCStrToListTimeSeries(value, d.interModal);
    if (key == "improveunitsstartup")
        return true; // value.to<bool>(d.improveUnitsStartup);

    if (key == "january.1st") // after 4.3
        return Date::StringToDayOfTheWeek(d.dayOfThe1stJanuary, value);

    if (key == "leapyear")
        return value.to(d.leapYear);

    if (key == "mode")
        return StringToStudyMode(d.mode, value);

    if (key == "nbyears")
    {
        uint y;
        if (value.to<uint>(y))
        {
            d.years(y);
            return true;
        }
        d.years(1);
        return false;
    }
    if (key == "nbtimeseriesload")
        return value.to<uint>(d.nbTimeSeriesLoad);
    if (key == "nbtimeserieshydro")
        return value.to<uint>(d.nbTimeSeriesHydro);
    if (key == "nbtimeserieswind")
        return value.to<uint>(d.nbTimeSeriesWind);
    if (key == "nbtimeseriesthermal")
        return value.to<uint>(d.nbTimeSeriesThermal);
    if (key == "nbtimeseriessolar")
        return value.to<uint>(d.nbTimeSeriesSolar);
    // Interval values
    if (key == "refreshintervalload")
        return value.to<uint>(d.refreshIntervalLoad);
    if (key == "refreshintervalhydro")
        return value.to<uint>(d.refreshIntervalHydro);
    if (key == "refreshintervalwind")
        return value.to<uint>(d.refreshIntervalWind);
    if (key == "refreshintervalthermal")
        return value.to<uint>(d.refreshIntervalThermal);
    if (key == "refreshintervalsolar")
        return value.to<uint>(d.refreshIntervalSolar);
    // What timeSeries to refresh ?
    if (key == "refreshtimeseries")
        return ConvertCStrToListTimeSeries(value, d.timeSeriesToRefresh);
    // readonly
    if (key == "readonly")
        return value.to<bool>(d.readonly);

    if (key == "simulation.start")
    {
        uint day;
        if (not value.to(day))
            return false;
        if (day == 0)
            day = 1;
        else
        {
            if (day > 365)
                day = 365;
            --day;
        }
        d.simulationDays.first = day;
        return true;
    }
    if (key == "simulation.end")
    {
        uint day;
        if (not value.to(day))
            return false;
        if (day == 0)
            day = 1;
        else if (day > 365)
            day = 365;
        d.simulationDays.end = day; // not included
        return true;
    }

    if (key == "thematic-trimming")
        return value.to<bool>(d.thematicTrimming);

    if (key == "user-playlist")
        return value.to<bool>(d.userPlaylist);

    if (key == "year-by-year")
        return value.to<bool>(d.yearByYear);

    return false;
}
static bool SGDIntLoadFamily_Input(Parameters& d,
                                   const String& key,
                                   const String& value,
                                   const String&,
                                   uint)
{
    if (key == "import")
        return ConvertCStrToListTimeSeries(value, d.timeSeriesToImport);

    return false;
}
static bool SGDIntLoadFamily_Output(Parameters& d,
                                    const String& key,
                                    const String& value,
                                    const String&,
                                    uint)
{
    if (key == "archives")
        return ConvertCStrToListTimeSeries(value, d.timeSeriesToArchive);
    if (key == "storenewset")
        return value.to<bool>(d.storeTimeseriesNumbers);
    if (key == "synthesis")
        return value.to<bool>(d.synthesis);
    if (key == "hydro-debug")
        return value.to<bool>(d.hydroDebug);

    return false;
}
static bool SGDIntLoadFamily_Optimization(Parameters& d,
                                          const String& key,
                                          const String& value,
                                          const String&,
                                          uint)
{
    if (key == "include-constraints")
        return value.to<bool>(d.include.constraints);
    if (key == "include-hurdlecosts")
        return value.to<bool>(d.include.hurdleCosts);
    if (key == "include-loopflowfee") // backward compatibility
        return true;                  // value.to<bool>(d.include.loopFlowFee);
    if (key == "include-tc-minstablepower")
        return value.to<bool>(d.include.thermal.minStablePower);
    if (key == "include-tc-min-ud-time")
        return value.to<bool>(d.include.thermal.minUPTime);
    if (key == "include-dayahead")
        return value.to<bool>(d.include.reserve.dayAhead);
    if (key == "include-strategicreserve")
        return value.to<bool>(d.include.reserve.strategic);
    if (key == "include-spinningreserve")
        return value.to<bool>(d.include.reserve.spinning);
    if (key == "include-primaryreserve")
        return value.to<bool>(d.include.reserve.primary);
    if (key == "include-exportmps")
        return value.to<bool>(d.include.exportMPS);
    if (key == "include-adequacypatch")
        return value.to<bool>(d.include.adequacyPatch);
    if (key == "set-to-null-ntc-from-physical-out-to-physical-in-for-first-step-adq-patch")
        return value.to<bool>(d.setToZeroNTCfromOutToIn_AdqPatch);
    if (key == "set-to-null-ntc-between-physical-out-for-first-step-adq-patch")
        return value.to<bool>(d.setToZeroNTCfromOutToOut_AdqPatch);
    if (key == "include-exportstructure")
        return value.to<bool>(d.include.exportStructure);
    if (key == "include-unfeasible-problem-behavior")
    {
        bool result = true;
        const std::string& string = value.to<std::string>();

        try
        {
            d.include.unfeasibleProblemBehavior
              = Enum::fromString<UnfeasibleProblemBehavior>(string);
        }
        catch (AssertionError& ex)
        {
            logs.warning()
              << "Assertion error for unfeasible problem behavior from string conversion : "
              << ex.what();

            result = false;
            d.include.unfeasibleProblemBehavior = UnfeasibleProblemBehavior::ERROR_MPS;
            logs.warning() << "parameters: invalid unfeasible problem behavior. Got '" << value
                           << "'. reset to " << Enum::toString(d.include.unfeasibleProblemBehavior);
        }
        return result;
    }

    if (key == "link-type")
    {
        CString<64, false> v = value;
        v.trim();
        v.toLower();
        if (value == "local")
            d.linkType = ltLocal;
        else if (value == "ac")
            d.linkType = ltAC;
        else
            d.linkType = ltLocal;
        return true;
    }

    if (key == "simplex-range")
    {
        d.simplexOptimizationRange = (!value.ifind("day")) ? sorDay : sorWeek;
        return true;
    }

    if (key == "transmission-capacities")
    {
        CString<64, false> v = value;
        v.trim();
        v.toLower();
        if (v == "infinite")
            d.transmissionCapacities = tncInfinite;
        else
            d.transmissionCapacities = v.to<bool>() ? tncEnabled : tncIgnore;
        return true;
    }
    return false;
}
static bool SGDIntLoadFamily_OtherPreferences(Parameters& d,
                                              const String& key,
                                              const String& value,
                                              const String&,
                                              uint)
{
    if (key == "day-ahead-reserve-management") // after 5.0
    {
        auto daReserve = StringToDayAheadReserveManagementMode(value);
        if (daReserve != daReserveUnknown)
        {
            d.reserveManagement.daMode = daReserve;
            return true;
        }
        logs.warning() << "parameters: invalid day ahead reserve management mode. Got '" << value
                       << "'. reset to global mode";
        d.reserveManagement.daMode = daGlobal;
        return false;
    }
    if (key == "hydro-heuristic-policy")
    {
        auto hhpolicy = StringToHydroHeuristicPolicy(value);
        if (hhpolicy != hhpUnknown)
        {
            d.hydroHeuristicPolicy.hhPolicy = hhpolicy;
            return true;
        }
        logs.warning() << "parameters: invalid hydro heuristic policy. Got '" << value
                       << "'. Reset to default accommodate rule curves.";
        d.hydroHeuristicPolicy.hhPolicy = hhpAccommodateRuleCurves;
        return false;
    }
    if (key == "hydro-pricing-mode")
    {
        auto hpricing = StringToHydroPricingMode(value);
        if (hpricing != hpUnknown)
        {
            d.hydroPricing.hpMode = hpricing;
            return true;
        }
        logs.warning() << "parameters: invalid hydro pricing mode. Got '" << value
                       << "'. reset to fast mode";
        d.hydroPricing.hpMode = hpHeuristic;
        return false;
    }
    if (key == "initial-reservoir-levels")
    {
        auto iniLevels = StringToInitialReservoirLevels(value);
        if (iniLevels != irlUnknown)
        {
            d.initialReservoirLevels.iniLevels = iniLevels;
            return true;
        }
        logs.warning() << "parameters: invalid initital reservoir levels mode. Got '" << value
                       << "'. reset to cold start mode.";
        d.initialReservoirLevels.iniLevels = irlColdStart;
        return false;
    }

    if (key == "number-of-cores-mode")
    {
        auto ncores = StringToNumberOfCoresMode(value);
        if (ncores != ncUnknown)
        {
            d.nbCores.ncMode = ncores;
            return true;
        }
        logs.warning() << "parameters: invalid number of cores mode. Got '" << value
                       << "'. reset to fast mode";
        d.nbCores.ncMode = ncMin;
        return false;
    }

    if (key == "power-fluctuations")
    {
        auto fluctuations = StringToPowerFluctuations(value);
        if (fluctuations != lssUnknown)
        {
            d.power.fluctuations = fluctuations;
            return true;
        }
        logs.error() << "parameters: invalid power fluctuations. Got '" << value
                     << "'. reset to 'free modulations'";
        d.power.fluctuations = lssFreeModulations;
        return false;
    }

    if (key == "shedding-strategy")
    {
        auto strategy = StringToSheddingStrategy(value);
        if (strategy != shsUnknown)
        {
            d.shedding.strategy = strategy;
            return true;
        }
        logs.error() << "parameters: invalid shedding strategy. Got '" << value << "'";
        return false;
    }
    if (key == "shedding-policy")
    {
        auto policy = StringToSheddingPolicy(value);
        if (policy != shpUnknown)
        {
            d.shedding.policy = policy;
            return true;
        }
        logs.error() << "parameters: invalid shedding policy. Got '" << value << "'";
        return false;
    }
    if (key == "unit-commitment-mode") // after 5.0
    {
        auto ucommitment = StringToUnitCommitmentMode(value);
        if (ucommitment != ucUnknown)
        {
            d.unitCommitment.ucMode = ucommitment;
            return true;
        }
        logs.warning() << "parameters: invalid unit commitment mode. Got '" << value
                       << "'. reset to fast mode";
        d.unitCommitment.ucMode = ucHeuristic;
        return false;
    }
    // Renewable generation modelling
    if (key == "renewable-generation-modelling")
        return ConvertStringToRenewableGenerationModelling(value,
                                                           d.renewableGeneration.rgModelling);

    return false;
}
static bool SGDIntLoadFamily_AdvancedParameters(Parameters& d,
                                                const String& key,
                                                const String& value,
                                                const String&,
                                                uint)
{
    if (key == "adequacy-block-size" || key == "adequacy_blocksize")
        return value.to<uint>(d.adequacyBlockSize);
    if (key == "accuracy-on-correlation")
        return ConvertCStrToListTimeSeries(value, d.timeSeriesAccuracyOnCorrelation);
    return false;
}
static bool SGDIntLoadFamily_Playlist(Parameters& d,
                                      const String& key,
                                      const String& value,
                                      const String&,
                                      uint)
{
    if (key == "playlist_reset")
    {
        bool mode = value.to<bool>();
        if (mode)
        {
            for (uint i = 0; i != d.nbYears; ++i)
                d.yearsFilter[i] = true;
        }
        else
        {
            for (uint i = 0; i != d.nbYears; ++i)
                d.yearsFilter[i] = false;
        }
        return true;
    }
    if (key == "playlist_year +")
    {
        uint y;
        if (value.to<uint>(y))
        {
            if (y < d.nbYears)
                d.yearsFilter[y] = true;
            return true;
        }
        return false;
    }
    if (key == "playlist_year -")
    {
        uint y;
        if (value.to<uint>(y))
        {
            if (y < d.nbYears)
                d.yearsFilter[y] = false;
            return true;
        }
        return false;
    }

    if (key == "playlist_year_weight")
    {
        // Get year index and weight from  string

        // Use of yuni to split string
        std::vector<float> values;
        value.split(values, ",");

        if (values.size() == 2)
        {
            uint y = (uint)(values[0]);
            float weight = values[1];

            // Check values
            bool valid = true;
            if (y > d.nbYears)
            {
                valid = false;
                logs.warning()
                  << "parameters: invalid MC year index for MC year weight definition. Got '" << y
                  << "'. Value not used";
            }

            if (weight < 0.f)
            {
                valid = false;
                logs.warning() << "parameters: invalid MC year weight.Got '" << weight
                               << "'. Value not used";
            }

            if (valid)
            {
                d.setYearWeight(y, weight);
            }

            return true;
        }
        else
        {
            logs.warning() << "parameters: invalid MC year index and weight definition. Must be "
                              "defined by [year],[weight] Got '"
                           << value << "'. Value not used";
            return false;
        }
        return false;
    }
    return false;
}
static bool SGDIntLoadFamily_VariablesSelection(Parameters& d,
                                                const String& key,
                                                const String& value,
                                                const String&,
                                                uint)
{
    if (key == "selected_vars_reset")
    {
        bool mode = value.to<bool>();
        if (mode)
        {
            for (uint i = 0; i != d.variablesPrintInfo.size(); ++i)
                d.variablesPrintInfo[i]->enablePrint(true);
        }
        else
        {
            for (uint i = 0; i != d.variablesPrintInfo.size(); ++i)
                d.variablesPrintInfo[i]->enablePrint(false);
        }
        return true;
    }
    if (key == "select_var +")
        return d.variablesPrintInfo.setPrintStatus(value.to<string>(), true);
    if (key == "select_var -")
        return d.variablesPrintInfo.setPrintStatus(value.to<string>(), false);
    return false;
}
static bool SGDIntLoadFamily_SeedsMersenneTwister(Parameters& d,
                                                  const String& key,
                                                  const String& value,
                                                  const String&,
                                                  uint)
{
    if (key.startsWith("seed")) // seeds
    {
        if (key.size() > 5 && key[4] == '_')
        {
            // This block is kept for compatibility with very old studies
            if (key == "seed_load")
                return value.to<uint>(d.seed[seedTsGenLoad]);
            if (key == "seed_wind")
                return value.to<uint>(d.seed[seedTsGenWind]);
            if (key == "seed_hydro")
                return value.to<uint>(d.seed[seedTsGenHydro]);
            if (key == "seed_thermal")
                return value.to<uint>(d.seed[seedTsGenThermal]);
            if (key == "seed_solar")
                return value.to<uint>(d.seed[seedTsGenSolar]);
            if (key == "seed_timeseriesnumbers")
                return value.to<uint>(d.seed[seedTimeseriesNumbers]);
        }
        else
        {
            // Looking for the good seed
            // TODO This algorithm should be replaced with something more efficient
            for (uint sd = 0; sd != (uint)seedMax; ++sd)
            {
                if (SeedToID((SeedIndex)sd) == key)
                    return value.to<uint>(d.seed[sd]);
            }
        }
    }
    return false;
}
static bool SGDIntLoadFamily_Legacy(Parameters& d,
                                    const String& key,
                                    const String& value,
                                    const String&,
                                    uint version)
{
    // Comparisons kept for compatibility reasons

    // Same time-series
    if (key == "correlateddraws")
        return ConvertCStrToListTimeSeries(value, d.intraModal);
    // Scenario builder
    if (key == "custom-ts-numbers")
        return value.to<bool>(d.useCustomScenario);

    if (key == "dayofthe1stjanuary") // before 4.3 - see january.1st
        return Date::StringToDayOfTheWeek(d.dayOfThe1stJanuary, value);

    if (key == "filtering" && version < 710)
        return value.to<bool>(d.geographicTrimming);

    if (version <= 310 && key == "storetimeseriesnumbers")
        return value.to<bool>(d.storeTimeseriesNumbers);

    // Custom set
    if (key == "customset")
        return true; // value ignored

    if (key == "finalhour") // ignored since v4.3
        return true;        // value.to<uint>(d.finalHour);

    if (key == "startyear") // ignored from 3.5.3155
        return true;

    if (key == "starttime")
        return true; // ignored since 4.3 // return value.to<uint>(d.startTime);

    // deprecated
    if (key == "seed_virtualcost" || key == "seed_misc")
        return true; // ignored since 3.8

    if (key == "spillage_bound") // ignored sinve v3.3
        return true;

    if (key == "spillage_cost") // ignored since v3.3
        return true;

    if (key == "shedding-strategy-local") // ignored since 4.0
        return true;

    if (key == "shedding-strategy-global") // ignored since 4.0
        return true;
    // deprecated
    if (key == "thresholdmin")
        return true; // value.to<int>(d.thresholdMinimum);
    if (key == "thresholdmax")
        return true; // value.to<int>(d.thresholdMaximum);

    return false;
}

bool firstKeyLetterIsValid(const String& name)
{
    char firstLetter = name.first();
    return (firstLetter >= 'a' && firstLetter <= 'z');
}

bool Parameters::loadFromINI(const IniFile& ini, uint version, const StudyLoadOptions& options)
{
    // Reset inner data
    reset();
    // A temporary buffer, used for the values in lowercase
    String value;
    String sectionName;
    typedef bool (*Callback)(
      Parameters&,   // [out] Parameter object to load the data into
      const String&, // [in] Key, comes left to the '=' sign in the .ini file
      const String&, // [in] Lowercase value, comes right to the '=' sign in the .ini file
      const String&, // [in] Raw value as writtent right to the '=' sign in the .ini file
      uint);         // [in] Version of the study (such as 710)

    static const std::map<String, Callback> sectionAssociatedToKeysProcess
      = {{"general", &SGDIntLoadFamily_General},
         {"input", &SGDIntLoadFamily_Input},
         {"output", &SGDIntLoadFamily_Output},
         {"optimization", &SGDIntLoadFamily_Optimization},
         {"other preferences", &SGDIntLoadFamily_OtherPreferences},
         {"advanced parameters", &SGDIntLoadFamily_AdvancedParameters},
         {"playlist", &SGDIntLoadFamily_Playlist},
         {"variables selection", &SGDIntLoadFamily_VariablesSelection},
         {"seeds - mersenne twister", &SGDIntLoadFamily_SeedsMersenneTwister}};

    Callback handleAllKeysInSection;
    // Foreach section on the ini file...
    for (auto* section = ini.firstSection; section; section = section->next)
    {
        sectionName = section->name;
        sectionName.toLower();
        try
        {
            handleAllKeysInSection = sectionAssociatedToKeysProcess.at(sectionName);
        }
        catch (const std::out_of_range&)
        {
            // Continue on error
            logs.warning() << ini.filename() << ": '" << section->name << "': Unknown section name";
            continue;
        }

        // Foreach properties in the section
        for (const IniFile::Property* p = section->firstProperty; p; p = p->next)
        {
            if (p->key.empty())
                continue;
            if (!firstKeyLetterIsValid(p->key))
                continue;
            // We convert the key and the value into the lower case format
            value = p->value;
            value.toLower();

            // Deal with the current property
            // Do not forget the variable `key` and `value` are identical to
            // `p->key` and `p->value` except they are already in the lower case format
            if (not handleAllKeysInSection(*this, p->key, value, p->value, version))
            {
                if (not SGDIntLoadFamily_Legacy(*this, p->key, value, p->value, version))
                {
                    // Continue on error
                    logs.warning() << ini.filename() << ": '" << p->key << "': Unknown property";
                }
            }
        }
    }

    // forcing value
    if (options.nbYears != 0)
    {
        if (options.nbYears > nbYears)
        {
            if (yearsFilter)
            {
                // The variable `yearsFilter` must be enlarged
                bool* newset = new bool[options.nbYears];
                for (uint i = 0; i != nbYears; ++i)
                    newset[i] = yearsFilter[i];
                for (uint i = nbYears; i < options.nbYears; ++i)
                    newset[i] = false;
                delete[] yearsFilter;
                yearsFilter = newset;
            }
            else
            {
                yearsFilter = new bool[options.nbYears];
                for (uint i = 0; i < options.nbYears; ++i)
                    yearsFilter[i] = true;
            }
        }
        nbYears = options.nbYears;

        // Resize years weight (add or remove item)
        if (yearsWeight.size() != nbYears)
        {
            yearsWeight.resize(nbYears, 1.f);
        }
    }

    if (version < 400)
    {
        // resetting shedding strategies
        power.fluctuations = lssFreeModulations;
        shedding.strategy = shsShareSheddings;
        shedding.policy = shpShavePeaks;
    }

    // Simulation mode
    // ... Expansion
    if (mode == stdmExpansion)
    {
        mode = stdmEconomy;
        expansion = true;
    }

    // ... Enforcing simulation mode
    if (options.forceMode != stdmUnknown)
    {
        if (options.forceMode == stdmExpansion)
        {
            mode = stdmEconomy;
            expansion = true;
        }
        else
            mode = options.forceMode;

        logs.info() << "  forcing the simulation mode " << StudyModeToCString(mode);
        assert(mode != stdmMax && "Invalid simulation mode");
    }
    else
        logs.info() << "  simulation mode: " << StudyModeToCString(mode);

    if (options.forceDerated)
        derated = true;

    // Define ortools parameters from options
    ortoolsUsed = options.ortoolsUsed;
    ortoolsEnumUsed = options.ortoolsEnumUsed;

    // Attempt to fix bad values if any
    fixBadValues();

    fixRefreshIntervals();

    fixGenRefreshForNTC();

    // Specific action before launching a simulation
    if (options.usedByTheSolver)
        prepareForSimulation(options);

    // We currently always returns true to not block any loading process
    // Anyway we already have reported all problems
    return true;
}

void Parameters::fixRefreshIntervals()
{
    if (timeSeriesLoad & timeSeriesToRefresh && 0 == refreshIntervalLoad)
    {
        refreshIntervalLoad = 1;
        logs.error() << "The load time-series must be refreshed but the interval is equal to 0. "
                        "Auto-Reset to a safe value (1).";
    }
    if (timeSeriesSolar & timeSeriesToRefresh && 0 == refreshIntervalSolar)
    {
        refreshIntervalSolar = 1;
        logs.error() << "The solar time-series must be refreshed but the interval is equal to 0. "
                        "Auto-Reset to a safe value (1).";
    }
    if (timeSeriesHydro & timeSeriesToRefresh && 0 == refreshIntervalHydro)
    {
        refreshIntervalHydro = 1;
        logs.error() << "The hydro time-series must be refreshed but the interval is equal to 0. "
                        "Auto-Reset to a safe value (1).";
    }
    if (timeSeriesWind & timeSeriesToRefresh && 0 == refreshIntervalWind)
    {
        refreshIntervalWind = 1;
        logs.error() << "The wind time-series must be refreshed but the interval is equal to 0. "
                        "Auto-Reset to a safe value (1).";
    }
    if (timeSeriesThermal & timeSeriesToRefresh && 0 == refreshIntervalThermal)
    {
        refreshIntervalThermal = 1;
        logs.error() << "The thermal time-series must be refreshed but the interval is equal to 0. "
                        "Auto-Reset to a safe value (1).";
    }
}

void Parameters::fixGenRefreshForNTC()
{
    if (timeSeriesTransmissionCapacities & timeSeriesToGenerate != 0)
    {
        timeSeriesToGenerate &= ~timeSeriesTransmissionCapacities;
        logs.error() << "Time-series generation is not available for transmission capacities. It "
                        "will be automatically disabled.";
    }
    if (timeSeriesTransmissionCapacities & timeSeriesToRefresh != 0)
    {
        timeSeriesToRefresh &= ~timeSeriesTransmissionCapacities;
        logs.error() << "Time-series refresh is not available for transmission capacities. It will "
                        "be automatically disabled.";
    }
    if (timeSeriesTransmissionCapacities & interModal != 0)
    {
        interModal &= ~timeSeriesTransmissionCapacities;
        logs.error() << "Inter-modal correlation is not available for transmission capacities. It "
                        "will be automatically disabled.";
    }
}

void Parameters::fixBadValues()
{
    // Adequacy block size
    if (adequacyBlockSize < 100 || adequacyBlockSize > 100000)
    {
        adequacyBlockSize = 100;
        logs.warning() << "The block size for the adequacy algorithm is invalid (100 <= blocksize "
                          "<= 100000). Reset to "
                       << adequacyBlockSize << '.';
    }

    if (derated)
    {
        // Force the number of years
        nbYears = 1;
        if (!yearsFilter)
        {
            yearsFilter = new bool[1];
            yearsFilter[0] = true;
        }

        resetYearsWeigth();
    }
    else
    {
        // Nb years
        if (nbYears > maximumMCYears)
        {
            // The maximal amount of years is an hard-coded value
            // If some changes are needed, please check in the same time the assert
            // in the routine MatrixResize()
            logs.error() << "The number of MC years is too high (>" << (uint)maximumMCYears << ")";
            nbYears = maximumMCYears;
        }
    }

    if (mode == stdmAdequacyDraft)
    {
        simulationDays.first = 0;
        simulationDays.end = 365;
    }

    if (!nbTimeSeriesLoad)
        nbTimeSeriesLoad = 1;
    if (!nbTimeSeriesThermal)
        nbTimeSeriesThermal = 1;
    if (!nbTimeSeriesHydro)
        nbTimeSeriesHydro = 1;
    if (!nbTimeSeriesWind)
        nbTimeSeriesWind = 1;
    if (!nbTimeSeriesSolar)
        nbTimeSeriesSolar = 1;
}

void Parameters::resetYearsWeigth()
{
    yearsWeight.clear();
    yearsWeight.assign(nbYears, 1.f);
}

std::vector<float> Parameters::getYearsWeight() const
{
    std::vector<float> result;

    if (userPlaylist)
    {
        result = yearsWeight;
    }
    else
    {
        result.assign(nbYears, 1.f);
    }

    return result;
}
float Parameters::getYearsWeightSum() const
{
    float result = 0.f;

    if (userPlaylist)
    {
        // must take into account inactive years
        for (uint i = 0; i < nbYears; ++i)
        {
            if (yearsFilter[i])
            {
                result += yearsWeight[i];
            }
        }

        // Check if value is 0.0 then return 1.0 to avoid division by 0
        if (result == 0.f)
        {
            result = 1.f;
        }
    }
    else
    {
        // if no user playlist, nbYears
        result = (float)nbYears;
    }

    return result;
}

void Parameters::setYearWeight(int year, float weight)
{
    assert(year < yearsWeight.size());
    yearsWeight[year] = weight;
}

void Parameters::prepareForSimulation(const StudyLoadOptions& options)
{
    // We don't care of the variable `horizon` since it is not used by the solver
    horizon.clear();
    horizon.shrink();

    // Simplex optimization range
    switch (simplexOptimizationRange)
    {
    case sorDay:
        logs.info() << "  simplex optimization range: day";
        break;
    case sorWeek:
        logs.info() << "  simplex optimization range: week";
        break;
    case sorUnknown:
        break;
    }

    if (mode == stdmAdequacyDraft)
    {
        yearByYear = false;
        userPlaylist = false;
        thematicTrimming = false;
    }

    if (derated && userPlaylist)
    {
        userPlaylist = false;
        logs.warning() << "The user's playlist will be ignored";
    }
    if (derated && useCustomScenario)
    {
        useCustomScenario = false;
        logs.warning() << "The custom build mode can not be used with the derated option";
    }
    if (useCustomScenario && activeRulesScenario.empty())
    {
        useCustomScenario = false;
        logs.warning() << "The custom build mode will be ignored (no active ruleset)";
    }

    // If the user's playlist is disabled, the filter must be reset
    assert(yearsFilter && "The array yearsFilter must be valid at this point");
    if (!userPlaylist)
    {
        for (uint i = 0; i < nbYears; ++i)
            yearsFilter[i] = true;
        effectiveNbYears = nbYears;
    }
    else
    {
        // calculating the effective number of years
        effectiveNbYears = 0;
        for (uint i = 0; i < nbYears; ++i)
        {
            if (yearsFilter[i])
                ++effectiveNbYears;
        }
        switch (effectiveNbYears)
        {
        case 0:
            logs.fatal() << "User's playlist: at least one year must be selected";
            break;
        case 1:
            logs.info() << "  1 year in the user's playlist";
            break;
        default:
            logs.info() << "  " << effectiveNbYears << " years in the user's playlist";
        }

        // Add log in case of MC year weight different from 1.0
        std::vector<int> maximumWeightYearsList;
        int nbYearsDifferentFrom1 = 0;
        float maximumWeight = *std::max_element(yearsWeight.begin(), yearsWeight.end());
        for (uint i = 0; i < yearsWeight.size(); i++)
        {
            float weight = yearsWeight[i];
            if (weight != 1.f)
            {
                nbYearsDifferentFrom1++;
                if (weight == maximumWeight)
                {
                    maximumWeightYearsList.push_back(i);
                }
            }
        }

        if (nbYearsDifferentFrom1 != 0)
        {
            logs.info() << "  " << nbYearsDifferentFrom1
                        << " years with weight !=1 in the user's playlist";

            // Transform maximum value years to string
            std::stringstream ss;
            copy(maximumWeightYearsList.begin(),
                 maximumWeightYearsList.end(),
                 std::ostream_iterator<int>(ss, ","));
            std::string s = ss.str();
            s = s.substr(0, s.length() - 1); // get rid of the trailing ,

            logs.info() << "  maximum weight " << maximumWeight << " for year(s) " << s;
        }
    }

    // Prepare output variables print info before the simulation (used to initialize output
    // variables)

    // Force enable/disable when cluster/aggragated production is enabled
    // This will be deprecated when support for aggragated production is dropped.
    switch (renewableGeneration()) // Warn the user about that.
    {
    case rgClusters:
        logs.info()
          << "Cluster renewables were chosen. Output will be disabled for aggregated modes.";
        break;
    case rgAggregated:
        logs.info()
          << "Aggregate renewables were chosen. Output will be disabled for renewable clusters.";
        break;
    }
    const std::vector<std::string> excluded_vars = renewableGeneration.excludedVariables();
    variablesPrintInfo.prepareForSimulation(thematicTrimming, excluded_vars);

    switch (mode)
    {
    case stdmEconomy:
    {
        // The year-by-year mode might have been requested from the command line
        if (options.forceYearByYear)
            yearByYear = true;
        break;
    }
    case stdmAdequacy:
    {
        // The year-by-year mode might have been requested from the command line
        if (options.forceYearByYear)
            yearByYear = true;
        break;
    }
    case stdmAdequacyDraft:
    {
        // The mode year-by-year can not be enabled in adequacy
        yearByYear = false;
        break;
    }
    case stdmUnknown:
    case stdmExpansion:
    case stdmMax:
    {
        // The mode year-by-year can not be enabled in adequacy
        yearByYear = false;
        break;
    }
    }

    if (interModal == timeSeriesLoad || interModal == timeSeriesSolar
        || interModal == timeSeriesWind || interModal == timeSeriesHydro
        || interModal == timeSeriesThermal || interModal == timeSeriesRenewable)
    {
        // Only one timeseries in interModal correlation, which is the same than nothing
        interModal = 0;
    }

    // Preprocessors
    if (!timeSeriesToGenerate)
    {
        // Nothing to refresh
        timeSeriesToRefresh = 0;
    }
    else
    {
        // Removing `refresh`
        if (!(timeSeriesToGenerate & timeSeriesLoad))
            timeSeriesToRefresh &= ~timeSeriesLoad;
        if (!(timeSeriesToGenerate & timeSeriesSolar))
            timeSeriesToRefresh &= ~timeSeriesSolar;
        if (!(timeSeriesToGenerate & timeSeriesWind))
            timeSeriesToRefresh &= ~timeSeriesWind;
        if (!(timeSeriesToGenerate & timeSeriesHydro))
            timeSeriesToRefresh &= ~timeSeriesHydro;
        if (!(timeSeriesToGenerate & timeSeriesThermal))
            timeSeriesToRefresh &= ~timeSeriesThermal;

        // Force mode refresh if the timeseries must be regenerated
        if (timeSeriesToGenerate & timeSeriesLoad && !(timeSeriesToRefresh & timeSeriesLoad))
        {
            timeSeriesToRefresh |= timeSeriesLoad;
            refreshIntervalLoad = UINT_MAX;
        }
        if (timeSeriesToGenerate & timeSeriesSolar && !(timeSeriesToRefresh & timeSeriesSolar))
        {
            timeSeriesToRefresh |= timeSeriesSolar;
            refreshIntervalSolar = UINT_MAX;
        }
        if (timeSeriesToGenerate & timeSeriesWind && !(timeSeriesToRefresh & timeSeriesWind))
        {
            timeSeriesToRefresh |= timeSeriesWind;
            refreshIntervalWind = UINT_MAX;
        }
        if (timeSeriesToGenerate & timeSeriesHydro && !(timeSeriesToRefresh & timeSeriesHydro))
        {
            timeSeriesToRefresh |= timeSeriesHydro;
            refreshIntervalHydro = UINT_MAX;
        }
        if (timeSeriesToGenerate & timeSeriesThermal && !(timeSeriesToRefresh & timeSeriesThermal))
        {
            timeSeriesToRefresh |= timeSeriesThermal;
            refreshIntervalThermal = UINT_MAX;
        }
    }

    if (options.noTimeseriesImportIntoInput && timeSeriesToArchive != 0)
    {
        logs.info() << "  :: ignoring timeseries importation to input";
        timeSeriesToImport = 0;
    }

    if (expansion)
        logs.info() << "  :: enabling expansion";
    if (yearByYear)
        logs.info() << "  :: enabling the 'year-by-year' mode";
    if (derated)
        logs.info() << "  :: enabling the 'derated' mode";
    if (userPlaylist)
        logs.info() << "  :: enabling the user playlist";
    if (thematicTrimming)
        logs.info() << "  :: enabling the user variable selection";
    if (useCustomScenario)
        logs.info() << "  :: enabling the custom build mode";
    if (geographicTrimming)
        logs.info() << "  :: enabling filtering by file";

    if (!include.constraints)
        logs.info() << "  :: ignoring binding constraints";
    if (!include.reserve.dayAhead)
        logs.info() << "  :: ignoring day ahead reserves";
    if (!include.reserve.primary)
        logs.info() << "  :: ignoring primary reserves";
    if (!include.reserve.strategic)
        logs.info() << "  :: ignoring strategic reserves";
    if (!include.reserve.spinning)
        logs.info() << "  :: ignoring spinning reserves";
    if (!include.thermal.minStablePower)
        logs.info() << "  :: ignoring min stable power for thermal clusters";
    if (!include.thermal.minUPTime)
        logs.info() << "  :: ignoring min up/down time for thermal clusters";
    if (!include.exportMPS)
        logs.info() << "  :: ignoring export mps";
    if (!include.adequacyPatch)
        logs.info() << "  :: ignoring adequacy patch";
    if (!include.exportStructure)
        logs.info() << "  :: ignoring export structure";
    if (!include.hurdleCosts)
        logs.info() << "  :: ignoring hurdle costs";

    // Indicate ortools solver used
    if (ortoolsUsed)
    {
        logs.info() << "  :: ortools solver " << Enum::toString(ortoolsEnumUsed)
                    << " used for problem resolution";
    }
}

void Parameters::years(uint y)
{
    // Resetting the filter on the years
    delete[] yearsFilter;

    if (y < 2)
    {
        nbYears = 1;
        yearsFilter = new bool[1];
        yearsFilter[0] = true;
    }
    else
    {
        nbYears = (y > (uint)maximumMCYears) ? (uint)maximumMCYears : y;
        // Reset the filter
        yearsFilter = new bool[nbYears];
        for (uint i = 0; i != nbYears; ++i)
            yearsFilter[i] = true;
    }

    resetYearsWeigth();
}

StudyError Parameters::checkIntegrity() const
{
    return stErrNone;
}

void Parameters::saveToINI(IniFile& ini) const
{
    // -- General --
    {
        auto* section = ini.addSection("general");

        // Mode
        if (expansion && mode == stdmEconomy)
            section->add("mode", "Expansion");
        else
        {
            section->add("mode", StudyModeToCString(mode));
            expansion = false;
        }

        // Calendar
        section->add("horizon  ", horizon);
        section->add("nbYears  ", nbYears);
        section->add("simulation.start", simulationDays.first + 1); // starts from 1
        section->add("simulation.end", simulationDays.end);         // starts from 1
        section->add("january.1st", Date::DayOfTheWeekToString(dayOfThe1stJanuary));
        section->add("first-month-in-year", Date::MonthToLowerString(firstMonthInYear));
        section->add("first.weekday", Date::DayOfTheWeekToString(firstWeekday));
        section->add("leapyear", leapYear);

        // Simulation
        section->add("year-by-year", yearByYear);
        section->add("derated", derated);
        section->add("custom-scenario", useCustomScenario);
        section->add("user-playlist", userPlaylist);
        section->add("thematic-trimming", thematicTrimming);
        section->add("geographic-trimming", geographicTrimming);
        if (not activeRulesScenario.empty())
            section->add("active-rules-scenario", activeRulesScenario);

        // Time series
        ParametersSaveTimeSeries(section, "generate", timeSeriesToGenerate);
        section->add("nbTimeSeriesLoad    ", nbTimeSeriesLoad);
        section->add("nbTimeSeriesHydro   ", nbTimeSeriesHydro);
        section->add("nbTimeSeriesWind    ", nbTimeSeriesWind);
        section->add("nbTimeSeriesThermal ", nbTimeSeriesThermal);
        section->add("nbTimeSeriesSolar   ", nbTimeSeriesSolar);

        // Refresh
        ParametersSaveTimeSeries(section, "refreshTimeSeries", timeSeriesToRefresh);
        ParametersSaveTimeSeries(section, "intra-modal", intraModal);
        ParametersSaveTimeSeries(section, "inter-modal", interModal);
        section->add("refreshIntervalLoad", refreshIntervalLoad);
        section->add("refreshIntervalHydro", refreshIntervalHydro);
        section->add("refreshIntervalWind", refreshIntervalWind);
        section->add("refreshIntervalThermal", refreshIntervalThermal);
        section->add("refreshIntervalSolar", refreshIntervalSolar);

        // Readonly
        section->add("readonly", readonly);
    }

    // input
    {
        auto* section = ini.addSection("input");
        ParametersSaveTimeSeries(section, "import", timeSeriesToImport);
    }

    // Output
    {
        auto* section = ini.addSection("output");
        section->add("synthesis", synthesis);
        section->add("storeNewSet", storeTimeseriesNumbers);
        if (hydroDebug)
            section->add("hydro-debug", hydroDebug);
        ParametersSaveTimeSeries(section, "archives", timeSeriesToArchive);
    }

    // Optimization
    {
        auto* section = ini.addSection("optimization");
        switch (simplexOptimizationRange)
        {
        case sorDay:
            section->add("simplex-range", "day");
            break;
        case sorWeek:
            section->add("simplex-range", "week");
            break;
        case sorUnknown:
            break;
        }
        // Optimization preferences
        switch (transmissionCapacities)
        {
        case tncEnabled:
            section->add("transmission-capacities", "true");
            break;
        case tncIgnore:
            section->add("transmission-capacities", "false");
            break;
        case tncInfinite:
            section->add("transmission-capacities", "infinite");
            break;
        }

        switch (linkType)
        {
        case ltLocal:
            section->add("link-type", "local");
            break;
        case ltAC:
            section->add("link-type", "ac");
            break;
        }
        section->add("include-constraints", include.constraints);
        section->add("include-hurdlecosts", include.hurdleCosts);
        section->add("include-tc-minstablepower", include.thermal.minStablePower);
        section->add("include-tc-min-ud-time", include.thermal.minUPTime);
        section->add("include-dayahead", include.reserve.dayAhead);
        section->add("include-strategicreserve", include.reserve.strategic);
        section->add("include-spinningreserve", include.reserve.spinning);
        section->add("include-primaryreserve", include.reserve.primary);

        section->add("include-exportmps", include.exportMPS);
        section->add("include-adequacypatch", include.adequacyPatch);
        section->add("set-to-null-ntc-from-physical-out-to-physical-in-for-first-step-adq-patch",
                     setToZeroNTCfromOutToIn_AdqPatch);
        section->add("set-to-null-ntc-between-physical-out-for-first-step-adq-patch",
                     setToZeroNTCfromOutToOut_AdqPatch);
        section->add("include-exportstructure", include.exportStructure);

        // Unfeasible problem behavior
        section->add("include-unfeasible-problem-behavior",
                     Enum::toString(include.unfeasibleProblemBehavior));
    }

    // Other preferences
    {
        auto* section = ini.addSection("other preferences");
        section->add("initial-reservoir-levels",
                     InitialReservoirLevelsToCString(initialReservoirLevels.iniLevels));
        section->add("hydro-heuristic-policy",
                     HydroHeuristicPolicyToCString(hydroHeuristicPolicy.hhPolicy));
        section->add("hydro-pricing-mode", HydroPricingModeToCString(hydroPricing.hpMode));
        section->add("power-fluctuations", PowerFluctuationsToCString(power.fluctuations));
        section->add("shedding-strategy", SheddingStrategyToCString(shedding.strategy));
        section->add("shedding-policy", SheddingPolicyToCString(shedding.policy));
        section->add("unit-commitment-mode", UnitCommitmentModeToCString(unitCommitment.ucMode));
        section->add("number-of-cores-mode", NumberOfCoresModeToCString(nbCores.ncMode));
        section->add("renewable-generation-modelling",
                     RenewableGenerationModellingToCString(renewableGeneration()));
        section->add("day-ahead-reserve-management",
                     DayAheadReserveManagementModeToCString(reserveManagement.daMode));
    }

    // Advanced parameters
    {
        auto* section = ini.addSection("advanced parameters");
        // Accuracy on correlation
        ParametersSaveTimeSeries(
          section, "accuracy-on-correlation", timeSeriesAccuracyOnCorrelation);
        // Adequacy Block size (adequacy draft)
        section->add("adequacy-block-size", adequacyBlockSize);
    }

    // User's playlist
    {
        assert(yearsFilter);
        uint effNbYears = 0;
        bool weightEnabled = false;
        for (uint i = 0; i != nbYears; ++i)
        {
            if (yearsFilter[i])
                ++effNbYears;
            weightEnabled |= yearsWeight[i] != 1.f;
        }

        // Playlist section must be added if at least one year is disable or one MC year weight is
        // not 1.0
        bool addPlayListSection = effNbYears != nbYears;
        addPlayListSection |= weightEnabled;

        if (addPlayListSection)
        {
            // We have something to write !
            auto* section = ini.addSection("playlist");
            if (effNbYears <= (nbYears / 2))
            {
                section->add("playlist_reset", "false");
                for (uint i = 0; i != nbYears; ++i)
                {
                    if (yearsFilter[i])
                        section->add("playlist_year +", i);
                }
            }
            else
            {
                for (uint i = 0; i != nbYears; ++i)
                {
                    if (not yearsFilter[i])
                        section->add("playlist_year -", i);
                }
            }

            for (uint i = 0; i != nbYears; ++i)
            {
                // Only write weight different from 1.0 to limit .ini file size and readability
                if (yearsWeight[i] != 1.f)
                {
                    std::string val = std::to_string(i) + "," + std::to_string(yearsWeight[i]);
                    section->add("playlist_year_weight", val);
                }
            }
        }
    }

    // Variable selection
    {
        assert(!variablesPrintInfo.isEmpty());
        uint nb_tot_vars = (uint)variablesPrintInfo.size();
        uint selected_vars = 0;

        for (uint i = 0; i != nb_tot_vars; ++i)
        {
            if (variablesPrintInfo[i]->isPrinted())
                ++selected_vars;
        }
        if (selected_vars != nb_tot_vars)
        {
            // We have something to write !
            auto* section = ini.addSection("variables selection");
            if (selected_vars <= (nb_tot_vars / 2))
            {
                section->add("selected_vars_reset", "false");
                for (uint i = 0; i != nb_tot_vars; ++i)
                {
                    if (variablesPrintInfo[i]->isPrinted())
                        section->add("select_var +", variablesPrintInfo[i]->name());
                }
            }
            else
            {
                for (uint i = 0; i != nb_tot_vars; ++i)
                {
                    if (not variablesPrintInfo[i]->isPrinted())
                        section->add("select_var -", variablesPrintInfo[i]->name());
                }
            }
        }
    }

    // Seeds
    {
        auto* section = ini.addSection("seeds - Mersenne Twister");
        for (uint sd = 0; sd != (uint)seedMax; ++sd)
            section->add(SeedToID((SeedIndex)sd), seed[sd]);
    }
}

bool Parameters::loadFromFile(const AnyString& filename,
                              uint version,
                              const StudyLoadOptions& options)
{
    // Loading the INI file
    IniFile ini;
    if (ini.open(filename))
        return loadFromINI(ini, version, options);

    // Error otherwise
    reset();

    // Try to detect and to fix some common mistakes
    fixBadValues();
    return false;
}

bool Parameters::saveToFile(const AnyString& filename) const
{
    IniFile ini;
    saveToINI(ini);
    return ini.save(filename);
}

std::vector<std::string> Parameters::RenewableGeneration::excludedVariables() const
{
    switch (rgModelling)
    {
    /*
       Order is important because AllVariablesPrintInfo::setPrintStatus
       does not reset the search pointer.

       Inverting some variable names below may result in some of them not being
       taken into account.
    */
    case rgAggregated:
        return {"wind offshore",
                "wind onshore",
                "solar concrt.",
                "solar pv",
                "solar rooft",
                "renw. 1",
                "renw. 2",
                "renw. 3",
                "renw. 4"};
    case rgClusters:
        return {"wind", "solar"};
    case rgUnknown:
        return {};
    }
    return {};
}

bool Parameters::haveToImport(int tsKind) const
{
    if (tsKind == timeSeriesThermal)
    {
        // Special case: some clusters might override the global parameter,
        // see Cluster::doWeGenerateTS
        return timeSeriesToImport & tsKind;
    }
    return (timeSeriesToImport & tsKind) && (timeSeriesToGenerate & tsKind);
}

RenewableGenerationModelling Parameters::RenewableGeneration::operator()() const
{
    return rgModelling;
}

void Parameters::RenewableGeneration::toAggregated()
{
    rgModelling = Antares::Data::rgAggregated;
}

void Parameters::RenewableGeneration::toClusters()
{
    rgModelling = Antares::Data::rgClusters;
}

bool Parameters::RenewableGeneration::isAggregated() const
{
    return rgModelling == Antares::Data::rgAggregated;
}

bool Parameters::RenewableGeneration::isClusters() const
{
    return rgModelling == Antares::Data::rgClusters;
}

} // namespace Data
} // namespace Antares
