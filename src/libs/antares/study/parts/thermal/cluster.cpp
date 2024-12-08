/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include "antares/study/parts/thermal/cluster.h"

#include <algorithm>
#include <cassert>
#include <numeric>

#include <boost/algorithm/string/case_conv.hpp>

#include <yuni/yuni.h>
#include <yuni/io/file.h>

#include <antares/inifile/inifile.h>
#include <antares/logs/logs.h>
#include <antares/solver/ts-generator/law.h>
#include <antares/utils/utils.h>
#include "antares/study/parts/thermal/cluster.h"
#include "antares/study/study.h"

using namespace Yuni;
using namespace Antares;

#define THERMALAGGREGATELIST_INITIAL_CAPACITY 10

namespace Yuni::Extension::CString
{
bool Into<Antares::Data::StatisticalLaw>::Perform(AnyString string, TargetType& out)
{
    string.trim();
    if (string.empty())
    {
        return false;
    }

    if (string.equalsInsensitive("uniform"))
    {
        out = Antares::Data::LawUniform;
        return true;
    }
    if (string.equalsInsensitive("geometric"))
    {
        out = Antares::Data::LawGeometric;
        return true;
    }
    return false;
}

bool Into<Antares::Data::CostGeneration>::Perform(AnyString string, TargetType& out)
{
    string.trim();
    if (string.empty())
    {
        return false;
    }

    if (string.equalsInsensitive("setManually"))
    {
        out = Antares::Data::setManually;
        return true;
    }
    if (string.equalsInsensitive("useCostTimeseries"))
    {
        out = Antares::Data::useCostTimeseries;
        return true;
    }
    return false;
}

bool Into<Antares::Data::LocalTSGenerationBehavior>::Perform(AnyString string, TargetType& out)
{
    string.trim();
    if (string.empty())
    {
        return false;
    }

    if (string.equalsInsensitive("use global"))
    {
        out = Antares::Data::LocalTSGenerationBehavior::useGlobalParameter;
        return true;
    }
    if (string.equalsInsensitive("force generation"))
    {
        out = Antares::Data::LocalTSGenerationBehavior::forceGen;
        return true;
    }
    if (string.equalsInsensitive("force no generation"))
    {
        out = Antares::Data::LocalTSGenerationBehavior::forceNoGen;
        return true;
    }
    return false;
}

} // namespace Yuni::Extension::CString

namespace Antares
{
namespace Data
{
Data::ThermalCluster::ThermalCluster(Area* parent):
    Cluster(parent),
    PthetaInf(HOURS_PER_YEAR, 0)
{
    // assert
    assert(parent && "A parent for a thermal dispatchable cluster can not be null");
}

uint ThermalCluster::groupId() const
{
    return groupID;
}

void Data::ThermalCluster::copyFrom(const ThermalCluster& cluster)
{
    // Note: In this method, only the data can be copied (and not the name or
    //   the ID for example)

    // mustrun
    mustrun = cluster.mustrun;
    mustrunOrigin = cluster.mustrunOrigin;

    // group
    groupID = cluster.groupID;
    pGroup = cluster.pGroup;

    // Enabled
    enabled = cluster.enabled;

    // unit count
    unitCount = cluster.unitCount;
    // nominal capacity
    nominalCapacity = cluster.nominalCapacity;
    nominalCapacityWithSpinning = cluster.nominalCapacityWithSpinning;

    minDivModulation = cluster.minDivModulation;

    minStablePower = cluster.minStablePower;
    minUpTime = cluster.minUpTime;
    minDownTime = cluster.minDownTime;

    // spinning
    spinning = cluster.spinning;

    // emissions
    emissions = cluster.emissions;

    // efficiency
    fuelEfficiency = cluster.fuelEfficiency;

    // volatility
    forcedVolatility = cluster.forcedVolatility;
    plannedVolatility = cluster.plannedVolatility;
    // law
    forcedLaw = cluster.forcedLaw;
    plannedLaw = cluster.plannedLaw;

    // costs
    costgeneration = cluster.costgeneration;
    marginalCost = cluster.marginalCost;
    spreadCost = cluster.spreadCost;
    variableomcost = cluster.variableomcost;
    fixedCost = cluster.fixedCost;
    startupCost = cluster.startupCost;
    marketBidCost = cluster.marketBidCost;

    // modulation
    modulation = cluster.modulation;
    cluster.modulation.unloadFromMemory();

    // Making sure that the data related to the prepro and timeseries are present
    // prepro
    if (!prepro)
    {
        prepro = std::make_unique<PreproAvailability>(id(), unitCount);
    }

    prepro->copyFrom(*cluster.prepro);
    ecoInput.copyFrom(cluster.ecoInput);
    // timseries

    series.timeSeries = cluster.series.timeSeries;
    cluster.series.timeSeries.unloadFromMemory();
    series.timeseriesNumbers.clear();

    // The parent must be invalidated to make sure that the clusters are really
    // re-written at the next 'Save' from the user interface.
    if (parentArea)
    {
        parentArea->forceReload();
    }
}

static Data::ThermalCluster::ThermalDispatchableGroup stringToGroup(Data::ClusterName& newgrp)
{
    using namespace Antares::Data;
    const static std::map<ClusterName, ThermalCluster::ThermalDispatchableGroup> mapping = {
      {"nuclear", ThermalCluster::thermalDispatchGrpNuclear},
      {"lignite", ThermalCluster::thermalDispatchGrpLignite},
      {"hard coal", ThermalCluster::thermalDispatchGrpHardCoal},
      {"gas", ThermalCluster::thermalDispatchGrpGas},
      {"oil", ThermalCluster::thermalDispatchGrpOil},
      {"mixed fuel", ThermalCluster::thermalDispatchGrpMixedFuel},
      {"other", ThermalCluster::thermalDispatchGrpOther1},
      {"other 1", ThermalCluster::thermalDispatchGrpOther1},
      {"other 2", ThermalCluster::thermalDispatchGrpOther2},
      {"other 3", ThermalCluster::thermalDispatchGrpOther3},
      {"other 4", ThermalCluster::thermalDispatchGrpOther4}};

    boost::to_lower(newgrp);
    if (auto res = mapping.find(newgrp); res != mapping.end())
    {
        return res->second;
    }
    // assigning a default value
    return ThermalCluster::thermalDispatchGrpOther1;
}

void Data::ThermalCluster::setGroup(Data::ClusterName newgrp)
{
    if (newgrp.empty())
    {
        groupID = thermalDispatchGrpOther1;
        pGroup.clear();
        return;
    }
    pGroup = newgrp;
    groupID = stringToGroup(newgrp);
}

bool Data::ThermalCluster::forceReload(bool reload) const
{
    bool ret = true;
    ret = modulation.forceReload(reload) && ret;
    ret = series.forceReload(reload) && ret;
    if (prepro)
    {
        ret = prepro->forceReload(reload) && ret;
    }
    ret = ecoInput.forceReload(reload) && ret;
    return ret;
}

void Data::ThermalCluster::markAsModified() const
{
    modulation.markAsModified();
    series.markAsModified();
    if (prepro)
    {
        prepro->markAsModified();
    }
    ecoInput.markAsModified();
}

void Data::ThermalCluster::calculationOfSpinning()
{
    // nominal capacity (for solver)
    nominalCapacityWithSpinning = nominalCapacity;

    // Nothing to do if the spinning is equal to zero
    // because it will the same multiply all entries of the matrix by 1.
    if (!Utils::isZero(spinning))
    {
        logs.debug() << "  Calculation of spinning... " << parentArea->name << "::" << pName;

        auto& ts = series.timeSeries;
        // The formula
        // const double s = 1. - cluster.spinning / 100.; */

        // All values in the matrix will be multiply by this coeff
        // It is no really useful to test if the result of the formula
        // is equal to zero, since the method `Matrix::multiplyAllValuesBy()`
        // already does this test.
        nominalCapacityWithSpinning *= 1 - (spinning / 100.);
        ts.multiplyAllEntriesBy(1. - (spinning / 100.));
    }
}

void Data::ThermalCluster::reverseCalculationOfSpinning()
{
    // Nothing to do if the spinning is equal to zero
    // because it will the same multiply all entries of the matrix by 1.
    if (Utils::isZero(spinning))
    {
        return;
    }

    // No ts have been generated, no need to save them into input or it will apply spinning twice
    if (tsGenBehavior == LocalTSGenerationBehavior::forceNoGen)
    {
        return;
    }

    logs.debug() << "  Calculation of spinning (reverse)... " << parentArea->name << "::" << pName;

    auto& ts = series.timeSeries;
    // The formula
    // const double s = 1. - cluster.spinning / 100.;

    // All values in the matrix will be multiply by this coeff
    // It is no really useful to test if the result of the formula
    // is equal to zero, since the method `Matrix::multiplyAllValuesBy()`
    // already does this test.
    ts.multiplyAllEntriesBy(1. / (1. - (spinning / 100.)));
    ts.roundAllEntries();
}

void Data::ThermalCluster::reset()
{
    Cluster::reset();

    mustrun = false;
    mustrunOrigin = false;
    nominalCapacityWithSpinning = 0.;
    minDivModulation.isCalculated = false;
    minStablePower = 0.;
    minUpDownTime = 1;
    minUpTime = 1;
    minDownTime = 1;

    // spinning
    spinning = 0.;

    // efficiency
    fuelEfficiency = 100.0;

    // pollutant emissions array
    emissions.factors.fill(0);
    // volatility
    forcedVolatility = 0.;
    plannedVolatility = 0.;
    // laws
    plannedLaw = LawUniform;
    forcedLaw = LawUniform;

    // costs
    costgeneration = setManually;
    marginalCost = 0.;
    spreadCost = 0.;
    fixedCost = 0.;
    startupCost = 0.;
    marketBidCost = 0.;
    variableomcost = 0.;

    // modulation
    modulation.resize(thermalModulationMax, HOURS_PER_YEAR);
    modulation.fill(1.);
    modulation.fillColumn(thermalMinGenModulation, 0.);

    // prepro
    // warning: the variables `prepro` and `series` __must__ not be destroyed
    //   since the interface may still have a pointer to them.
    //   we must simply reset their content.
    if (!prepro)
    {
        prepro = std::make_unique<PreproAvailability>(id(), unitCount);
    }

    prepro->reset();
    ecoInput.reset();
}

bool Data::ThermalCluster::integrityCheck()
{
    if (!parentArea)
    {
        logs.error() << "Thermal cluster " << pName << ": The parent area is missing";
        return false;
    }

    if (std::isnan(marketBidCost))
    {
        logs.error() << "Thermal cluster " << pName << ": NaN detected for market bid cost";
        return false;
    }
    if (std::isnan(marginalCost))
    {
        logs.error() << "Thermal cluster " << parentArea->name << '/' << pName
                     << ": NaN detected for marginal cost";
        return false;
    }
    if (std::isnan(spreadCost))
    {
        logs.error() << "Thermal cluster " << parentArea->name << '/' << pName
                     << ": NaN detected for marginal cost";
        return false;
    }

    bool ret = true;

    if (nominalCapacity < 0.)
    {
        logs.error() << "Thermal cluster " << parentArea->name << "/" << pName
                     << ": The Nominal capacity must be positive or null";
        nominalCapacity = 0.;
        nominalCapacityWithSpinning = 0.;
        ret = false;
    }
    if (spinning < 0. or spinning > 100.)
    {
        if (spinning < 0.)
        {
            spinning = 0;
        }
        else
        {
            spinning = 100.;
        }
        logs.error() << "Thermal cluster: " << parentArea->name << '/' << pName
                     << ": The spinning must be within the range [0,+100] (rounded to " << spinning
                     << ')';
        ret = false;
        nominalCapacityWithSpinning = nominalCapacity;
    }
    // emissions
    for (auto i = 0; i < Pollutant::POLLUTANT_MAX; i++)
    {
        if (emissions.factors[i] < 0)
        {
            logs.error() << "Thermal cluster: " << parentArea->name << '/' << pName << ": The "
                         << Pollutant::getPollutantName(i) << " pollutant factor must be >= 0";
        }
    }
    if (fuelEfficiency <= 0. || fuelEfficiency > 100.)
    {
        fuelEfficiency = 100.;
        logs.error() << "Thermal cluster: " << parentArea->name << '/' << pName
                     << ": The efficiency must be within the range (0,+100] (rounded to "
                     << fuelEfficiency << ')';
        ret = false;
    }
    if (spreadCost < 0.)
    {
        logs.error() << "Thermal cluster: " << parentArea->name << '/' << pName
                     << ": The spread must be positive or null";
        spreadCost = 0.;
        ret = false;
    }
    if (variableomcost < 0.)
    {
        logs.error() << "Thermal cluster: " << parentArea->name << '/' << pName
                     << ": The variable operation & maintenance cost must be positive or null";
        variableomcost = 0.;
        ret = false;
    }

    // Modulation
    if (modulation.height > 0)
    {
        CString<ant_k_cluster_name_max_length + ant_k_area_name_max_length + 50, false> buffer;
        buffer << "Thermal cluster: " << parentArea->name << '/' << pName << ": Modulation";
        ret = MatrixTestForPositiveValues(buffer.c_str(), &modulation) && ret;
    }

    // la valeur minStablePower should not be modified
    /*
    if (minStablePower > nominalCapacity)
    {
            logs.error() << "Thermal cluster: " << parentArea->name << '/' << pName
                    << ": failed min stable power < nominal capacity (with min power = "
                    << minStablePower << ", nominal power = " << nominalCapacity;
            minStablePower = nominalCapacity;
            ret = false;
    }*/

    return ret;
}

const char* Data::ThermalCluster::GroupName(enum ThermalDispatchableGroup grp)
{
    switch (grp)
    {
    case thermalDispatchGrpNuclear:
        return "Nuclear";
    case thermalDispatchGrpLignite:
        return "Lignite";
    case thermalDispatchGrpHardCoal:
        return "Hard Coal";
    case thermalDispatchGrpGas:
        return "Gas";
    case thermalDispatchGrpOil:
        return "Oil";
    case thermalDispatchGrpMixedFuel:
        return "Mixed Fuel";
    case thermalDispatchGrpOther1:
        return "Other";
    case thermalDispatchGrpOther2:
        return "Other 2";
    case thermalDispatchGrpOther3:
        return "Other 3";
    case thermalDispatchGrpOther4:
        return "Other 4";

    case groupMax:
        return "";
    }
    return "";
}

void ThermalCluster::calculatMinDivModulation()
{
    minDivModulation.value = (modulation[thermalModulationCapacity][0]
                              / std::ceil(modulation[thermalModulationCapacity][0]));
    minDivModulation.index = 0;

    for (uint t = 1; t < modulation.height; t++)
    {
        double div = modulation[thermalModulationCapacity][t]
                     / ceil(modulation[thermalModulationCapacity][t]);

        if (div < minDivModulation.value)
        {
            minDivModulation.value = div;
            minDivModulation.index = t;
        }
    }
    minDivModulation.isCalculated = true;
}

bool ThermalCluster::checkMinStablePower()
{
    if (!minDivModulation.isCalculated) // not has been initialized
    {
        calculatMinDivModulation();
    }

    if (minDivModulation.value < 0)
    {
        minDivModulation.isValidated = false;
        return false;
    }

    // calculate nominalCapacityWithSpinning
    double nomCapacityWithSpinning = nominalCapacity * (1 - spinning / 101);

    if (Utils::isZero(1 - spinning / 101))
    {
        minDivModulation.border = .0;
    }
    else
    {
        minDivModulation.border = std::min(nomCapacityWithSpinning, minStablePower)
                                  / nomCapacityWithSpinning;
    }

    if (minDivModulation.value < minDivModulation.border)
    {
        minDivModulation.isValidated = false;
        return false;
    }

    minDivModulation.isValidated = true;
    return true;
}

bool ThermalCluster::checkMinStablePowerWithNewModulation(uint idx, double value)
{
    if (!minDivModulation.isCalculated || idx == minDivModulation.index)
    {
        calculatMinDivModulation();
    }
    else
    {
        double div = value / ceil(value);
        if (div < minDivModulation.value)
        {
            minDivModulation.value = div;
            minDivModulation.index = idx;
        }
    }

    return checkMinStablePower();
}

bool ThermalCluster::doWeGenerateTS(bool globalTSgeneration) const
{
    switch (tsGenBehavior)
    {
    case LocalTSGenerationBehavior::useGlobalParameter:
        return globalTSgeneration;
    case LocalTSGenerationBehavior::forceGen:
        return true;
    default:
        return false;
    }
}

unsigned int ThermalCluster::precision() const
{
    return 0;
}

CostProvider& ThermalCluster::getCostProvider()
{
    if (!costProvider)
    {
        switch (costgeneration)
        {
        case Data::setManually:
            costProvider = std::make_unique<ConstantCostProvider>(*this);
            break;
        case Data::useCostTimeseries:
            costProvider = std::make_unique<ScenarizedCostProvider>(*this);
            break;
        default:
            throw std::runtime_error("Invalid costgeneration parameter");
        }
    }
    return *costProvider;
}

void ThermalCluster::checkAndCorrectAvailability()
{
    const auto PmaxDUnGroupeDuPalierThermique = nominalCapacityWithSpinning;
    const auto PminDUnGroupeDuPalierThermique = (nominalCapacityWithSpinning < minStablePower)
                                                  ? nominalCapacityWithSpinning
                                                  : minStablePower;

    bool condition = false;
    bool report = false;

    for (uint y = 0; y != series.timeSeries.height; ++y)
    {
        for (uint x = 0; x != series.timeSeries.width; ++x)
        {
            auto rightpart = PminDUnGroupeDuPalierThermique
                             * ceil(series.timeSeries.entry[x][y] / PmaxDUnGroupeDuPalierThermique);
            condition = rightpart > series.timeSeries.entry[x][y];
            if (condition)
            {
                series.timeSeries.entry[x][y] = rightpart;
                report = true;
            }
        }
    }

    if (report)
    {
        logs.warning() << "Area : " << parentArea->name << " cluster name : " << name()
                       << " available power lifted to match Pmin and Pnom requirements";
    }
}

bool ThermalCluster::isActive() const
{
    return enabled && !mustrun;
}

} // namespace Data
} // namespace Antares
