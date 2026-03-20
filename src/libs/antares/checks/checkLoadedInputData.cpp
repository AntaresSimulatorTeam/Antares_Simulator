// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/checks/checkLoadedInputData.h>
#include <antares/exception/LoadingError.hpp>
#include <antares/series/series.h>
#include <antares/study/area/area.h>

namespace Antares::Check
{

void checkStudyVersion(const Data::StudyVersion& version, const AnyString& StudyFolder)
{
    using namespace Antares::Data;
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
void checkSimplexRangeHydroPricing(Antares::Data::SimplexOptimization optRange,
                                   Antares::Data::HydroPricingMode hpMode)
{
    using namespace Antares::Data;
    if (optRange == SimplexOptimization::sorDay && hpMode == HydroPricingMode::hpMILP)
    {
        throw Error::IncompatibleOptRangeHydroPricing();
    }
}

// CHECK incompatible de choix simultané des options « simplex range= daily » et «
// unit-commitment = MILP ».
void checkSimplexRangeUnitCommitmentMode(Antares::Data::SimplexOptimization optRange,
                                         Antares::Data::UnitCommitmentMode ucMode)
{
    if (optRange == Antares::Data::SimplexOptimization::sorDay
        && ucMode == Antares::Data::UnitCommitmentMode::ucMILP)
    {
        throw Error::IncompatibleOptRangeUCMode();
    }
}

// Daily simplex optimisation and any area's use heurictic target turned to "No" are not
// compatible.
void checkSimplexRangeHydroHeuristic(Antares::Data::SimplexOptimization optRange,
                                     const Antares::Data::AreaList& areas)
{
    if (optRange == Antares::Data::SimplexOptimization::sorDay)
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

bool areasThermalClustersMinStablePowerValidity(const Antares::Data::AreaList& areas,
                                                std::map<int, std::string>& areaClusterNames)
{
    YString areaname = "";
    bool resultat = true;
    int count = 0;
    for (const auto& [_, area]: areas)
    {
        areaname = area->name;
        logs.debug() << "areaname : " << areaname;

        std::vector<YString> clusternames;

        if (!area->thermalClustersMinStablePowerValidity(clusternames))
        {
            for (auto it = clusternames.begin(); it != clusternames.end(); it++)
            {
                logs.debug() << "areaname : " << areaname << " ; clustername : " << (*it);
                YString res = "Area : " + areaname + " cluster name : " + (*it).c_str();
                areaClusterNames.try_emplace(count++, res);
            }
            resultat = false;
        }
    }
    return resultat;
}

void checkMinStablePower(bool tsGenThermal, const Antares::Data::AreaList& areas)
{
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
static void checkThermalColumnNumber(
  const Antares::Data::AreaList& areas,
  Antares::Data::TimeSeries::TS Antares::Data::EconomicInputData::*matrix)
{
    ExceptionT exception;
    bool error = false;
    for (uint areaIndex = 0; areaIndex < areas.size(); ++areaIndex)
    {
        const auto& area = *(areas.byIndex[areaIndex]);
        for (auto& cluster: area.thermal.list.each_enabled())
        {
            if (cluster->costgeneration == Antares::Data::setManually)
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

void checkFuelCostColumnNumber(const Antares::Data::AreaList& areas)
{
    checkThermalColumnNumber<Antares::Error::IncompatibleFuelCostColumns>(
      areas,
      &Antares::Data::EconomicInputData::fuelcost);
}

void checkCO2CostColumnNumber(const Antares::Data::AreaList& areas)
{
    checkThermalColumnNumber<Antares::Error::IncompatibleCO2CostColumns>(
      areas,
      &Antares::Data::EconomicInputData::co2cost);
}

IncompatibleDailyOptHeuristicForArea::IncompatibleDailyOptHeuristicForArea(
  const Antares::Data::AreaName& name):
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
} // namespace Antares::Check
