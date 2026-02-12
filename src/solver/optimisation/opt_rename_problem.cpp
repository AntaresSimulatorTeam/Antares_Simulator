// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/opt_rename_problem.h"

#include <algorithm>
#include <map>

const std::string SEP = "::";
const std::string AREA_SEP = "$$";

const std::string HOUR("hour");
const std::string DAY("day");
const std::string WEEK("week");
const std::string LINK("link");
const std::string AREA("area");

std::string ShortTermStorageCumulationIdentifier(const std::string& name)
{
    return "Constraint<" + name + ">";
}

std::string LocationIdentifier(const std::string& location, const std::string& locationType)
{
    return locationType + "<" + location + ">";
}

std::string BuildName(const std::string& name,
                      const std::string& location,
                      const std::string& additional_identifier)
{
    std::string result = name + SEP + location + SEP + additional_identifier;
    std::replace(result.begin(), result.end(), ' ', '*');
    return result;
}

Namer::Namer(std::vector<std::string>& target_names):
    names_(target_names)
{
}

void Namer::UpdateTimeStep(unsigned timeStep)
{
    timeStep_ = timeStep;
}

void Namer::UpdateArea(const std::string& area)
{
    area_ = area;
}

void Namer::updateExtremities(const std::string& origin, const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
}

std::string Namer::TimeIdentifier(const std::string& timeGranularity)
{
    return timeGranularity + "<" + std::to_string(timeStep_) + ">";
}

std::string Namer::linkLocation()
{
    return LocationIdentifier(origin_ + AREA_SEP + destination_, LINK);
}

std::string Namer::areaLocation()
{
    return LocationIdentifier(area_, AREA);
}

std::vector<std::string>& Namer::names()
{
    return names_;
}

void Namer::SetLinkElementName(unsigned elementIndex, const std::string& elementType)
{
    std::string location = linkLocation();
    std::string time = TimeIdentifier(HOUR);
    std::string name = BuildName(elementType, location, time);
    names_[elementIndex] = name;
}

void Namer::SetAreaElementNameHour(unsigned elementIndex, const std::string& elementType)
{
    SetAreaElementName(elementIndex, elementType, HOUR);
}

void Namer::SetAreaElementNameWeek(unsigned elementIndex, const std::string& elementType)
{
    SetAreaElementName(elementIndex, elementType, WEEK);
}

void Namer::SetAreaElementName(unsigned elementIndex,
                               const std::string& elementType,
                               const std::string& timeGranularity)
{
    std::string location = LocationIdentifier(area_, AREA);
    std::string time = TimeIdentifier(timeGranularity);
    std::string name = BuildName(elementType, location, time);
    names_[elementIndex] = name;
}

void VariableNamer::SetAreaVariableName(unsigned varIndex,
                                        const std::string& variableType,
                                        int layerIndex)
{
    std::string location = areaLocation() + SEP + "Layer<" + std::to_string(layerIndex) + ">";
    std::string time = TimeIdentifier(HOUR);
    std::string name = BuildName(variableType, location, time);
    names()[varIndex] = name;
}

void Namer::SetThermalClusterElementName(unsigned varIndex,
                                         const std::string& elementType,
                                         const std::string& clusterName)
{
    std::string location = areaLocation() + SEP + "ThermalCluster" + "<" + clusterName + ">";
    std::string time = TimeIdentifier(HOUR);
    std::string name = BuildName(elementType, location, time);
    names_[varIndex] = name;
}

void VariableNamer::DispatchableProduction(unsigned varIndex, const std::string& clusterName)
{
    SetThermalClusterElementName(varIndex, "DispatchableProduction", clusterName);
}

void VariableNamer::NODU(unsigned varIndex, const std::string& clusterName)
{
    SetThermalClusterElementName(varIndex, "NODU", clusterName);
}

void VariableNamer::NumberStoppingDispatchableUnits(unsigned varIndex,
                                                    const std::string& clusterName)
{
    SetThermalClusterElementName(varIndex, "NumberStoppingDispatchableUnits", clusterName);
}

void VariableNamer::NumberStartingDispatchableUnits(unsigned varIndex,
                                                    const std::string& clusterName)
{
    SetThermalClusterElementName(varIndex, "NumberStartingDispatchableUnits", clusterName);
}

void VariableNamer::NumberBreakingDownDispatchableUnits(unsigned varIndex,
                                                        const std::string& clusterName)
{
    SetThermalClusterElementName(varIndex, "NumberBreakingDownDispatchableUnits", clusterName);
}

void VariableNamer::NTCDirect(unsigned varIndex)
{
    SetLinkElementName(varIndex, "NTCDirect");
}

void VariableNamer::IntercoDirectCost(unsigned varIndex)
{
    SetLinkElementName(varIndex, "IntercoDirectCost");
}

void VariableNamer::IntercoIndirectCost(unsigned varIndex)
{
    SetLinkElementName(varIndex, "IntercoIndirectCost");
}

void VariableNamer::SetShortTermStorageVariableName(unsigned varIndex,
                                                    const std::string& variableType,
                                                    const std::string& sts_name)
{
    std::string location = areaLocation() + SEP + "ShortTermStorage" + "<" + sts_name + ">";
    std::string time = TimeIdentifier(HOUR);
    std::string name = BuildName(variableType, location, time);
    names()[varIndex] = name;
}

void VariableNamer::ShortTermStorageInjection(unsigned varIndex, const std::string& sts_name)
{
    SetShortTermStorageVariableName(varIndex, "Injection", sts_name);
}

void VariableNamer::ShortTermStorageWithdrawal(unsigned varIndex, const std::string& sts_name)
{
    SetShortTermStorageVariableName(varIndex, "Withdrawal", sts_name);
}

void VariableNamer::ShortTermStorageLevel(unsigned varIndex, const std::string& sts_name)
{
    SetShortTermStorageVariableName(varIndex, "Level", sts_name);
}

void VariableNamer::ShortTermStorageOverflow(unsigned varIndex, const std::string& sts_name)
{
    SetShortTermStorageVariableName(varIndex, "Overflow", sts_name);
}

void VariableNamer::ShortTermStorageCostVariationInjection(unsigned varIndex,
                                                           const std::string& sts_name)
{
    SetShortTermStorageVariableName(varIndex, "CostVariationInjection", sts_name);
}

void VariableNamer::ShortTermStorageCostVariationWithdrawal(unsigned varIndex,
                                                            const std::string& sts_name)
{
    SetShortTermStorageVariableName(varIndex, "CostVariationWithdrawal", sts_name);
}

void VariableNamer::HydProd(unsigned varIndex)
{
    SetAreaElementNameHour(varIndex, "HydProd");
}

void VariableNamer::HydProdDown(unsigned varIndex)
{
    SetAreaElementNameHour(varIndex, "HydProdDown");
}

void VariableNamer::HydProdUp(unsigned varIndex)
{
    SetAreaElementNameHour(varIndex, "HydProdUp");
}

void VariableNamer::Pumping(unsigned varIndex)
{
    SetAreaElementNameHour(varIndex, "Pumping");
}

void VariableNamer::HydroLevel(unsigned varIndex)
{
    SetAreaElementNameHour(varIndex, "HydroLevel");
}

void VariableNamer::Overflow(unsigned varIndex)
{
    SetAreaElementNameHour(varIndex, "Overflow");
}

void VariableNamer::LayerStorage(unsigned varIndex, int layerIndex)
{
    SetAreaVariableName(varIndex, "LayerStorage", layerIndex);
}

void VariableNamer::FinalStorage(unsigned varIndex)
{
    SetAreaElementNameHour(varIndex, "FinalStorage");
}

void VariableNamer::UnsuppliedEnergy(unsigned varIndex)
{
    // TODO : what is "PositiveUnsuppliedEnergy" ?
    // We mean "UnsuppliedEnergy". To be replaced.
    SetAreaElementNameHour(varIndex, "PositiveUnsuppliedEnergy");
}

void VariableNamer::Spillage(unsigned varIndex)
{
    // TODO : what is "NegativeUnsuppliedEnergy" ?
    // Should be replaced with "Spillage".
    SetAreaElementNameHour(varIndex, "NegativeUnsuppliedEnergy");
}

void VariableNamer::AreaBalance(unsigned varIndex)
{
    SetAreaElementNameHour(varIndex, "AreaBalance");
}

void ConstraintNamer::FlowDissociation(unsigned constrIndex)
{
    SetLinkElementName(constrIndex, "FlowDissociation");
}

void ConstraintNamer::CsrFlowDissociation(unsigned constrIndex)
{
    SetLinkElementName(constrIndex, "CsrFlowDissociation");
}

void ConstraintNamer::CsrAreaBalance(unsigned constrIndex)
{
    SetAreaElementNameHour(constrIndex, "CsrAreaBalance");
}

void ConstraintNamer::AreaBalance(unsigned constrIndex)
{
    SetAreaElementNameHour(constrIndex, "AreaBalance");
}

void ConstraintNamer::FictiveLoads(unsigned constrIndex)
{
    SetAreaElementNameHour(constrIndex, "FictiveLoads");
}

void ConstraintNamer::MaxUnsuppliedEnergy(unsigned constrIndex)
{
    SetAreaElementNameHour(constrIndex, "MaxUnsupEnergy");
}

void ConstraintNamer::HydroPower(unsigned constrIndex)
{
    SetAreaElementNameWeek(constrIndex, "HydroPower");
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationSum(unsigned constrIndex)
{
    SetAreaElementNameHour(constrIndex, "HydroPowerSmoothingUsingVariationSum");
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxDown(unsigned constrIndex)
{
    SetAreaElementNameHour(constrIndex, "HydroPowerSmoothingUsingVariationMaxDown");
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxUp(unsigned constrIndex)
{
    SetAreaElementNameHour(constrIndex, "HydroPowerSmoothingUsingVariationMaxUp");
}

void ConstraintNamer::MinHydroPower(unsigned constrIndex)
{
    SetAreaElementNameWeek(constrIndex, "MinHydroPower");
}

void ConstraintNamer::MaxHydroPower(unsigned constrIndex)
{
    SetAreaElementNameWeek(constrIndex, "MaxHydroPower");
}

void ConstraintNamer::MaxPumping(unsigned constrIndex)
{
    SetAreaElementNameWeek(constrIndex, "MaxPumping");
}

void ConstraintNamer::AreaHydroLevel(unsigned constrIndex)
{
    SetAreaElementNameHour(constrIndex, "AreaHydroLevel");
}

void ConstraintNamer::FinalStockEquivalent(unsigned constrIndex)
{
    SetAreaElementNameHour(constrIndex, "FinalStockEquivalent");
}

void ConstraintNamer::FinalStockExpression(unsigned constrIndex)
{
    SetAreaElementNameHour(constrIndex, "FinalStockExpression");
}

void ConstraintNamer::BindingConstraint(unsigned constrIndex,
                                        const std::string& name,
                                        const std::pair<std::string, std::string>& timeGranularity)
{
    std::string time = TimeIdentifier(timeGranularity.first);
    std::string new_name = BuildName(name, timeGranularity.second, time);
    names()[constrIndex] = new_name;
}

void ConstraintNamer::BindingConstraintHour(unsigned constrIndex, const std::string& name)
{
    BindingConstraint(constrIndex, name, {HOUR, "hourly"});
}

void ConstraintNamer::BindingConstraintDay(unsigned constrIndex, const std::string& name)
{
    BindingConstraint(constrIndex, name, {DAY, "daily"});
}

void ConstraintNamer::BindingConstraintWeek(unsigned constrIndex, const std::string& name)
{
    BindingConstraint(constrIndex, name, {WEEK, "weekly"});
}

void ConstraintNamer::NbUnitsOutageLessThanNbUnitsStop(unsigned constrIndex,
                                                       const std::string& clusterName)
{
    SetThermalClusterElementName(constrIndex, "NbUnitsOutageLessThanNbUnitsStop", clusterName);
}

void ConstraintNamer::NbDispUnitsMinBoundSinceMinUpTime(unsigned constrIndex,
                                                        const std::string& clusterName)
{
    SetThermalClusterElementName(constrIndex, "NbDispUnitsMinBoundSinceMinUpTime", clusterName);
}

void ConstraintNamer::MinDownTime(unsigned constrIndex, const std::string& clusterName)
{
    SetThermalClusterElementName(constrIndex, "MinDownTime", clusterName);
}

void ConstraintNamer::PMaxDispatchableGeneration(unsigned constrIndex,
                                                 const std::string& clusterName)
{
    SetThermalClusterElementName(constrIndex, "PMaxDispatchableGeneration", clusterName);
}

void ConstraintNamer::PMinDispatchableGeneration(unsigned constrIndex,
                                                 const std::string& clusterName)
{
    SetThermalClusterElementName(constrIndex, "PMinDispatchableGeneration", clusterName);
}

void ConstraintNamer::ConsistenceNODU(unsigned constrIndex, const std::string& clusterName)
{
    SetThermalClusterElementName(constrIndex, "ConsistenceNODU", clusterName);
}

void ConstraintNamer::ShortTermStorageLevel(unsigned constrIndex, const std::string& sts_name)
{
    std::string location = areaLocation() + SEP + "ShortTermStorage" + "<" + sts_name + ">";
    std::string time = TimeIdentifier(HOUR);
    std::string name = BuildName("Level", location, time);
    names()[constrIndex] = name;
}

void ConstraintNamer::ShortTermStorageCostVariation(const std::string& constraint_name,
                                                    unsigned constrIndex,
                                                    const std::string& sts_name)
{
    std::string location = areaLocation() + SEP + "ShortTermStorage" + "<" + sts_name + ">";
    std::string time = TimeIdentifier(HOUR);
    std::string name = BuildName(constraint_name, location, time);
    names()[constrIndex] = name;
}

void ConstraintNamer::ShortTermStorageCumulation(const std::string& constraint_type,
                                                 unsigned constrIndex,
                                                 const std::string& sts_name,
                                                 const std::string& constraint_name)
{
    std::string location = areaLocation() + SEP + "ShortTermStorage" + "<" + sts_name + ">" + SEP
                           + "Constraint" + "<" + constraint_name + ">";
    std::string time = TimeIdentifier(WEEK);
    std::string name = BuildName(constraint_type, location, time);
    names()[constrIndex] = name;
}
