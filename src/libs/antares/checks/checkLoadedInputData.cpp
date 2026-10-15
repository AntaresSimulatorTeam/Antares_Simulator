// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/checks/checkLoadedInputData.h>
#include <antares/exception/LoadingError.hpp>
#include <antares/series/series.h>
#include <antares/study/area/area.h>
#include <antares/study/parameters.h>

namespace Antares::Check
{
using namespace Antares::Data;

void checkStudyVersion(const StudyVersion& version, const std::string& StudyFolder)
{
    if (version == StudyVersion::unknown())
    {
        throw Error::InvalidStudy(StudyFolder);
    }

    if (version > StudyVersion::latest())
    {
        throw Error::InvalidVersion(version.toString(), StudyVersion::latest().toString());
    }
}

// CHECK incompatible de choix simultané des options « simplex range= daily » et « hydro-pricing
// = MILP ».
void checkSimplexRangeHydroPricing(SimplexOptimization optRange, HydroPricingMode hpMode)
{
    if (optRange == SimplexOptimization::sorDay && hpMode == HydroPricingMode::hpMILP)
    {
        throw Error::IncompatibleOptRangeHydroPricing();
    }
}

// CHECK incompatible de choix simultané des options « simplex range= daily » et «
// unit-commitment = MILP ».
void checkSimplexRangeUnitCommitmentMode(SimplexOptimization optRange, UnitCommitmentMode ucMode)
{
    if (optRange == SimplexOptimization::sorDay && ucMode == UnitCommitmentMode::ucMILP)
    {
        throw Error::IncompatibleOptRangeUCMode();
    }
}

// Daily simplex optimisation and any area's use heurictic target turned to "No" are not
// compatible.
void checkSimplexRangeHydroHeuristic(SimplexOptimization optRange, const AreaList& areas)
{
    if (optRange == SimplexOptimization::sorDay)
    {
        for (uint i = 0; i < areas.size(); ++i)
        {
            const auto& area = *(areas.byIndex[i]);
            if (!area.hydro.useHeuristicTarget)
            {
                throw IncompatibleDailyOptHeuristicForArea(area.name);
            }
        }
    }
}

bool areasThermalClustersMinStablePowerValidity(const AreaList& areas,
                                                std::map<int, std::string>& areaClusterNames)
{
    std::string areaname;
    bool resultat = true;
    int count = 0;
    for (const auto& [_, area]: areas)
    {
        areaname = area->name;
        logs.debug() << "areaname : " << areaname;

        std::vector<std::string> clusternames;

        if (!area->thermalClustersMinStablePowerValidity(clusternames))
        {
            for (const auto& clusterName: clusternames)
            {
                logs.debug() << "areaname : " << areaname << " ; clustername : " << clusterName;
                std::string res = "Area : " + areaname + " cluster name : " + clusterName;
                areaClusterNames.try_emplace(count++, res);
            }
            resultat = false;
        }
    }
    return resultat;
}

void checkMinStablePower(const Antares::Data::Parameters* parameters,
                         const Antares::Data::AreaList& areas)
{
    const bool tsGenThermal = (parameters->timeSeriesToGenerate
                               & Antares::Data::TimeSeriesType::timeSeriesThermal)
                              != 0;
    if (tsGenThermal)
    {
        std::map<int, std::string> areaClusterNames;
        if (!(areasThermalClustersMinStablePowerValidity(areas, areaClusterNames)))
        {
            throw InvalidParametersForThermalClusters(areaClusterNames);
        }
    }
    else
    {
        areas.each([](const auto& area) { area.thermal.checkAndCorrectAvailability(); });
    }
}

// Number of columns for Fuel & CO2 cost in thermal clusters must be one, or same as the number of
// TS
template<class ExceptionT>
static void checkThermalColumnNumber(const AreaList& areas,
                                     TimeSeries::TS EconomicInputData::*matrix)
{
    ExceptionT exception;
    bool error = false;
    for (uint areaIndex = 0; areaIndex < areas.size(); ++areaIndex)
    {
        const auto& area = *(areas.byIndex[areaIndex]);
        for (auto& cluster: area.thermal.list.each_enabled())
        {
            if (cluster->costgeneration == setManually)
            {
                continue;
            }
            const uint otherMatrixWidth = (cluster->ecoInput.*matrix).width;
            uint tsWidth = cluster->series.timeSeries.width;
            if (otherMatrixWidth != 1 && otherMatrixWidth != tsWidth)
            {
                logs.warning() << "Area: " << area.name << ". Cluster name: " << cluster->name()
                               << ". " << exception.what();
                error = true;
            }
        }
    }
    if (error)
    {
        throw exception;
    }
}

void checkFuelCostColumnNumber(const AreaList& areas)
{
    checkThermalColumnNumber<Antares::Error::IncompatibleFuelCostColumns>(
      areas,
      &EconomicInputData::fuelcost);
}

void checkCO2CostColumnNumber(const AreaList& areas)
{
    checkThermalColumnNumber<Antares::Error::IncompatibleCO2CostColumns>(
      areas,
      &EconomicInputData::co2cost);
}

IncompatibleDailyOptHeuristicForArea::IncompatibleDailyOptHeuristicForArea(const AreaName& name):
    LoadingError(
      std::string("Area ") + name.c_str()
      + " : simplex daily optimization and use heuristic target == no are not compatible")
{
}

std::string InvalidParametersForThermalClusters::buildMessage(
  const std::map<int, std::string>& clusterNames) const
{
    const std::string startMessage("Conflict between Min Stable Power, Pnom, spinning and capacity "
                                   "modulation for the following clusters : ");
    std::string clusters;
    for (const auto& it: clusterNames)
    {
        clusters += it.second.c_str();
        clusters += ";";
    }
    if (!clusters.empty())
    {
        clusters.pop_back(); // Remove final semicolon
    }
    return startMessage + clusters;
}

InvalidParametersForThermalClusters::InvalidParametersForThermalClusters(
  const std::map<int, std::string>& clusterNames):
    LoadingError(buildMessage(clusterNames))
{
}

void checkThermalRampingWithFastMode(const Parameters& params)
{
    if (params.include.thermal_ramping && params.unitCommitment.ucMode == ucHeuristicFast)
    {
        logs.warning()
          << "Thermal cluster ramping is enabled but 'unit-commitment-mode' is set to 'fast'. "
          << "Ramping constraints, ramping costs, and ramping variables are ignored in fast mode. "
          << "To enforce ramping, set 'unit-commitment-mode' to 'accurate' or 'milp'.";
    }
}
} // namespace Antares::Check
