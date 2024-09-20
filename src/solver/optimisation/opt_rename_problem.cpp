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

#include "antares/solver/optimisation/opt_rename_problem.h"

#include <map>
#include <sstream>

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
                      const std::string& timeIdentifier)
{
    std::string result = name + SEPARATOR + location + SEPARATOR + timeIdentifier;
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

    targetUpdater_.UpdateTargetAtIndex(
      BuildName(elementType, location, TimeIdentifier(timeStep_, HOUR)), variable);
}

void Namer::SetSTStorageClusterElementName(unsigned int variable,
                                           const std::string& elementType,
                                           const std::string& clusterName)
{
    const auto location = LocationIdentifier(area_, AREA) + SEPARATOR + "ShortTerm Storage Cluster"
                          + "<" + clusterName + ">";

    targetUpdater_.UpdateTargetAtIndex(
      BuildName(elementType, location, TimeIdentifier(timeStep_, HOUR)), variable);
}

void Namer::SetSTStorageClusterAndReserveElementName(unsigned int variable,
                                                     const std::string& elementType,
                                                     const std::string& clusterName,
                                                     const std::string& reserveName)
{
    const auto location = LocationIdentifier(area_, AREA) + SEPARATOR + "ShortTerm Storage Cluster"
                          + "<" + clusterName + ">" + SEPARATOR + "Reserve" + "<" + reserveName
                          + ">";

    targetUpdater_.UpdateTargetAtIndex(
      BuildName(elementType, location, TimeIdentifier(timeStep_, HOUR)), variable);
}

void Namer::SetLTStorageClusterElementName(unsigned int variable,
                                           const std::string& elementType,
                                           const std::string& clusterName)
{
    const auto location = LocationIdentifier(area_, AREA) + SEPARATOR + "LongTerm Storage Cluster"
                          + "<" + clusterName + ">";

    targetUpdater_.UpdateTargetAtIndex(
      BuildName(elementType, location, TimeIdentifier(timeStep_, HOUR)), variable);
}

void Namer::SetLTStorageClusterAndReserveElementName(unsigned int variable,
                                                     const std::string& elementType,
                                                     const std::string& clusterName,
                                                     const std::string& reserveName)
{
    const auto location = LocationIdentifier(area_, AREA) + SEPARATOR + "LongTerm Storage Cluster"
                          + "<" + clusterName + ">" + SEPARATOR + "Reserve" + "<" + reserveName
                          + ">";

    targetUpdater_.UpdateTargetAtIndex(
      BuildName(elementType, location, TimeIdentifier(timeStep_, HOUR)), variable);
}

void Namer::SetThermalClusterReserveElementName(unsigned int variable,
                                                const std::string& elementType,
                                                const std::string& reserveName)
{
    const auto location
      = LocationIdentifier(area_, AREA) + SEPARATOR + "Reserve" + "<" + reserveName + ">";

    targetUpdater_.UpdateTargetAtIndex(
      BuildName(elementType, location, TimeIdentifier(timeStep_, HOUR)), variable);
}

void VariableNamer::DispatchableProduction(unsigned int variable, const std::string& clusterName)
{
    SetThermalClusterElementName(variable, "DispatchableProduction", clusterName);
}

void VariableNamer::ParticipationOfUnitsToReserve(unsigned int variable,
                                                  const std::string& clusterName,
                                                  const std::string& reserveName)
{
    SetThermalClusterAndReserveElementName(
      variable, "ParticipationOfUnitsToReserve", clusterName, reserveName);
}

void VariableNamer::ParticipationOfSTStorageTurbiningToReserve(unsigned int variable,
                                                               const std::string& clusterName,
                                                               const std::string& reserveName)
{
    SetSTStorageClusterAndReserveElementName(
      variable, "ParticipationOfSTStorageTurbiningToReserve", clusterName, reserveName);
}

void VariableNamer::ParticipationOfSTStoragePumpingToReserve(unsigned int variable,
                                                             const std::string& clusterName,
                                                             const std::string& reserveName)
{
    SetSTStorageClusterAndReserveElementName(
      variable, "ParticipationOfSTStoragePumpingToReserve", clusterName, reserveName);
}

void VariableNamer::ParticipationOfSTStorageToUpReserve(unsigned int variable,
                                                        const std::string& clusterName,
                                                        const std::string& reserveName)
{
    SetSTStorageClusterAndReserveElementName(
      variable, "ParticipationOfSTStorageToUpReserve", clusterName, reserveName);
}

void VariableNamer::ParticipationOfSTStorageToDownReserve(unsigned int variable,
                                                          const std::string& clusterName,
                                                          const std::string& reserveName)
{
    SetSTStorageClusterAndReserveElementName(
      variable, "ParticipationOfSTStorageToDownReserve", clusterName, reserveName);
}

void VariableNamer::ParticipationOfLTStorageTurbiningToReserve(unsigned int variable,
                                                               const std::string& clusterName,
                                                               const std::string& reserveName)
{
    SetLTStorageClusterAndReserveElementName(
      variable, "ParticipationOfLTStorageTurbiningToReserve", clusterName, reserveName);
}

void VariableNamer::ParticipationOfLTStoragePumpingToReserve(unsigned int variable,
                                                             const std::string& clusterName,
                                                             const std::string& reserveName)
{
    SetLTStorageClusterAndReserveElementName(
      variable, "ParticipationOfLTStoragePumpingToReserve", clusterName, reserveName);
}

void VariableNamer::ParticipationOfLTStorageToUpReserve(unsigned int variable,
                                                        const std::string& clusterName,
                                                        const std::string& reserveName)
{
    SetLTStorageClusterAndReserveElementName(
      variable, "ParticipationOfLTStorageToUpReserve", clusterName, reserveName);
}

void VariableNamer::ParticipationOfLTStorageToDownReserve(unsigned int variable,
                                                          const std::string& clusterName,
                                                          const std::string& reserveName)
{
    SetLTStorageClusterAndReserveElementName(
      variable, "ParticipationOfSTStorageToDownReserve", clusterName, reserveName);
}

void VariableNamer::ParticipationOfRunningUnitsToReserve(unsigned int variable,
                                                         const std::string& clusterName,
                                                         const std::string& reserveName)
{
    SetThermalClusterAndReserveElementName(
      variable, "ParticipationOfRunningUnitsToReserve", clusterName, reserveName);
}

void VariableNamer::ParticipationOfOffUnitsToReserve(unsigned int variable,
                                                     const std::string& clusterName,
                                                     const std::string& reserveName)
{
    SetThermalClusterAndReserveElementName(
      variable, "ParticipationOfOffUnitsToReserve", clusterName, reserveName);
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

void VariableNamer::NumberOfOffUnitsParticipatingToReserve(unsigned int variable,
                                                           const std::string& clusterName,
                                                           const std::string& reserveName)
{
    SetThermalClusterReserveElementName(
      variable, "NumberOfOffUnitsParticipatingToReserve", clusterName);
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

void ConstraintNamer::NumberOfOffUnitsParticipatingToReserve(unsigned int constraint,
                                                              const std::string& clusterName,
                                                              const std::string& reserveName)
{
    SetThermalClusterAndReserveElementName(constraint, "NumberOfOffUnitsParticipatingToReserve", clusterName, reserveName);
}

void ConstraintNamer::ParticipationOfUnitsToReserve(unsigned int constraint,
                                                    const std::string& clusterName,
                                                    const std::string& reserveName)
{
    SetThermalClusterAndReserveElementName(
      constraint, "ParticipationOfUnitsToReserve", clusterName, reserveName);
}

void ConstraintNamer::POffUnitsLowerBound(unsigned int constraint,
                                          const std::string& clusterName,
                                          const std::string& reserveName)
{
    SetThermalClusterAndReserveElementName(
      constraint, "POffUnitsLowerBound", clusterName, reserveName);
}

void ConstraintNamer::POffUnitsUpperBound(unsigned int constraint,
                                          const std::string& clusterName,
                                          const std::string& reserveName)
{
    SetThermalClusterAndReserveElementName(
      constraint, "POffUnitsUpperBound", clusterName, reserveName);
}

void ConstraintNamer::POutCapacityThreasholdInf(unsigned int constraint,
                                  const std::string& clusterName)
{
    SetThermalClusterElementName(constraint, "POutCapacityThreasholdInf", clusterName);
}

void ConstraintNamer::POutCapacityThreasholdSup(unsigned int constraint,
                                  const std::string& clusterName)
{
    SetThermalClusterElementName(constraint, "POutCapacityThreasholdSup", clusterName);
}

void ConstraintNamer::POutBoundMin(unsigned int constraint, const std::string& clusterName)
{
    SetThermalClusterElementName(constraint, "POutBoundMin", clusterName);
}

void ConstraintNamer::POutBoundMax(unsigned int constraint, const std::string& clusterName)
{
    SetThermalClusterElementName(constraint, "POutBoundMax", clusterName);
}

void ConstraintNamer::STReserveUpParticipation(unsigned int constraint,
                                               const std::string& clusterName,
                                               const std::string& reserveName)
{
    SetSTStorageClusterAndReserveElementName(
      constraint, "STReserveUpParticipation", clusterName, reserveName);
}

void ConstraintNamer::STReserveDownParticipation(unsigned int constraint,
                                                 const std::string& clusterName,
                                                 const std::string& reserveName)
{
    SetSTStorageClusterAndReserveElementName(
      constraint, "STReserveDownParticipation", clusterName, reserveName);
}

void ConstraintNamer::STTurbiningMaxReserve(unsigned int constraint,
                                            const std::string& clusterName,
                                            const std::string& reserveName)
{
    SetSTStorageClusterAndReserveElementName(
      constraint, "STTurbiningMaxReserve", clusterName, reserveName);
}

void ConstraintNamer::STPumpingMaxReserve(unsigned int constraint,
                                          const std::string& clusterName,
                                          const std::string& reserveName)
{
    SetSTStorageClusterAndReserveElementName(
      constraint, "STPumpingMaxReserve", clusterName, reserveName);
}

void ConstraintNamer::STTurbiningCapacityThreasholdsUp(unsigned int constraint,
                                                     const std::string& clusterName)
{
    SetSTStorageClusterElementName(constraint, "STTurbiningCapacityThreasholdsUp", clusterName);
}

void ConstraintNamer::STPumpingCapacityThreasholdsUp(unsigned int constraint,
                                                   const std::string& clusterName)
{
    SetSTStorageClusterElementName(constraint, "STPumpingCapacityThreasholdsUp", clusterName);
}

void ConstraintNamer::STTurbiningCapacityThreasholdsDown(unsigned int constraint,
    const std::string& clusterName)
{
    SetSTStorageClusterElementName(constraint, "STTurbiningCapacityThreasholdsDown", clusterName);
}

void ConstraintNamer::STPumpingCapacityThreasholdsDown(unsigned int constraint,
    const std::string& clusterName)
{
    SetSTStorageClusterElementName(constraint, "STPumpingCapacityThreasholdsDown", clusterName);
}

void ConstraintNamer::LTReserveUpParticipation(unsigned int constraint,
                                               const std::string& clusterName,
                                               const std::string& reserveName)
{
    SetLTStorageClusterAndReserveElementName(
      constraint, "LTReserveUpParticipation", clusterName, reserveName);
}

void ConstraintNamer::LTReserveDownParticipation(unsigned int constraint,
                                                 const std::string& clusterName,
                                                 const std::string& reserveName)
{
    SetLTStorageClusterAndReserveElementName(
      constraint, "LTReserveDownParticipation", clusterName, reserveName);
}

void ConstraintNamer::LTTurbiningMaxReserve(unsigned int constraint,
                                            const std::string& clusterName,
                                            const std::string& reserveName)
{
    SetLTStorageClusterAndReserveElementName(
      constraint, "LTTurbiningMaxReserve", clusterName, reserveName);
}

void ConstraintNamer::LTPumpingMaxReserve(unsigned int constraint,
                                          const std::string& clusterName,
                                          const std::string& reserveName)
{
    SetLTStorageClusterAndReserveElementName(
      constraint, "LTPumpingMaxReserve", clusterName, reserveName);
}

void ConstraintNamer::LTTurbiningCapacityThreasholdsUp(unsigned int constraint,
                                                     const std::string& clusterName)
{
    SetSTStorageClusterElementName(constraint, "LTTurbiningCapacityThreasholdsUp", clusterName);
}

void ConstraintNamer::LTPumpingCapacityThreasholdsUp(unsigned int constraint,
                                                   const std::string& clusterName)
{
    SetSTStorageClusterElementName(constraint, "LTPumpingCapacityThreasholdsUp", clusterName);
}

void ConstraintNamer::LTTurbiningCapacityThreasholdsDown(unsigned int constraint,
                                                     const std::string& clusterName)
{
    SetSTStorageClusterElementName(constraint, "LTTurbiningCapacityThreasholdsDown", clusterName);
}

void ConstraintNamer::LTPumpingCapacityThreasholdsDown(unsigned int constraint,
                                                   const std::string& clusterName)
{
    SetSTStorageClusterElementName(constraint, "LTPumpingCapacityThreasholdsDown", clusterName);
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
