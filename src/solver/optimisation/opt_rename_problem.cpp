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

Namer::Namer(std::vector<std::string>& target_names, LegacyInfoVec* legacyInfo):
    names_(target_names),
    legacyInfo_(legacyInfo)
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

std::string Namer::TimeIdentifier(const std::string& timeGranularity) const
{
    return timeGranularity + "<" + std::to_string(timeStep_) + ">";
}

std::string Namer::linkLocation() const
{
    return LocationIdentifier(origin_ + AREA_SEP + destination_, LINK);
}

std::string Namer::areaLocation() const
{
    return LocationIdentifier(area_.value(), AREA);
}

std::vector<std::string>& Namer::names() const
{
    return names_;
}

void Namer::RecordLegacyVariableInfo(unsigned index,
                                     const std::string& output,
                                     const std::string& component) const
{
    if (legacyInfo_ != nullptr)
    {
        (*legacyInfo_)[index] = {output, component, timeStep()};
    }
}

void Namer::SetLinkElementName(unsigned elementIndex, const std::string& elementType) const
{
    std::string location = linkLocation();
    std::string time = TimeIdentifier(HOUR);
    std::string name = BuildName(elementType, location, time);
    names_[elementIndex] = name;
    // Alphabetical order per the ST nomenclature
    const auto& [a1, a2] = (origin_ < destination_) ? std::tie(origin_, destination_)
                                                    : std::tie(destination_, origin_);
    RecordLegacyVariableInfo(elementIndex, elementType, a1 + "_" + a2 + "_link");
}

void Namer::SetAreaElementNameHour(unsigned elementIndex, const std::string& elementType) const
{
    SetAreaElementName(elementIndex, elementType, HOUR);
}

void Namer::SetAreaElementNameWeek(unsigned elementIndex, const std::string& elementType) const
{
    SetAreaElementName(elementIndex, elementType, WEEK);
}

void Namer::SetAreaElementName(unsigned elementIndex,
                               const std::string& elementType,
                               const std::string& timeGranularity) const
{
    std::string location = LocationIdentifier(area_.value(), AREA);
    std::string time = TimeIdentifier(timeGranularity);
    std::string name = BuildName(elementType, location, time);
    names_[elementIndex] = name;
    RecordLegacyVariableInfo(elementIndex, elementType, area_.value() + "_node");
}

void VariableNamer::SetAreaVariableName(unsigned varIndex,
                                        const std::string& variableType,
                                        int layerIndex) const
{
    std::string location = areaLocation() + SEP + "Layer<" + std::to_string(layerIndex) + ">";
    std::string time = TimeIdentifier(HOUR);
    std::string name = BuildName(variableType, location, time);
    names()[varIndex] = name;
    RecordLegacyVariableInfo(varIndex, variableType, std::to_string(layerIndex));
}

void Namer::SetThermalClusterElementName(unsigned varIndex,
                                         const std::string& elementType,
                                         const std::string& clusterName) const
{
    std::string location = areaLocation() + SEP + "ThermalCluster" + "<" + clusterName + ">";
    std::string time = TimeIdentifier(HOUR);
    std::string name = BuildName(elementType, location, time);
    names_[varIndex] = name;
    RecordLegacyVariableInfo(varIndex, elementType, area_.value() + "_thermal_" + clusterName);
}

void Namer::SetThermalClusterAndReserveElementName(unsigned varIndex,
                                                   const std::string& elementType,
                                                   const std::string& clusterName,
                                                   const std::string& reserveName) const
{
    std::string location = areaLocation() + SEP + "ThermalCluster" + "<" + clusterName + ">" + SEP
                           + "Reserve" + "<" + reserveName + ">";
    std::string time = TimeIdentifier(HOUR);
    std::string name = BuildName(elementType, location, time);
    names_[varIndex] = name;
    RecordLegacyVariableInfo(varIndex, elementType, area_.value() + "_thermal_" + clusterName);
}

void Namer::SetThermalClusterAndReservesElementName(unsigned varIndex,
                                                    const std::string& elementType,
                                                    const std::string& clusterName,
                                                    const std::string& reserveName1,
                                                    const std::string& reserveName2) const
{
    std::string location = areaLocation() + SEP + "ThermalCluster" + "<" + clusterName + ">" + SEP
                           + "Reserves" + "<" + reserveName1 + "," + reserveName2 + ">";
    std::string time = TimeIdentifier(HOUR);
    std::string name = BuildName(elementType, location, time);
    names_[varIndex] = name;
    RecordLegacyVariableInfo(varIndex, elementType, area_.value() + "_thermal_" + clusterName);
}

void Namer::SetSTStorageClusterElementName(unsigned varIndex,
                                           const std::string& elementType,
                                           const std::string& clusterName) const
{
    std::string location = areaLocation() + SEP + "ShortTerm Storage Cluster" + "<" + clusterName
                           + ">";
    std::string time = TimeIdentifier(HOUR);
    std::string name = BuildName(elementType, location, time);
    names_[varIndex] = name;
    RecordLegacyVariableInfo(varIndex,
                             elementType,
                             area_.value() + "_short_term_storage_" + clusterName);
}

void Namer::SetSTStorageClusterAndReserveElementName(unsigned varIndex,
                                                     const std::string& elementType,
                                                     const std::string& clusterName,
                                                     const std::string& reserveName) const
{
    std::string location = areaLocation() + SEP + "ShortTerm Storage Cluster" + "<" + clusterName
                           + ">" + SEP + "Reserve" + "<" + reserveName + ">";
    std::string time = TimeIdentifier(HOUR);
    std::string name = BuildName(elementType, location, time);
    names_[varIndex] = name;
    RecordLegacyVariableInfo(varIndex,
                             elementType,
                             area_.value() + "_short_term_storage_" + clusterName);
}

void Namer::SetHydroElementName(unsigned varIndex,
                                const std::string& elementType,
                                const std::string& clusterName) const
{
    std::string location = areaLocation() + SEP + "Hydro" + "<" + clusterName + ">";
    std::string time = TimeIdentifier(HOUR);
    std::string name = BuildName(elementType, location, time);
    names_[varIndex] = name;
    RecordLegacyVariableInfo(varIndex, elementType, area_.value() + "_hydro_storage");
}

void Namer::SetHydroAndReserveElementName(unsigned varIndex,
                                          const std::string& elementType,
                                          const std::string& clusterName,
                                          const std::string& reserveName) const
{
    std::string location = areaLocation() + SEP + "Hydro" + "<" + clusterName + ">" + SEP
                           + "Reserve" + "<" + reserveName + ">";
    std::string time = TimeIdentifier(HOUR);
    std::string name = BuildName(elementType, location, time);
    names_[varIndex] = name;
    RecordLegacyVariableInfo(varIndex, elementType, area_.value() + "_hydro_storage");
}

void Namer::SetThermalClusterReserveElementName(unsigned varIndex,
                                                const std::string& elementType,
                                                const std::string& reserveName) const
{
    std::string location = areaLocation() + SEP + "Reserve" + "<" + reserveName + ">";
    std::string time = TimeIdentifier(HOUR);
    std::string name = BuildName(elementType, location, time);
    names_[varIndex] = name;
    RecordLegacyVariableInfo(varIndex, elementType, reserveName);
}

void VariableNamer::DispatchableProduction(unsigned varIndex, const std::string& clusterName) const
{
    SetThermalClusterElementName(varIndex, "DispatchableProduction", clusterName);
}

void VariableNamer::ThermalClusterReserveParticipation(unsigned varIndex,
                                                       const std::string& clusterName,
                                                       const std::string& reserveName) const
{
    SetThermalClusterAndReserveElementName(varIndex,
                                           "ThermalClusterReserveParticipation",
                                           clusterName,
                                           reserveName);
}

void VariableNamer::ParticipationOfSTStorageReleaseToReserve(unsigned varIndex,
                                                             const std::string& clusterName,
                                                             const std::string& reserveName) const
{
    SetSTStorageClusterAndReserveElementName(varIndex,
                                             "ParticipationOfSTStorageReleaseToReserve",
                                             clusterName,
                                             reserveName);
}

void VariableNamer::ParticipationOfSTStorageStoreToReserve(unsigned varIndex,
                                                           const std::string& clusterName,
                                                           const std::string& reserveName) const
{
    SetSTStorageClusterAndReserveElementName(varIndex,
                                             "ParticipationOfSTStorageStoreToReserve",
                                             clusterName,
                                             reserveName);
}

void VariableNamer::ParticipationOfSTStorageToReserve(ReserveType type,
                                                      unsigned varIndex,
                                                      const std::string& clusterName,
                                                      const std::string& reserveName) const
{
    SetSTStorageClusterAndReserveElementName(varIndex,
                                             type == ReserveType::UP
                                               ? "ParticipationOfSTStorageToUpReserve"
                                               : "ParticipationOfSTStorageToDownReserve",
                                             clusterName,
                                             reserveName);
}

void VariableNamer::ParticipationOfHydroReleaseToReserve(unsigned varIndex,
                                                         const std::string& clusterName,
                                                         const std::string& reserveName) const
{
    SetHydroAndReserveElementName(varIndex,
                                  "ParticipationOfHydroReleaseToReserve",
                                  clusterName,
                                  reserveName);
}

void VariableNamer::ParticipationOfHydroStoreToReserve(unsigned varIndex,
                                                       const std::string& clusterName,
                                                       const std::string& reserveName) const
{
    SetHydroAndReserveElementName(varIndex,
                                  "ParticipationOfHydroStoreToReserve",
                                  clusterName,
                                  reserveName);
}

void VariableNamer::ParticipationOfHydroToReserve(ReserveType type,
                                                  unsigned varIndex,
                                                  const std::string& clusterName,
                                                  const std::string& reserveName) const
{
    if (type == ReserveType::UP)
    {
        SetHydroAndReserveElementName(varIndex,
                                      "ParticipationOfHydroToUpReserve",
                                      clusterName,
                                      reserveName);
    }

    else
    {
        SetHydroAndReserveElementName(varIndex,
                                      "ParticipationOfSTStorageToDownReserve",
                                      clusterName,
                                      reserveName);
    }
}

void VariableNamer::ParticipationOfRunningUnitsToReserve(unsigned varIndex,
                                                         const std::string& clusterName,
                                                         const std::string& reserveName) const
{
    SetThermalClusterAndReserveElementName(varIndex,
                                           "ParticipationOfRunningUnitsToReserve",
                                           clusterName,
                                           reserveName);
}

void VariableNamer::ParticipationOfOffUnitsToReserve(unsigned varIndex,
                                                     const std::string& clusterName,
                                                     const std::string& reserveName) const
{
    SetThermalClusterAndReserveElementName(varIndex,
                                           "ParticipationOfOffUnitsToReserve",
                                           clusterName,
                                           reserveName);
}

void VariableNamer::InternalUnsatisfiedReserve(unsigned varIndex,
                                               const std::string& reserveName) const
{
    SetThermalClusterReserveElementName(varIndex, "InternalUnsatisfiedReserve", reserveName);
}

void VariableNamer::InternalExcessReserve(unsigned varIndex, const std::string& reserveName) const
{
    SetThermalClusterReserveElementName(varIndex, "InternalExcessReserve", reserveName);
}

void VariableNamer::NODU(unsigned varIndex, const std::string& clusterName) const
{
    SetThermalClusterElementName(varIndex, "NODU", clusterName);
}

void VariableNamer::NumberStoppingDispatchableUnits(unsigned varIndex,
                                                    const std::string& clusterName) const
{
    SetThermalClusterElementName(varIndex, "NumberStoppingDispatchableUnits", clusterName);
}

void VariableNamer::NumberStartingDispatchableUnits(unsigned varIndex,
                                                    const std::string& clusterName) const
{
    SetThermalClusterElementName(varIndex, "NumberStartingDispatchableUnits", clusterName);
}

void VariableNamer::NumberBreakingDownDispatchableUnits(unsigned varIndex,
                                                        const std::string& clusterName) const
{
    SetThermalClusterElementName(varIndex, "NumberBreakingDownDispatchableUnits", clusterName);
}

void VariableNamer::DirectFlow(unsigned varIndex) const
{
    SetLinkElementName(varIndex, "DirectFlow");
}

void VariableNamer::PositiveDirectFlow(unsigned varIndex) const
{
    SetLinkElementName(varIndex, "PositiveDirectFlow");
}

void VariableNamer::PositiveIndirectFlow(unsigned varIndex) const
{
    SetLinkElementName(varIndex, "PositiveIndirectFlow");
}

void VariableNamer::SetShortTermStorageVariableName(unsigned varIndex,
                                                    const std::string& variableType,
                                                    const std::string& sts_name) const
{
    std::string location = areaLocation() + SEP + "ShortTermStorage" + "<" + sts_name + ">";
    std::string time = TimeIdentifier(HOUR);
    std::string name = BuildName(variableType, location, time);
    names()[varIndex] = name;
    RecordLegacyVariableInfo(varIndex, variableType, getArea() + "_short_term_storage_" + sts_name);
}

void VariableNamer::ShortTermStorageInjection(unsigned varIndex, const std::string& sts_name) const
{
    SetShortTermStorageVariableName(varIndex, "Injection", sts_name);
}

void VariableNamer::ShortTermStorageWithdrawal(unsigned varIndex, const std::string& sts_name) const
{
    SetShortTermStorageVariableName(varIndex, "Withdrawal", sts_name);
}

void VariableNamer::ShortTermStorageLevel(unsigned varIndex, const std::string& sts_name) const
{
    SetShortTermStorageVariableName(varIndex, "Level", sts_name);
}

void VariableNamer::ShortTermStorageOverflow(unsigned varIndex, const std::string& sts_name) const
{
    SetShortTermStorageVariableName(varIndex, "Overflow", sts_name);
}

void VariableNamer::ShortTermStorageCostVariationInjection(unsigned varIndex,
                                                           const std::string& sts_name) const
{
    SetShortTermStorageVariableName(varIndex, "CostVariationInjection", sts_name);
}

void VariableNamer::ShortTermStorageCostVariationWithdrawal(unsigned varIndex,
                                                            const std::string& sts_name) const
{
    SetShortTermStorageVariableName(varIndex, "CostVariationWithdrawal", sts_name);
}

void VariableNamer::HydProd(unsigned varIndex) const
{
    SetAreaElementNameHour(varIndex, "HydProd");
    overrideComponent(varIndex, getArea() + "_hydro_storage");
}

void VariableNamer::HydProdDown(unsigned varIndex) const
{
    SetAreaElementNameHour(varIndex, "HydProdDown");
    overrideComponent(varIndex, getArea() + "_hydro_storage");
}

void VariableNamer::HydProdUp(unsigned varIndex) const
{
    SetAreaElementNameHour(varIndex, "HydProdUp");
    overrideComponent(varIndex, getArea() + "_hydro_storage");
}

void VariableNamer::Pumping(unsigned varIndex) const
{
    SetAreaElementNameHour(varIndex, "Pumping");
    overrideComponent(varIndex, getArea() + "_hydro_storage");
}

void VariableNamer::HydroLevel(unsigned varIndex) const
{
    SetAreaElementNameHour(varIndex, "HydroLevel");
    overrideComponent(varIndex, getArea() + "_hydro_storage");
}

void VariableNamer::Overflow(unsigned varIndex) const
{
    SetAreaElementNameHour(varIndex, "Overflow");
    overrideComponent(varIndex, getArea() + "_hydro_storage");
}

void VariableNamer::LayerStorage(unsigned varIndex, int layerIndex) const
{
    SetAreaVariableName(varIndex, "LayerStorage", layerIndex);
    overrideComponent(varIndex, getArea() + "_hydro_storage");
}

void VariableNamer::FinalStorage(unsigned varIndex) const
{
    SetAreaElementNameHour(varIndex, "FinalStorage");
    overrideComponent(varIndex, getArea() + "_hydro_storage");
}

void VariableNamer::UnsuppliedEnergy(unsigned varIndex) const
{
    SetAreaElementNameHour(varIndex, "UnsuppliedEnergy");
    overrideComponent(varIndex, getArea() + "_node");
}

void VariableNamer::Spillage(unsigned varIndex) const
{
    SetAreaElementNameHour(varIndex, "Spillage");
    overrideComponent(varIndex, getArea() + "_node");
}

void VariableNamer::AreaBalance(unsigned varIndex) const
{
    SetAreaElementNameHour(varIndex, "AreaBalance");
    overrideComponent(varIndex, getArea() + "_node");
}

void ConstraintNamer::FlowDissociation(unsigned constrIndex) const
{
    SetLinkElementName(constrIndex, "FlowDissociation");
}

void ConstraintNamer::CsrFlowDissociation(unsigned constrIndex) const
{
    SetLinkElementName(constrIndex, "CsrFlowDissociation");
}

void ConstraintNamer::CsrAreaBalance(unsigned constrIndex) const
{
    SetAreaElementNameHour(constrIndex, "CsrAreaBalance");
}

void ConstraintNamer::AreaBalance(unsigned constrIndex) const
{
    SetAreaElementNameHour(constrIndex, "AreaBalance");
    overrideComponent(constrIndex, getArea() + "_node");
}

void ConstraintNamer::FictiveLoads(unsigned constrIndex) const
{
    SetAreaElementNameHour(constrIndex, "FictiveLoads");
}

void ConstraintNamer::CsrFictitiousLoad(unsigned constrIndex) const
{
    SetAreaElementNameHour(constrIndex, "CsrFictitiousLoad");
}

void ConstraintNamer::CsrMaxEnsLoad(unsigned constrIndex) const
{
    SetAreaElementNameHour(constrIndex, "CsrMaxEnsLoad");
}

void ConstraintNamer::MaxUnsuppliedEnergy(unsigned constrIndex) const
{
    SetAreaElementNameHour(constrIndex, "MaxUnsupEnergy");
}

void ConstraintNamer::HydroPower(unsigned constrIndex) const
{
    SetAreaElementNameWeek(constrIndex, "HydroPower");
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationSum(unsigned constrIndex) const
{
    SetAreaElementNameHour(constrIndex, "HydroPowerSmoothingUsingVariationSum");
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxDown(unsigned constrIndex) const
{
    SetAreaElementNameHour(constrIndex, "HydroPowerSmoothingUsingVariationMaxDown");
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxUp(unsigned constrIndex) const
{
    SetAreaElementNameHour(constrIndex, "HydroPowerSmoothingUsingVariationMaxUp");
}

void ConstraintNamer::MinHydroPower(unsigned constrIndex) const
{
    SetAreaElementNameWeek(constrIndex, "MinHydroPower");
}

void ConstraintNamer::MaxHydroPower(unsigned constrIndex) const
{
    SetAreaElementNameWeek(constrIndex, "MaxHydroPower");
}

void ConstraintNamer::MaxPumping(unsigned constrIndex) const
{
    SetAreaElementNameWeek(constrIndex, "MaxPumping");
}

void ConstraintNamer::AreaHydroLevel(unsigned constrIndex) const
{
    SetAreaElementNameHour(constrIndex, "AreaHydroLevel");
}

void ConstraintNamer::FinalStockEquivalent(unsigned constrIndex) const
{
    SetAreaElementNameHour(constrIndex, "FinalStockEquivalent");
}

void ConstraintNamer::FinalStockExpression(unsigned constrIndex) const
{
    SetAreaElementNameHour(constrIndex, "FinalStockExpression");
    overrideComponent(constrIndex, getArea() + "_hydro_storage");
}

void ConstraintNamer::BindingConstraint(
  unsigned constrIndex,
  const std::string& name,
  const std::pair<std::string, std::string>& timeGranularity) const
{
    std::string time = TimeIdentifier(timeGranularity.first);
    std::string new_name = BuildName(name, timeGranularity.second, time);
    names()[constrIndex] = new_name;
}

void ConstraintNamer::BindingConstraintHour(unsigned constrIndex, const std::string& name) const
{
    BindingConstraint(constrIndex, name, {HOUR, "hourly"});
}

void ConstraintNamer::BindingConstraintDay(unsigned constrIndex, const std::string& name) const
{
    BindingConstraint(constrIndex, name, {DAY, "daily"});
}

void ConstraintNamer::BindingConstraintWeek(unsigned constrIndex, const std::string& name) const
{
    BindingConstraint(constrIndex, name, {WEEK, "weekly"});
}

void ConstraintNamer::NbUnitsOutageLessThanNbUnitsStop(unsigned constrIndex,
                                                       const std::string& clusterName) const
{
    SetThermalClusterElementName(constrIndex, "NbUnitsOutageLessThanNbUnitsStop", clusterName);
}

void ConstraintNamer::NbDispUnitsMinBoundSinceMinUpTime(unsigned constrIndex,
                                                        const std::string& clusterName) const
{
    SetThermalClusterElementName(constrIndex, "NbDispUnitsMinBoundSinceMinUpTime", clusterName);
}

void ConstraintNamer::MinDownTime(unsigned constrIndex, const std::string& clusterName) const
{
    SetThermalClusterElementName(constrIndex, "MinDownTime", clusterName);
}

void ConstraintNamer::PMaxReserve(unsigned constrIndex,
                                  const std::string& clusterName,
                                  const std::string& reserveName) const
{
    SetThermalClusterAndReserveElementName(constrIndex, "PMaxReserve", clusterName, reserveName);
}

void ConstraintNamer::ParticipationOfOffUnitsToReserve(unsigned constrIndex,
                                                       const std::string& clusterName,
                                                       const std::string& reserveName) const
{
    SetThermalClusterAndReserveElementName(constrIndex,
                                           "ParticipationOfOffUnitsToReserve",
                                           clusterName,
                                           reserveName);
}

void ConstraintNamer::ParticipationOfUnitsToReserve(unsigned constrIndex,
                                                    const std::string& clusterName,
                                                    const std::string& reserveName) const
{
    SetThermalClusterAndReserveElementName(constrIndex,
                                           "ParticipationOfUnitsToReserve",
                                           clusterName,
                                           reserveName);
}

void ConstraintNamer::SymmetryReserveParticipation(unsigned constrIndex,
                                                   const std::string& clusterName,
                                                   const std::string& reserveName1,
                                                   const std::string& reserveName2) const
{
    SetThermalClusterAndReservesElementName(constrIndex,
                                            "ParticipationOfUnitsToReserve",
                                            clusterName,
                                            reserveName1,
                                            reserveName2);
}

void ConstraintNamer::POffUnitsUpperBound(unsigned constrIndex,
                                          const std::string& clusterName) const
{
    SetThermalClusterElementName(constrIndex, "POffUnitsUpperBound", clusterName);
}

void ConstraintNamer::POutCapacityThresholdInf(unsigned constrIndex,
                                               const std::string& clusterName) const
{
    SetThermalClusterElementName(constrIndex, "POutCapacityThresholdInf", clusterName);
}

void ConstraintNamer::POutCapacityThresholdSup(unsigned constrIndex,
                                               const std::string& clusterName) const
{
    SetThermalClusterElementName(constrIndex, "POutCapacityThresholdSup", clusterName);
}

void ConstraintNamer::POutBoundMin(unsigned constrIndex, const std::string& clusterName) const
{
    SetThermalClusterElementName(constrIndex, "POutBoundMin", clusterName);
}

void ConstraintNamer::POutBoundMax(unsigned constrIndex, const std::string& clusterName) const
{
    SetThermalClusterElementName(constrIndex, "POutBoundMax", clusterName);
}

void ConstraintNamer::STReserveParticipation(unsigned constrIndex,
                                             const std::string& clusterName,
                                             const std::string& reserveName,
                                             ReserveType type) const
{
    SetSTStorageClusterAndReserveElementName(constrIndex,
                                             type == ReserveType::UP ? "STReserveUpParticipation"
                                                                     : "STReserveDownParticipation",
                                             clusterName,
                                             reserveName);
}

void ConstraintNamer::STReleaseMaxReserve(unsigned constrIndex,
                                          const std::string& clusterName,
                                          const std::string& reserveName) const
{
    SetSTStorageClusterAndReserveElementName(constrIndex,
                                             "STReleaseMaxReserve",
                                             clusterName,
                                             reserveName);
}

void ConstraintNamer::STStoreMaxReserve(unsigned constrIndex,
                                        const std::string& clusterName,
                                        const std::string& reserveName) const
{
    SetSTStorageClusterAndReserveElementName(constrIndex,
                                             "STStoreMaxReserve",
                                             clusterName,
                                             reserveName);
}

void ConstraintNamer::STReleaseCapacityThresholdsUp(unsigned constrIndex,
                                                    const std::string& clusterName) const
{
    SetSTStorageClusterElementName(constrIndex, "STReleaseCapacityThresholdsUp", clusterName);
}

void ConstraintNamer::STStoreCapacityThresholdsUp(unsigned constrIndex,
                                                  const std::string& clusterName) const
{
    SetSTStorageClusterElementName(constrIndex, "STStoreCapacityThresholdsUp", clusterName);
}

void ConstraintNamer::STReleaseCapacityThresholdsDown(unsigned constrIndex,
                                                      const std::string& clusterName) const
{
    SetSTStorageClusterElementName(constrIndex, "STReleaseCapacityThresholdsDown", clusterName);
}

void ConstraintNamer::STStoreCapacityThresholdsDown(unsigned constrIndex,
                                                    const std::string& clusterName) const
{
    SetSTStorageClusterElementName(constrIndex, "STStoreCapacityThresholdsDown", clusterName);
}

void ConstraintNamer::STStorageLevelReserveParticipation(unsigned constrIndex,
                                                         const std::string& clusterName,
                                                         ReserveType type) const
{
    if (type == ReserveType::DOWN)
    {
        SetSTStorageClusterElementName(constrIndex,
                                       "STStorageLevelReserveParticipationDown",
                                       clusterName);
    }
    else if (type == ReserveType::UP)
    {
        SetSTStorageClusterElementName(constrIndex,
                                       "STStorageLevelReserveParticipationUP",
                                       clusterName);
    }
}

void ConstraintNamer::STEnergyStockLevelReserveParticipation(unsigned constrIndex,
                                                             const std::string& clusterName,
                                                             const std::string& reserveName) const
{
    SetSTStorageClusterAndReserveElementName(constrIndex,
                                             "STEnergyStockLevelReserveParticipation",
                                             clusterName,
                                             reserveName);
}

void ConstraintNamer::STGlobalEnergyStockLevelReserveParticipation(unsigned constrIndex,
                                                                   const std::string& clusterName,
                                                                   ReserveType type) const
{
    if (type == ReserveType::DOWN)
    {
        SetSTStorageClusterElementName(constrIndex,
                                       "STGlobalEnergyStockLevelReserveParticipationDown",
                                       clusterName);
    }
    else if (type == ReserveType::UP)
    {
        SetSTStorageClusterElementName(constrIndex,
                                       "STGlobalEnergyStockLevelReserveParticipationUp",
                                       clusterName);
    }
    else
    {
        throw std::runtime_error("Wrong type inside STGlobalEnergyStockLevelReserveParticipation");
    }
}

void ConstraintNamer::HydroReserveParticipation(ReserveType type,
                                                unsigned constrIndex,
                                                const std::string& clusterName,
                                                const std::string& reserveName) const
{
    SetHydroAndReserveElementName(constrIndex,
                                  type == ReserveType::UP ? "HydroReserveUpParticipation"
                                                          : "HydroReserveDownParticipation",
                                  clusterName,
                                  reserveName);
}

void ConstraintNamer::HydroReleaseMaxReserve(unsigned constrIndex,
                                             const std::string& clusterName,
                                             const std::string& reserveName) const
{
    SetHydroAndReserveElementName(constrIndex, "HydroReleaseMaxReserve", clusterName, reserveName);
}

void ConstraintNamer::HydroStoreMaxReserve(unsigned constrIndex,
                                           const std::string& clusterName,
                                           const std::string& reserveName) const
{
    SetHydroAndReserveElementName(constrIndex, "HydroStoreMaxReserve", clusterName, reserveName);
}

void ConstraintNamer::HydroReleaseCapacityThresholdsUp(unsigned constrIndex,
                                                       const std::string& clusterName) const
{
    SetHydroElementName(constrIndex, "HydroReleaseCapacityThresholdsUp", clusterName);
}

void ConstraintNamer::HydroStoreCapacityThresholdsUp(unsigned constrIndex,
                                                     const std::string& clusterName) const
{
    SetHydroElementName(constrIndex, "HydroStoreCapacityThresholdsUp", clusterName);
}

void ConstraintNamer::HydroReleaseCapacityThresholdsDown(unsigned constrIndex,
                                                         const std::string& clusterName) const
{
    SetHydroElementName(constrIndex, "HydroReleaseCapacityThresholdsDown", clusterName);
}

void ConstraintNamer::HydroStoreCapacityThresholdsDown(unsigned constrIndex,
                                                       const std::string& clusterName) const
{
    SetHydroElementName(constrIndex, "HydroStoreCapacityThresholdsDown", clusterName);
}

void ConstraintNamer::HydroLevelReserveParticipation(ReserveType type,
                                                     unsigned constrIndex,
                                                     const std::string& clusterName) const
{
    SetHydroElementName(constrIndex,
                        type == ReserveType::UP ? "HydroLevelReserveParticipationUp"
                                                : "HydroLevelReserveParticipationDown",
                        clusterName);
}

void ConstraintNamer::HydroEnergyLevelReserveParticipation(unsigned constrIndex,
                                                           const std::string& clusterName,
                                                           const std::string& reserveName) const
{
    SetHydroAndReserveElementName(constrIndex,
                                  "HydroEnergyLevelReserveParticipation",
                                  clusterName,
                                  reserveName);
}

void ConstraintNamer::HydroGlobalEnergyLevelReserveParticipationDown(
  unsigned constrIndex,
  const std::string& clusterName) const
{
    SetHydroElementName(constrIndex, "HydroGlobalEnergyLevelReserveParticipationDown", clusterName);
}

void ConstraintNamer::HydroGlobalEnergyLevelReserveParticipationUp(
  unsigned constrIndex,
  const std::string& clusterName) const
{
    SetHydroElementName(constrIndex, "HydroGlobalEnergyLevelReserveParticipationUp", clusterName);
}

void ConstraintNamer::ReserveSatisfaction(unsigned constrIndex,
                                          const std::string& reserveName) const
{
    SetThermalClusterReserveElementName(constrIndex, "ReserveSatisfaction", reserveName);
}

void ConstraintNamer::PMaxDispatchableGeneration(unsigned constrIndex,
                                                 const std::string& clusterName) const
{
    SetThermalClusterElementName(constrIndex, "PMaxDispatchableGeneration", clusterName);
}

void ConstraintNamer::PMinDispatchableGeneration(unsigned constrIndex,
                                                 const std::string& clusterName) const
{
    SetThermalClusterElementName(constrIndex, "PMinDispatchableGeneration", clusterName);
}

void ConstraintNamer::ConsistenceNODU(unsigned constrIndex, const std::string& clusterName) const
{
    SetThermalClusterElementName(constrIndex, "ConsistenceNODU", clusterName);
}

void ConstraintNamer::ShortTermStorageLevel(unsigned constrIndex, const std::string& sts_name) const
{
    std::string location = areaLocation() + SEP + "ShortTermStorage" + "<" + sts_name + ">";
    std::string time = TimeIdentifier(HOUR);
    std::string name = BuildName("Level", location, time);
    names()[constrIndex] = name;
}

void ConstraintNamer::ShortTermStorageCostVariation(const std::string& constraint_name,
                                                    unsigned constrIndex,
                                                    const std::string& sts_name) const
{
    std::string location = areaLocation() + SEP + "ShortTermStorage" + "<" + sts_name + ">";
    std::string time = TimeIdentifier(HOUR);
    std::string name = BuildName(constraint_name, location, time);
    names()[constrIndex] = name;
}

void ConstraintNamer::ShortTermStorageCumulation(const std::string& constraint_type,
                                                 unsigned constrIndex,
                                                 const std::string& sts_name,
                                                 const std::string& constraint_name) const
{
    std::string location = areaLocation() + SEP + "ShortTermStorage" + "<" + sts_name + ">" + SEP
                           + "Constraint" + "<" + constraint_name + ">";
    std::string time = TimeIdentifier(WEEK);
    std::string name = BuildName(constraint_type, location, time);
    names()[constrIndex] = name;
}
