// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>
#include <vector>

#include <antares/study/fwd.h>

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
    void SetThermalClusterAndReserveElementName(unsigned varIndex,
                                                const std::string& elementType,
                                                const std::string& clusterName,
                                                const std::string& reserveName);
    void SetThermalClusterAndReservesElementName(unsigned varIndex,
                                                 const std::string& elementType,
                                                 const std::string& clusterName,
                                                 const std::string& reserveName1,
                                                 const std::string& reserveName2);
    void SetSTStorageClusterElementName(unsigned varIndex,
                                        const std::string& variableType,
                                        const std::string& clusterName);
    void SetSTStorageClusterAndReserveElementName(unsigned varIndex,
                                                  const std::string& elementType,
                                                  const std::string& clusterName,
                                                  const std::string& reserveName);
    void SetHydroElementName(unsigned varIndex,
                             const std::string& variableType,
                             const std::string& clusterName);
    void SetHydroAndReserveElementName(unsigned varIndex,
                                       const std::string& elementType,
                                       const std::string& clusterName,
                                       const std::string& reserveName);
    void SetThermalClusterReserveElementName(unsigned varIndex,
                                             const std::string& elementType,
                                             const std::string& reserveName);
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

using namespace Antares::Data;

class VariableNamer: public Namer
{
public:
    using Namer::Namer;
    void DispatchableProduction(unsigned varIndex, const std::string& clusterName);
    void ThermalClusterReserveParticipation(unsigned varIndex,
                                            const std::string& clusterName,
                                            const std::string& reserveName);
    void ParticipationOfSTStorageReleaseToReserve(unsigned varIndex,
                                                  const std::string& clusterName,
                                                  const std::string& reserveName);
    void ParticipationOfSTStorageStoreToReserve(unsigned varIndex,
                                                const std::string& clusterName,
                                                const std::string& reserveName);
    void ParticipationOfSTStorageToReserve(ReserveType type,
                                           unsigned varIndex,
                                           const std::string& clusterName,
                                           const std::string& reserveName);
    void ParticipationOfHydroReleaseToReserve(unsigned varIndex,
                                              const std::string& clusterName,
                                              const std::string& reserveName);
    void ParticipationOfHydroStoreToReserve(unsigned varIndex,
                                            const std::string& clusterName,
                                            const std::string& reserveName);
    void ParticipationOfHydroToReserve(ReserveType type,
                                       unsigned varIndex,
                                       const std::string& clusterName,
                                       const std::string& reserveName);
    void ParticipationOfRunningUnitsToReserve(unsigned varIndex,
                                              const std::string& clusterName,
                                              const std::string& reserveName);
    void ParticipationOfOffUnitsToReserve(unsigned varIndex,
                                          const std::string& clusterName,
                                          const std::string& reserveName);
    void InternalUnsatisfiedReserve(unsigned varIndex, const std::string& reserveName);
    void InternalExcessReserve(unsigned varIndex, const std::string& reserveName);
    void NODU(unsigned varIndex, const std::string& clusterName);
    void NumberStoppingDispatchableUnits(unsigned varIndex, const std::string& clusterName);
    void NumberStartingDispatchableUnits(unsigned varIndex, const std::string& clusterName);
    void NumberBreakingDownDispatchableUnits(unsigned varIndex, const std::string& clusterName);
    void DirectFlow(unsigned varIndex);
    void PositiveDirectFlow(unsigned varIndex);
    void PositiveIndirectFlow(unsigned varIndex);
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
    void UnsuppliedEnergy(unsigned varIndex);
    void Spillage(unsigned varIndex);
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
    void MaxUnsuppliedEnergy(unsigned constrIndex);
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
    void PMaxReserve(unsigned constrIndex,
                     const std::string& clusterName,
                     const std::string& reserveName);

    void ParticipationOfOffUnitsToReserve(unsigned constrIndex,
                                          const std::string& clusterName,
                                          const std::string& reserveName);
    void ParticipationOfUnitsToReserve(unsigned constrIndex,
                                       const std::string& clusterName,
                                       const std::string& reserveName);
    void SymmetryReserveParticipation(unsigned constrIndex,
                                      const std::string& clusterName,
                                      const std::string& reserveName1,
                                      const std::string& reserveName2);
    void POffUnitsUpperBound(unsigned constrIndex, const std::string& clusterName);
    void POutCapacityThresholdInf(unsigned constrIndex, const std::string& clusterName);
    void POutCapacityThresholdSup(unsigned constrIndex, const std::string& clusterName);
    void POutBoundMin(unsigned constrIndex, const std::string& clusterName);
    void POutBoundMax(unsigned constrIndex, const std::string& clusterName);
    void STReserveParticipation(unsigned constrIndex,
                                const std::string& clusterName,
                                const std::string& reserveName,
                                ReserveType type);
    void STReleaseMaxReserve(unsigned constrIndex,
                             const std::string& clusterName,
                             const std::string& reserveName);
    void STStoreMaxReserve(unsigned constrIndex,
                           const std::string& clusterName,
                           const std::string& reserveName);
    void STReleaseCapacityThresholdsUp(unsigned constrIndex, const std::string& clusterName);
    void STReleaseCapacityThresholdsDown(unsigned constrIndex, const std::string& clusterName);
    void STStoreCapacityThresholdsUp(unsigned constrIndex, const std::string& clusterName);
    void STStoreCapacityThresholdsDown(unsigned constrIndex, const std::string& clusterName);
    void STStorageLevelReserveParticipation(unsigned constrIndex,
                                            const std::string& clusterName,
                                            ReserveType type);
    void STEnergyStockLevelReserveParticipation(unsigned constrIndex,
                                                const std::string& clusterName,
                                                const std::string& reserveName);
    void STGlobalEnergyStockLevelReserveParticipation(unsigned constrIndex,
                                                      const std::string& clusterName,
                                                      ReserveType type);

    void HydroReserveParticipation(ReserveType type,
                                   unsigned constrIndex,
                                   const std::string& clusterName,
                                   const std::string& reserveName);
    void HydroReleaseMaxReserve(unsigned constrIndex,
                                const std::string& clusterName,
                                const std::string& reserveName);
    void HydroStoreMaxReserve(unsigned constrIndex,
                              const std::string& clusterName,
                              const std::string& reserveName);
    void HydroReleaseCapacityThresholdsUp(unsigned constrIndex, const std::string& clusterName);
    void HydroReleaseCapacityThresholdsDown(unsigned constrIndex, const std::string& clusterName);
    void HydroStoreCapacityThresholdsUp(unsigned constrIndex, const std::string& clusterName);
    void HydroStoreCapacityThresholdsDown(unsigned constrIndex, const std::string& clusterName);
    void HydroLevelReserveParticipation(ReserveType type,
                                        unsigned constrIndex,
                                        const std::string& clusterName);
    void HydroEnergyLevelReserveParticipation(unsigned constrIndex,
                                              const std::string& clusterName,
                                              const std::string& reserveName);
    void HydroGlobalEnergyLevelReserveParticipationDown(unsigned constrIndex,
                                                        const std::string& clusterName);
    void HydroGlobalEnergyLevelReserveParticipationUp(unsigned constrIndex,
                                                      const std::string& clusterName);

    void ReserveSatisfaction(unsigned constrIndex, const std::string& reserveName);
    void PMaxDispatchableGeneration(unsigned constrIndex, const std::string& clusterName);
    void PMinDispatchableGeneration(unsigned constrIndex, const std::string& clusterName);
    void ConsistenceNODU(unsigned constrIndex, const std::string& clusterName);
    void ShortTermStorageLevel(unsigned constrIndex, const std::string& name);
    void BindingConstraintHour(unsigned constrIndex, const std::string& name);
    void BindingConstraintDay(unsigned constrIndex, const std::string& name);
    void BindingConstraintWeek(unsigned constrIndex, const std::string& name);
    void CsrFlowDissociation(unsigned constrIndex);
    void CsrFictitiousLoad(unsigned constrIndex);
    void CsrMaxEnsLoad(unsigned constrIndex);
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
