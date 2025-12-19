/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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
#pragma once

#include <string>
#include <vector>

class Namer
{
public:
    explicit Namer(std::vector<std::string>& target_names);
    void UpdateTimeStep(unsigned timeStep);
    void UpdateArea(const std::string& area);
    void updateExtremities(const std::string& origin, const std::string& destination);

protected:
    void SetLinkElementName(unsigned varIndex, const std::string& variableType);
    void SetAreaElementNameHour(unsigned varIndex, const std::string& variableType);
    void SetAreaElementNameWeek(unsigned varIndex, const std::string& variableType);
    void SetAreaElementName(unsigned varIndex,
                            const std::string& variableType,
                            const std::string& timeGranularity);
    void SetThermalClusterElementName(unsigned varIndex,
                                      const std::string& variableType,
                                      const std::string& clusterName);
    std::string TimeIdentifier(const std::string& timeGranularity);
    std::string linkLocation();
    std::string areaLocation();
    std::vector<std::string>& names();

private:
    std::string origin_;
    std::string destination_;
    std::string area_;
    unsigned timeStep_ = 0;
    std::vector<std::string>& names_;
};

class VariableNamer: public Namer
{
public:
    using Namer::Namer;
    void DispatchableProduction(unsigned varIndex, const std::string& clusterName);
    void NODU(unsigned varIndex, const std::string& clusterName);
    void NumberStoppingDispatchableUnits(unsigned varIndex, const std::string& clusterName);
    void NumberStartingDispatchableUnits(unsigned varIndex, const std::string& clusterName);
    void NumberBreakingDownDispatchableUnits(unsigned varIndex, const std::string& clusterName);
    void NTCDirect(unsigned varIndex);
    void IntercoDirectCost(unsigned varIndex);
    void IntercoIndirectCost(unsigned varIndex);
    void ShortTermStorageInjection(unsigned varIndex, const std::string& sts_name);
    void ShortTermStorageWithdrawal(unsigned varIndex, const std::string& sts_name);
    void ShortTermStorageLevel(unsigned varIndex, const std::string& sts_name);
    void ShortTermStorageOverflow(unsigned varIndex, const std::string& sts_name);
    void ShortTermStorageCostVariationInjection(unsigned varIndex, const std::string& sts_name);
    void ShortTermStorageCostVariationWithdrawal(unsigned varIndex, const std::string& sts_name);
    void HydProd(unsigned varIndex);
    void HydProdDown(unsigned varIndex);
    void HydProdUp(unsigned varIndex);
    void Pumping(unsigned varIndex);
    void HydroLevel(unsigned varIndex);
    void Overflow(unsigned varIndex);
    void FinalStorage(unsigned varIndex);
    void LayerStorage(unsigned varIndex, int layerIndex);
    void PositiveUnsuppliedEnergy(unsigned varIndex);
    void NegativeUnsuppliedEnergy(unsigned varIndex);
    void AreaBalance(unsigned varIndex);

private:
    void SetAreaVariableName(unsigned varIndex, const std::string& variableType, int layerIndex);
    void SetShortTermStorageVariableName(unsigned varIndex,
                                         const std::string& variableType,
                                         const std::string& sts_name);
};

class ConstraintNamer: public Namer
{
public:
    using Namer::Namer;

    void FlowDissociation(unsigned constrIndex);
    void AreaBalance(unsigned constrIndex);
    void FictiveLoads(unsigned constrIndex);
    void HydroPower(unsigned constrIndex);
    void HydroPowerSmoothingUsingVariationSum(unsigned constrIndex);
    void HydroPowerSmoothingUsingVariationMaxDown(unsigned constrIndex);
    void HydroPowerSmoothingUsingVariationMaxUp(unsigned constrIndex);
    void MinHydroPower(unsigned constrIndex);
    void MaxHydroPower(unsigned constrIndex);
    void MaxPumping(unsigned constrIndex);
    void AreaHydroLevel(unsigned constrIndex);
    void FinalStockEquivalent(unsigned constrIndex);
    void FinalStockExpression(unsigned constrIndex);
    void NbUnitsOutageLessThanNbUnitsStop(unsigned constrIndex, const std::string& clusterName);
    void NbDispUnitsMinBoundSinceMinUpTime(unsigned constrIndex, const std::string& clusterName);
    void MinDownTime(unsigned constrIndex, const std::string& clusterName);
    void PMaxDispatchableGeneration(unsigned constrIndex, const std::string& clusterName);
    void PMinDispatchableGeneration(unsigned constrIndex, const std::string& clusterName);
    void ConsistenceNODU(unsigned constrIndex, const std::string& clusterName);
    void ShortTermStorageLevel(unsigned constrIndex, const std::string& name);
    void BindingConstraintHour(unsigned constrIndex, const std::string& name);
    void BindingConstraintDay(unsigned constrIndex, const std::string& name);
    void BindingConstraintWeek(unsigned constrIndex, const std::string& name);
    void CsrFlowDissociation(unsigned constrIndex);
    void CsrAreaBalance(unsigned constrIndex);
    void ShortTermStorageCostVariation(const std::string& constrIndex_name,
                                       unsigned constrIndex,
                                       const std::string& sts_name);
    void ShortTermStorageCumulation(const std::string& constraint_type,
                                    unsigned constrIndex,
                                    const std::string& sts_name,
                                    const std::string& constrIndex_name);

private:
    void BindingConstraint(unsigned constrIndex,
                           const std::string& name,
                           const std::pair<std::string, std::string>& timeGranularity);
};
