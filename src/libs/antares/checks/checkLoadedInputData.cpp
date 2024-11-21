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

#include <antares/checks/checkLoadedInputData.h>
#include <antares/exception/InitializationError.hpp>
#include <antares/exception/LoadingError.hpp>
#include <antares/series/series.h>
#include <antares/study/area/area.h>
#include <antares/study/header.h>
#include <antares/study/version.h>

namespace Antares::Check
{
void checkSolverMILPincompatibility(Antares::Data::UnitCommitmentMode ucMode,
                                    const std::string& solverName)
{
    using namespace Antares::Data;
    if (ucMode == UnitCommitmentMode::ucMILP && solverName == "sirius")
    {
        throw Error::IncompatibleMILPOrtoolsSolver();
    }
}

void checkStudyVersion(const AnyString& optStudyFolder)
{
    using namespace Antares::Data;
    auto version = StudyHeader::tryToFindTheVersion(optStudyFolder);

    if (version == StudyVersion::unknown())
    {
        throw Error::InvalidStudy(optStudyFolder);
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
                throw Error::IncompatibleDailyOptHeuristicForArea(area.name);
            }
        }
    }
}

bool areasThermalClustersMinStablePowerValidity(const Antares::Data::AreaList& areas,
                                                std::map<int, YString>& areaClusterNames)
{
    YString areaname = "";
    bool resultat = true;
    auto endarea = areas.end();
    int count = 0;

    for (auto areait = areas.begin(); areait != endarea; areait++)
    {
        areaname = areait->second->name;
        logs.debug() << "areaname : " << areaname;

        std::vector<YString> clusternames;

        if (!areait->second->thermalClustersMinStablePowerValidity(clusternames))
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
        std::map<int, YString> areaClusterNames;
        if (!(areasThermalClustersMinStablePowerValidity(areas, areaClusterNames)))
        {
            throw Error::InvalidParametersForThermalClusters(areaClusterNames);
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

} // namespace Antares::Check
