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

#include <antares/exception/InitializationError.hpp>
#include <antares/exception/LoadingError.hpp>
#include <version.h>

#include "checkLoadedInputData.h"
#include "area.h"

namespace Antares::Check
{
void checkStudyVersion(const AnyString& optStudyFolder)
{
    using namespace Antares::Data;
    auto version = StudyTryToFindTheVersion(optStudyFolder);
    if (version == versionUnknown)
    {
        throw Error::InvalidStudy(optStudyFolder);
    }
    else
    {
        if ((uint)version > (uint)versionLatest)
        {
            throw Error::InvalidVersion(VersionToCStr(version),
                                        VersionToCStr(versionLatest));
        }
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
        areas.each([](Antares::Data::Area& area) { area.thermal.checkAndCorrectAvailability(); });
    }
}

// Number of columns for Fuel & CO2 cost in thermal clusters must be one, or same as the number of
// TS
template<class ExceptionT>
static void checkThermalColumnNumber(const Antares::Data::AreaList& areas,
                                     Matrix<double> Antares::Data::EconomicInputData::*matrix)
{
    ExceptionT exception;
    bool error = false;
    for (uint areaIndex = 0; areaIndex < areas.size(); ++areaIndex)
    {
        const auto& area = *(areas.byIndex[areaIndex]);
        for (uint clusterIndex = 0; clusterIndex != area.thermal.clusterCount(); ++clusterIndex)
        {
            const auto& cluster = *(area.thermal.clusters[clusterIndex]);
            if (cluster.costgeneration == Antares::Data::setManually)
                continue;
            const uint otherMatrixWidth = (cluster.ecoInput.*matrix).width;
            uint tsWidth = cluster.series->timeSeries.width;
            if (otherMatrixWidth != 1 && otherMatrixWidth != tsWidth)
            {
                logs.warning() << "Area: " << area.name << ". Cluster name: " << cluster.name()
                               << ". " << exception.what();
                error = true;
            }
        }
    }
    if (error)
        throw exception;
}

void checkFuelCostColumnNumber(const Antares::Data::AreaList& areas)
{
    checkThermalColumnNumber<Antares::Error::IncompatibleFuelCostColumns>(areas,
                             &Antares::Data::EconomicInputData::fuelcost);
}

void checkCO2CostColumnNumber(const Antares::Data::AreaList& areas)
{
    checkThermalColumnNumber<Antares::Error::IncompatibleCO2CostColumns>(areas,
                             &Antares::Data::EconomicInputData::co2cost);
}

} // namespace Antares::Check
