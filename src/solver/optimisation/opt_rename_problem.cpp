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

#include "antares/solver/optimisation/opt_rename_problem.h"

#include <map>
#include <string>

const std::string HOUR("hour");
const std::string DAY("day");
const std::string WEEK("week");
const std::map<std::string, std::string> BindingConstraintTimeGranularity = {{HOUR, "hourly"},
                                                                             {DAY, "daily"},
                                                                             {WEEK, "weekly"}};
const std::string LINK("link");
const std::string AREA("area");

std::string BuildName(const std::string& name,
                      const std::string& location,
                      const std::string& additional_identifier)
{
    std::string result = name + SEPARATOR + location + SEPARATOR + additional_identifier;
    std::replace(result.begin(), result.end(), ' ', '*');
    return result;
}

void Namer::SetLinkElementName(unsigned int element, const std::string& elementType)
{
    const auto location = origin_ + AREA_SEP + destination_;
    targetUpdater_.UpdateTargetAtIndex(BuildName(elementType,
                                                 LocationIdentifier(location, LINK),
                                                 TimeIdentifier(timeStep_, HOUR)),
                                       element);
}

void Namer::SetAreaElementNameHour(unsigned int element, const std::string& elementType)
{
    SetAreaElementName(element, elementType, HOUR);
}

void Namer::SetAreaElementNameWeek(unsigned int element, const std::string& elementType)
{
    SetAreaElementName(element, elementType, WEEK);
}

void Namer::SetAreaElementName(unsigned int element,
                               const std::string& elementType,
                               const std::string& timeStepType)
{
    targetUpdater_.UpdateTargetAtIndex(BuildName(elementType,
                                                 LocationIdentifier(area_, AREA),
                                                 TimeIdentifier(timeStep_, timeStepType)),
                                       element);
}

void VariableNamer::SetAreaVariableName(unsigned int variable,
                                        const std::string& variableType,
                                        int layerIndex)
{
    targetUpdater_.UpdateTargetAtIndex(BuildName(variableType,
                                                 LocationIdentifier(area_, AREA) + SEPARATOR
                                                   + "Layer<" + std::to_string(layerIndex) + ">",
                                                 TimeIdentifier(timeStep_, HOUR)),
                                       variable);
}

void Namer::SetThermalClusterElementName(unsigned int variable,
                                         const std::string& elementType,
                                         const std::string& clusterName)
{
    const auto location = LocationIdentifier(area_, AREA) + SEPARATOR + "ThermalCluster" + "<"
                          + clusterName + ">";

    targetUpdater_.UpdateTargetAtIndex(BuildName(elementType,
                                                 location,
                                                 TimeIdentifier(timeStep_, HOUR)),
                                       variable);
}

void Namer::SetThermalClusterAndReserveElementName(unsigned int variable,
                                                   const std::string& elementType,
                                                   const std::string& clusterName,
                                                   const std::string& reserveName)
{
    const auto location = LocationIdentifier(area_, AREA) + SEPARATOR + "ThermalCluster" + "<"
                          + clusterName + ">" + SEPARATOR + "Reserve" + "<" + reserveName + ">";

    targetUpdater_.UpdateTargetAtIndex(BuildName(elementType,
                                                 location,
                                                 TimeIdentifier(timeStep_, HOUR)),
                                       variable);
}

void Namer::SetThermalClusterAndReservesElementName(unsigned int variable,
                                                    const std::string& elementType,
                                                    const std::string& clusterName,
                                                    const std::string& reserveName1,
                                                    const std::string& reserveName2)
{
    const auto location = LocationIdentifier(area_, AREA) + SEPARATOR + "ThermalCluster" + "<"
                          + clusterName + ">" + SEPARATOR + "Reserves" + "<" + reserveName1 + ","
                          + reserveName2 + ">";

    targetUpdater_.UpdateTargetAtIndex(BuildName(elementType,
                                                 location,
                                                 TimeIdentifier(timeStep_, HOUR)),
                                       variable);
}

void Namer::SetSTStorageClusterElementName(unsigned int variable,
                                           const std::string& elementType,
                                           const std::string& clusterName)
{
    const auto location = LocationIdentifier(area_, AREA) + SEPARATOR + "ShortTerm Storage Cluster"
                          + "<" + clusterName + ">";

    targetUpdater_.UpdateTargetAtIndex(BuildName(elementType,
                                                 location,
                                                 TimeIdentifier(timeStep_, HOUR)),
                                       variable);
}

void Namer::SetSTStorageClusterAndReserveElementName(unsigned int variable,
                                                     const std::string& elementType,
                                                     const std::string& clusterName,
                                                     const std::string& reserveName)
{
    const auto location = LocationIdentifier(area_, AREA) + SEPARATOR + "ShortTerm Storage Cluster"
                          + "<" + clusterName + ">" + SEPARATOR + "Reserve" + "<" + reserveName
                          + ">";

    targetUpdater_.UpdateTargetAtIndex(BuildName(elementType,
                                                 location,
                                                 TimeIdentifier(timeStep_, HOUR)),
                                       variable);
}

void Namer::SetHydroElementName(unsigned int variable,
                                const std::string& elementType,
                                const std::string& clusterName)
{
    const auto location = LocationIdentifier(area_, AREA) + SEPARATOR + "Hydro" + "<" + clusterName
                          + ">";

    targetUpdater_.UpdateTargetAtIndex(BuildName(elementType,
                                                 location,
                                                 TimeIdentifier(timeStep_, HOUR)),
                                       variable);
}

void Namer::SetHydroAndReserveElementName(unsigned int variable,
                                          const std::string& elementType,
                                          const std::string& clusterName,
                                          const std::string& reserveName)
{
    const auto location = LocationIdentifier(area_, AREA) + SEPARATOR + "Hydro" + "<" + clusterName
                          + ">" + SEPARATOR + "Reserve" + "<" + reserveName + ">";

    targetUpdater_.UpdateTargetAtIndex(BuildName(elementType,
                                                 location,
                                                 TimeIdentifier(timeStep_, HOUR)),
                                       variable);
}

void Namer::SetThermalClusterReserveElementName(unsigned int variable,
                                                const std::string& elementType,
                                                const std::string& reserveName)
{
    const auto location = LocationIdentifier(area_, AREA) + SEPARATOR + "Reserve" + "<"
                          + reserveName + ">";

    targetUpdater_.UpdateTargetAtIndex(BuildName(elementType,
                                                 location,
                                                 TimeIdentifier(timeStep_, HOUR)),
                                       variable);
}

void VariableNamer::DispatchableProduction(unsigned int variable, const std::string& clusterName)
{
    SetThermalClusterElementName(variable, "DispatchableProduction", clusterName);
}

void VariableNamer::ThermalClusterReserveParticipation(unsigned int variable,
                                                       const std::string& clusterName,
                                                       const std::string& reserveName)
{
    SetThermalClusterAndReserveElementName(variable,
                                           "ThermalClusterReserveParticipation",
                                           clusterName,
                                           reserveName);
}

void VariableNamer::ParticipationOfSTStorageReleaseToReserve(unsigned int variable,
                                                             const std::string& clusterName,
                                                             const std::string& reserveName)
{
    SetSTStorageClusterAndReserveElementName(variable,
                                             "ParticipationOfSTStorageReleaseToReserve",
                                             clusterName,
                                             reserveName);
}

void VariableNamer::ParticipationOfSTStorageStoreToReserve(unsigned int variable,
                                                           const std::string& clusterName,
                                                           const std::string& reserveName)
{
    SetSTStorageClusterAndReserveElementName(variable,
                                             "ParticipationOfSTStorageStoreToReserve",
                                             clusterName,
                                             reserveName);
}

void VariableNamer::ParticipationOfSTStorageToReserve(ReserveType type,
                                                      unsigned int variable,
                                                      const std::string& clusterName,
                                                      const std::string& reserveName)
{
    SetSTStorageClusterAndReserveElementName(variable,
                                             type == ReserveType::UP
                                               ? "ParticipationOfSTStorageToUpReserve"
                                               : "ParticipationOfSTStorageToDownReserve",
                                             clusterName,
                                             reserveName);
}

void VariableNamer::ParticipationOfHydroReleaseToReserve(unsigned int variable,
                                                         const std::string& clusterName,
                                                         const std::string& reserveName)
{
    SetHydroAndReserveElementName(variable,
                                  "ParticipationOfHydroReleaseToReserve",
                                  clusterName,
                                  reserveName);
}

void VariableNamer::ParticipationOfHydroStoreToReserve(unsigned int variable,
                                                       const std::string& clusterName,
                                                       const std::string& reserveName)
{
    SetHydroAndReserveElementName(variable,
                                  "ParticipationOfHydroStoreToReserve",
                                  clusterName,
                                  reserveName);
}

void VariableNamer::ParticipationOfHydroToReserve(ReserveType type,
                                                  unsigned int variable,
                                                  const std::string& clusterName,
                                                  const std::string& reserveName)
{
    if (type == ReserveType::UP)
    {
        SetHydroAndReserveElementName(variable,
                                      "ParticipationOfHydroToUpReserve",
                                      clusterName,
                                      reserveName);
    }

    else
    {
        SetHydroAndReserveElementName(variable,
                                      "ParticipationOfSTStorageToDownReserve",
                                      clusterName,
                                      reserveName);
    }
}

void VariableNamer::ParticipationOfRunningUnitsToReserve(unsigned int variable,
                                                         const std::string& clusterName,
                                                         const std::string& reserveName)
{
    SetThermalClusterAndReserveElementName(variable,
                                           "ParticipationOfRunningUnitsToReserve",
                                           clusterName,
                                           reserveName);
}

void VariableNamer::ParticipationOfOffUnitsToReserve(unsigned int variable,
                                                     const std::string& clusterName,
                                                     const std::string& reserveName)
{
    SetThermalClusterAndReserveElementName(variable,
                                           "ParticipationOfOffUnitsToReserve",
                                           clusterName,
                                           reserveName);
}

void VariableNamer::InternalUnsatisfiedReserve(unsigned int variable,
                                               const std::string& reserveName)
{
    SetThermalClusterReserveElementName(variable, "InternalUnsatisfiedReserve", reserveName);
}

void VariableNamer::InternalExcessReserve(unsigned int variable, const std::string& reserveName)
{
    SetThermalClusterReserveElementName(variable, "InternalExcessReserve", reserveName);
}

void VariableNamer::NODU(unsigned int variable, const std::string& clusterName)
{
    SetThermalClusterElementName(variable, "NODU", clusterName);
}

void VariableNamer::PowerOfOffUnitsParticipatingToReserve(unsigned int variable,
                                                          const std::string& clusterName,
                                                          const std::string& reserveName)
{
    SetThermalClusterAndReserveElementName(variable,
                                           "PowerOfOffUnitsParticipatingToReserve",
                                           clusterName,
                                           reserveName);
}

void VariableNamer::NumberStoppingDispatchableUnits(unsigned int variable,
                                                    const std::string& clusterName)
{
    SetThermalClusterElementName(variable, "NumberStoppingDispatchableUnits", clusterName);
}

void VariableNamer::NumberStartingDispatchableUnits(unsigned int variable,
                                                    const std::string& clusterName)
{
    SetThermalClusterElementName(variable, "NumberStartingDispatchableUnits", clusterName);
}

void VariableNamer::NumberBreakingDownDispatchableUnits(unsigned int variable,
                                                        const std::string& clusterName)
{
    SetThermalClusterElementName(variable, "NumberBreakingDownDispatchableUnits", clusterName);
}

void VariableNamer::NTCDirect(unsigned int variable,
                              const std::string& origin,
                              const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
    SetLinkElementName(variable, "NTCDirect");
}

void VariableNamer::IntercoDirectCost(unsigned int variable,
                                      const std::string& origin,
                                      const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
    SetLinkElementName(variable, "IntercoDirectCost");
}

void VariableNamer::IntercoIndirectCost(unsigned int variable,
                                        const std::string& origin,
                                        const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
    SetLinkElementName(variable, "IntercoIndirectCost");
}

void VariableNamer::SetShortTermStorageVariableName(unsigned int variable,
                                                    const std::string& variableType,
                                                    const std::string& shortTermStorageName)
{
    const auto location = LocationIdentifier(area_, AREA) + SEPARATOR + "ShortTermStorage" + "<"
                          + shortTermStorageName + ">";
    targetUpdater_.UpdateTargetAtIndex(BuildName(variableType,
                                                 location,
                                                 TimeIdentifier(timeStep_, HOUR)),
                                       variable);
}

void VariableNamer::ShortTermStorageInjection(unsigned int variable,
                                              const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(variable, "Injection", shortTermStorageName);
}

void VariableNamer::ShortTermStorageWithdrawal(unsigned int variable,
                                               const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(variable, "Withdrawal", shortTermStorageName);
}

void VariableNamer::ShortTermStorageLevel(unsigned int variable,
                                          const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(variable, "Level", shortTermStorageName);
}

void VariableNamer::ShortTermStorageOverflow(unsigned int variable,
                                             const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(variable, "Overflow", shortTermStorageName);
}

void VariableNamer::ShortTermStorageCostVariationInjection(unsigned int variable,
                                                           const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(variable, "CostVariationInjection", shortTermStorageName);
}

void VariableNamer::ShortTermStorageCostVariationWithdrawal(unsigned int variable,
                                                            const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(variable, "CostVariationWithdrawal", shortTermStorageName);
}

void VariableNamer::HydProd(unsigned int variable)
{
    SetAreaElementNameHour(variable, "HydProd");
}

void VariableNamer::HydProdDown(unsigned int variable)
{
    SetAreaElementNameHour(variable, "HydProdDown");
}

void VariableNamer::HydProdUp(unsigned int variable)
{
    SetAreaElementNameHour(variable, "HydProdUp");
}

void VariableNamer::Pumping(unsigned int variable)
{
    SetAreaElementNameHour(variable, "Pumping");
}

void VariableNamer::HydroLevel(unsigned int variable)
{
    SetAreaElementNameHour(variable, "HydroLevel");
}

void VariableNamer::Overflow(unsigned int variable)
{
    SetAreaElementNameHour(variable, "Overflow");
}

void VariableNamer::LayerStorage(unsigned int variable, int layerIndex)
{
    SetAreaVariableName(variable, "LayerStorage", layerIndex);
}

void VariableNamer::FinalStorage(unsigned int variable)
{
    SetAreaElementNameHour(variable, "FinalStorage");
}

void VariableNamer::PositiveUnsuppliedEnergy(unsigned int variable)
{
    SetAreaElementNameHour(variable, "PositiveUnsuppliedEnergy");
}

void VariableNamer::NegativeUnsuppliedEnergy(unsigned int variable)
{
    SetAreaElementNameHour(variable, "NegativeUnsuppliedEnergy");
}

void VariableNamer::AreaBalance(unsigned int variable)
{
    SetAreaElementNameHour(variable, "AreaBalance");
}

void ConstraintNamer::FlowDissociation(unsigned int constraint,
                                       const std::string& origin,
                                       const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
    SetLinkElementName(constraint, "FlowDissociation");
}

void ConstraintNamer::CsrFlowDissociation(unsigned int constraint,
                                          const std::string& origin,
                                          const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
    SetLinkElementName(constraint, "CsrFlowDissociation");
}

void ConstraintNamer::CsrAreaBalance(unsigned int constraint)
{
    SetAreaElementNameHour(constraint, "CsrAreaBalance");
}

void ConstraintNamer::AreaBalance(unsigned int constraint)
{
    SetAreaElementNameHour(constraint, "AreaBalance");
}

void ConstraintNamer::FictiveLoads(unsigned int constraint)
{
    SetAreaElementNameHour(constraint, "FictiveLoads");
}

void ConstraintNamer::HydroPower(unsigned int constraint)
{
    SetAreaElementNameWeek(constraint, "HydroPower");
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationSum(unsigned int constraint)
{
    SetAreaElementNameHour(constraint, "HydroPowerSmoothingUsingVariationSum");
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxDown(unsigned int constraint)
{
    SetAreaElementNameHour(constraint, "HydroPowerSmoothingUsingVariationMaxDown");
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxUp(unsigned int constraint)
{
    SetAreaElementNameHour(constraint, "HydroPowerSmoothingUsingVariationMaxUp");
}

void ConstraintNamer::MinHydroPower(unsigned int constraint)
{
    SetAreaElementNameWeek(constraint, "MinHydroPower");
}

void ConstraintNamer::MaxHydroPower(unsigned int constraint)
{
    SetAreaElementNameWeek(constraint, "MaxHydroPower");
}

void ConstraintNamer::MaxPumping(unsigned int constraint)
{
    SetAreaElementNameWeek(constraint, "MaxPumping");
}

void ConstraintNamer::AreaHydroLevel(unsigned int constraint)
{
    SetAreaElementNameHour(constraint, "AreaHydroLevel");
}

void ConstraintNamer::FinalStockEquivalent(unsigned int constraint)
{
    SetAreaElementNameHour(constraint, "FinalStockEquivalent");
}

void ConstraintNamer::FinalStockExpression(unsigned int constraint)
{
    SetAreaElementNameHour(constraint, "FinalStockExpression");
}

void ConstraintNamer::nameWithTimeGranularity(unsigned int constraint,
                                              const std::string& name,
                                              const std::string& type)
{
    targetUpdater_.UpdateTargetAtIndex(BuildName(name,
                                                 BindingConstraintTimeGranularity.at(type),
                                                 TimeIdentifier(timeStep_, type)),
                                       constraint);
}

void ConstraintNamer::NbUnitsOutageLessThanNbUnitsStop(unsigned int constraint,
                                                       const std::string& clusterName)
{
    SetThermalClusterElementName(constraint, "NbUnitsOutageLessThanNbUnitsStop", clusterName);
}

void ConstraintNamer::NbDispUnitsMinBoundSinceMinUpTime(unsigned int constraint,
                                                        const std::string& clusterName)
{
    SetThermalClusterElementName(constraint, "NbDispUnitsMinBoundSinceMinUpTime", clusterName);
}

void ConstraintNamer::MinDownTime(unsigned int constraint, const std::string& clusterName)
{
    SetThermalClusterElementName(constraint, "MinDownTime", clusterName);
}

void ConstraintNamer::PMaxReserve(unsigned int constraint,
                                  const std::string& clusterName,
                                  const std::string& reserveName)
{
    SetThermalClusterAndReserveElementName(constraint, "PMaxReserve", clusterName, reserveName);
}

void ConstraintNamer::PowerOfOffUnitsParticipatingToReserve(unsigned int constraint,
                                                            const std::string& clusterName,
                                                            const std::string& reserveName)
{
    SetThermalClusterAndReserveElementName(constraint,
                                           "PowerOfOffUnitsParticipatingToReserve",
                                           clusterName,
                                           reserveName);
}

void ConstraintNamer::ParticipationOfUnitsToReserve(unsigned int constraint,
                                                    const std::string& clusterName,
                                                    const std::string& reserveName)
{
    SetThermalClusterAndReserveElementName(constraint,
                                           "ParticipationOfUnitsToReserve",
                                           clusterName,
                                           reserveName);
}

void ConstraintNamer::SymmetryReserveParticipation(unsigned int constraint,
                                                   const std::string& clusterName,
                                                   const std::string& reserveName1,
                                                   const std::string& reserveName2)
{
    SetThermalClusterAndReservesElementName(constraint,
                                            "ParticipationOfUnitsToReserve",
                                            clusterName,
                                            reserveName1,
                                            reserveName2);
}

void ConstraintNamer::POffUnitsUpperBound(unsigned int constraint, const std::string& clusterName)
{
    SetThermalClusterElementName(constraint, "POffUnitsUpperBound", clusterName);
}

void ConstraintNamer::POutCapacityThresholdInf(unsigned int constraint,
                                               const std::string& clusterName)
{
    SetThermalClusterElementName(constraint, "POutCapacityThresholdInf", clusterName);
}

void ConstraintNamer::POutCapacityThresholdSup(unsigned int constraint,
                                               const std::string& clusterName)
{
    SetThermalClusterElementName(constraint, "POutCapacityThresholdSup", clusterName);
}

void ConstraintNamer::POutBoundMin(unsigned int constraint, const std::string& clusterName)
{
    SetThermalClusterElementName(constraint, "POutBoundMin", clusterName);
}

void ConstraintNamer::POutBoundMax(unsigned int constraint, const std::string& clusterName)
{
    SetThermalClusterElementName(constraint, "POutBoundMax", clusterName);
}

void ConstraintNamer::STReserveParticipation(unsigned int constraint,
                                             const std::string& clusterName,
                                             const std::string& reserveName,
                                             ReserveType type)
{
    SetSTStorageClusterAndReserveElementName(constraint,
                                             type == ReserveType::UP ? "STReserveUpParticipation"
                                                                     : "STReserveDownParticipation",
                                             clusterName,
                                             reserveName);
}

void ConstraintNamer::STReleaseMaxReserve(unsigned int constraint,
                                          const std::string& clusterName,
                                          const std::string& reserveName)
{
    SetSTStorageClusterAndReserveElementName(constraint,
                                             "STReleaseMaxReserve",
                                             clusterName,
                                             reserveName);
}

void ConstraintNamer::STStoreMaxReserve(unsigned int constraint,
                                        const std::string& clusterName,
                                        const std::string& reserveName)
{
    SetSTStorageClusterAndReserveElementName(constraint,
                                             "STStoreMaxReserve",
                                             clusterName,
                                             reserveName);
}

void ConstraintNamer::STReleaseCapacityThresholdsUp(unsigned int constraint,
                                                    const std::string& clusterName)
{
    SetSTStorageClusterElementName(constraint, "STReleaseCapacityThresholdsUp", clusterName);
}

void ConstraintNamer::STStoreCapacityThresholdsUp(unsigned int constraint,
                                                  const std::string& clusterName)
{
    SetSTStorageClusterElementName(constraint, "STStoreCapacityThresholdsUp", clusterName);
}

void ConstraintNamer::STReleaseCapacityThresholdsDown(unsigned int constraint,
                                                      const std::string& clusterName)
{
    SetSTStorageClusterElementName(constraint, "STReleaseCapacityThresholdsDown", clusterName);
}

void ConstraintNamer::STStoreCapacityThresholdsDown(unsigned int constraint,
                                                    const std::string& clusterName)
{
    SetSTStorageClusterElementName(constraint, "STStoreCapacityThresholdsDown", clusterName);
}

void ConstraintNamer::STStorageLevelReserveParticipation(unsigned int constraint,
                                                         const std::string& clusterName,
                                                         ReserveType type)
{
    if (type == ReserveType::DOWN)
    {
        SetSTStorageClusterElementName(constraint,
                                       "STStorageLevelReserveParticipationDown",
                                       clusterName);
    }
    else if (type == ReserveType::UP)
    {
        SetSTStorageClusterElementName(constraint,
                                       "STStorageLevelReserveParticipationUP",
                                       clusterName);
    }
}

void ConstraintNamer::STEnergyStockLevelReserveParticipation(unsigned int constraint,
                                                             const std::string& clusterName,
                                                             const std::string& reserveName)
{
    SetSTStorageClusterAndReserveElementName(constraint,
                                             "STEnergyStockLevelReserveParticipation",
                                             clusterName,
                                             reserveName);
}

void ConstraintNamer::STGlobalEnergyStockLevelReserveParticipation(unsigned int constraint,
                                                                   const std::string& clusterName,
                                                                   ReserveType type)
{
    if (type == ReserveType::DOWN)
    {
        SetSTStorageClusterElementName(constraint,
                                       "STGlobalEnergyStockLevelReserveParticipationDown",
                                       clusterName);
    }
    else if (type == ReserveType::UP)
    {
        SetSTStorageClusterElementName(constraint,
                                       "STGlobalEnergyStockLevelReserveParticipationUp",
                                       clusterName);
    }
    else
    {
        throw std::runtime_error("Wrong type inside STGlobalEnergyStockLevelReserveParticipation");
    }
}

void ConstraintNamer::HydroReserveParticipation(ReserveType type,
                                                unsigned int constraint,
                                                const std::string& clusterName,
                                                const std::string& reserveName)
{
    SetHydroAndReserveElementName(constraint,
                                  type == ReserveType::UP ? "HydroReserveUpParticipation"
                                                          : "HydroReserveDownParticipation",
                                  clusterName,
                                  reserveName);
}

void ConstraintNamer::HydroReleaseMaxReserve(unsigned int constraint,
                                             const std::string& clusterName,
                                             const std::string& reserveName)
{
    SetHydroAndReserveElementName(constraint, "HydroReleaseMaxReserve", clusterName, reserveName);
}

void ConstraintNamer::HydroStoreMaxReserve(unsigned int constraint,
                                           const std::string& clusterName,
                                           const std::string& reserveName)
{
    SetHydroAndReserveElementName(constraint, "HydroStoreMaxReserve", clusterName, reserveName);
}

void ConstraintNamer::HydroReleaseCapacityThresholdsUp(unsigned int constraint,
                                                       const std::string& clusterName)
{
    SetHydroElementName(constraint, "HydroReleaseCapacityThresholdsUp", clusterName);
}

void ConstraintNamer::HydroStoreCapacityThresholdsUp(unsigned int constraint,
                                                     const std::string& clusterName)
{
    SetHydroElementName(constraint, "HydroStoreCapacityThresholdsUp", clusterName);
}

void ConstraintNamer::HydroReleaseCapacityThresholdsDown(unsigned int constraint,
                                                         const std::string& clusterName)
{
    SetHydroElementName(constraint, "HydroReleaseCapacityThresholdsDown", clusterName);
}

void ConstraintNamer::HydroStoreCapacityThresholdsDown(unsigned int constraint,
                                                       const std::string& clusterName)
{
    SetHydroElementName(constraint, "HydroStoreCapacityThresholdsDown", clusterName);
}

void ConstraintNamer::HydroLevelReserveParticipation(ReserveType type,
                                                     unsigned int constraint,
                                                     const std::string& clusterName)
{
    SetHydroElementName(constraint,
                        type == ReserveType::UP ? "HydroLevelReserveParticipationUp"
                                                : "HydroLevelReserveParticipationDown",
                        clusterName);
}

void ConstraintNamer::HydroEnergyLevelReserveParticipation(unsigned int constraint,
                                                           const std::string& clusterName,
                                                           const std::string& reserveName)
{
    SetHydroAndReserveElementName(constraint,
                                  "HydroEnergyLevelReserveParticipation",
                                  clusterName,
                                  reserveName);
}

void ConstraintNamer::HydroGlobalEnergyLevelReserveParticipationDown(unsigned int constraint,
                                                                     const std::string& clusterName)
{
    SetHydroElementName(constraint, "HydroGlobalEnergyLevelReserveParticipationDown", clusterName);
}

void ConstraintNamer::HydroGlobalEnergyLevelReserveParticipationUp(unsigned int constraint,
                                                                   const std::string& clusterName)
{
    SetHydroElementName(constraint, "HydroGlobalEnergyLevelReserveParticipationUp", clusterName);
}

void ConstraintNamer::ReserveSatisfaction(unsigned int constraint, const std::string& reserveName)
{
    SetThermalClusterReserveElementName(constraint, "ReserveSatisfaction", reserveName);
}

void ConstraintNamer::PMaxDispatchableGeneration(unsigned int constraint,
                                                 const std::string& clusterName)
{
    SetThermalClusterElementName(constraint, "PMaxDispatchableGeneration", clusterName);
}

void ConstraintNamer::PMinDispatchableGeneration(unsigned int constraint,
                                                 const std::string& clusterName)
{
    SetThermalClusterElementName(constraint, "PMinDispatchableGeneration", clusterName);
}

void ConstraintNamer::ConsistenceNODU(unsigned int constraint, const std::string& clusterName)
{
    SetThermalClusterElementName(constraint, "ConsistenceNODU", clusterName);
}

void ConstraintNamer::ShortTermStorageLevel(unsigned int constraint, const std::string& name)
{
    targetUpdater_.UpdateTargetAtIndex(BuildName("Level",
                                                 LocationIdentifier(area_, AREA) + SEPARATOR
                                                   + "ShortTermStorage" + "<" + name + ">",
                                                 TimeIdentifier(timeStep_, HOUR)),
                                       constraint);
}

void ConstraintNamer::BindingConstraintHour(unsigned int constraint, const std::string& name)
{
    nameWithTimeGranularity(constraint, name, HOUR);
}

void ConstraintNamer::CsrBindingConstraintHour(unsigned int constraint, const std::string& name)
{
    nameWithTimeGranularity(constraint, name, HOUR);
}

void ConstraintNamer::BindingConstraintDay(unsigned int constraint, const std::string& name)
{
    nameWithTimeGranularity(constraint, name, DAY);
}

void ConstraintNamer::BindingConstraintWeek(unsigned int constraint, const std::string& name)
{
    nameWithTimeGranularity(constraint, name, WEEK);
}

void ConstraintNamer::ShortTermStorageCostVariation(const std::string& constraint_name,
                                                    unsigned int constraint,
                                                    const std::string& short_term_name)
{
    targetUpdater_.UpdateTargetAtIndex(BuildName(constraint_name,
                                                 LocationIdentifier(area_, AREA) + SEPARATOR
                                                   + "ShortTermStorage" + "<" + short_term_name
                                                   + ">",
                                                 TimeIdentifier(timeStep_, HOUR)),
                                       constraint);
}

void ConstraintNamer::ShortTermStorageCumulation(const std::string& constraint_type,
                                                 unsigned int constraint,
                                                 const std::string& short_term_name,
                                                 const std::string& constraint_name)
{
    targetUpdater_.UpdateTargetAtIndex(
      BuildName(constraint_type,
                LocationIdentifier(area_, AREA) + SEPARATOR + "ShortTermStorage" + "<"
                  + short_term_name + ">",
                ShortTermStorageCumulationIdentifier(constraint_name)),
      constraint);
}
