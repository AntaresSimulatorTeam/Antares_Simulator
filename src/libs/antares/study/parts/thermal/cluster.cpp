// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/thermal/cluster.h"

#include <algorithm>
#include <cassert>
#include <numeric>

#include <yuni/yuni.h>
#include <yuni/io/file.h>

#include <antares/inifile/inifile.h>
#include <antares/logs/logs.h>
#include <antares/solver/ts-generator/law.h>
#include <antares/utils/utils.h>
#include "antares/array/matrix.h"
#include "antares/study/parts/thermal/cluster.h"
#include "antares/study/study.h"

using namespace Yuni;
using namespace Antares;

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

namespace Antares::Data
{
Data::ThermalCluster::ThermalCluster(Area* parent):
    Cluster(parent),
    PthetaInf(HOURS_PER_YEAR, 0)
{
    // assert
    assert(parent && "A parent for a thermal dispatchable cluster can not be null");
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
    if (tsGenBehavior == LocalTSGenerationBehavior::forceNoGen)
    {
        return;
    }

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
    ret = checkModulation() && ret;

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

bool ThermalCluster::checkModulation()
{
    std::string buffer = "Thermal cluster: " + parentArea->name + '/' + pName + ": Modulation";
    if (modulation.width and modulation.height)
    {
        for (unsigned x = 0; x < modulation.width; ++x)
        {
            for (unsigned y = 0; y < modulation.height; ++y)
            {
                if (modulation[x][y] < 0.)
                {
                    logs.error() << buffer << ": Negative value detected (at the position " << x
                                 << ',' << y << ')';
                    return false;
                }
            }
        }
    }
    return true;
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
    std::call_once(onceFlag,
                   [&]
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
                   });
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

} // namespace Antares::Data
