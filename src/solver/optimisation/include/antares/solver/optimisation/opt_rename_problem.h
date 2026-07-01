// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <optional>
#include <string>
#include <vector>

#include <antares/study/fwd.h>
#include "antares/solver/optimisation/LegacyVariableInfo.h"

// Structured legacy descriptions, parallel to the names vector.
using LegacyInfoVec = std::vector<std::optional<Antares::Optimization::LegacyVariableInfo>>;

class Namer
{
public:
    explicit Namer(std::vector<std::string>& target_names, LegacyInfoVec& legacyInfo);
    explicit Namer(std::vector<std::string>& target_names);
    virtual ~Namer() = default;
    void UpdateTimeStep(unsigned timeStep);
    void UpdateArea(const std::string& area);
    void updateExtremities(const std::string& origin, const std::string& destination);

protected:
    // Records the structured legacy description of a variable, parallel to the name.
    // No-op when this namer was built without a recording target.
    void RecordLegacyVariableInfo(unsigned index,
                                  const std::string& output,
                                  const std::string& component) const;

    void RecordNothing(unsigned index, const std::string&, const std::string&) const
    {
    }

    unsigned timeStep() const
    {
        return timeStep_;
    }

    const std::string& area() const
    {
        return area_;
    }

    void SetLinkElementName(unsigned varIndex, const std::string& variableType) const;
    void SetAreaElementNameHour(unsigned varIndex, const std::string& variableType) const;
    void SetAreaElementNameWeek(unsigned varIndex, const std::string& variableType) const;
    void SetAreaElementName(unsigned varIndex,
                            const std::string& variableType,
                            const std::string& timeGranularity) const;
    void SetThermalClusterElementName(unsigned varIndex,
                                      const std::string& variableType,
                                      const std::string& clusterName) const;
    void SetThermalClusterAndReserveElementName(unsigned varIndex,
                                                const std::string& elementType,
                                                const std::string& clusterName,
                                                const std::string& reserveName) const;
    void SetThermalClusterAndReservesElementName(unsigned varIndex,
                                                 const std::string& elementType,
                                                 const std::string& clusterName,
                                                 const std::string& reserveName1,
                                                 const std::string& reserveName2) const;
    void SetSTStorageClusterElementName(unsigned varIndex,
                                        const std::string& variableType,
                                        const std::string& clusterName) const;
    void SetSTStorageClusterAndReserveElementName(unsigned varIndex,
                                                  const std::string& elementType,
                                                  const std::string& clusterName,
                                                  const std::string& reserveName) const;
    void SetHydroElementName(unsigned varIndex,
                             const std::string& variableType,
                             const std::string& clusterName) const;
    void SetHydroAndReserveElementName(unsigned varIndex,
                                       const std::string& elementType,
                                       const std::string& clusterName,
                                       const std::string& reserveName) const;
    void SetThermalClusterReserveElementName(unsigned varIndex,
                                             const std::string& elementType,
                                             const std::string& reserveName) const;
    std::string TimeIdentifier(const std::string& timeGranularity) const;
    std::string linkLocation() const;
    std::string areaLocation() const;
    std::vector<std::string>& names() const;
    std::function<void(const Namer&, unsigned, const std::string&, const std::string&)> record_;

private:
    std::string origin_;
    std::string destination_;
    std::string area_;
    unsigned timeStep_ = 0;
    std::vector<std::string>& names_;
    LegacyInfoVec* legacyInfo_ = nullptr;
};

using namespace Antares::Data;

class VariableNamer: public Namer
{
public:
    using Namer::Namer;

    void DispatchableProduction(unsigned varIndex, const std::string& clusterName) const;
    void ThermalClusterReserveParticipation(unsigned varIndex,
                                            const std::string& clusterName,
                                            const std::string& reserveName) const;
    void ParticipationOfSTStorageReleaseToReserve(unsigned varIndex,
                                                  const std::string& clusterName,
                                                  const std::string& reserveName) const;
    void ParticipationOfSTStorageStoreToReserve(unsigned varIndex,
                                                const std::string& clusterName,
                                                const std::string& reserveName) const;
    void ParticipationOfSTStorageToReserve(ReserveType type,
                                           unsigned varIndex,
                                           const std::string& clusterName,
                                           const std::string& reserveName) const;
    void ParticipationOfHydroReleaseToReserve(unsigned varIndex,
                                              const std::string& clusterName,
                                              const std::string& reserveName) const;
    void ParticipationOfHydroStoreToReserve(unsigned varIndex,
                                            const std::string& clusterName,
                                            const std::string& reserveName) const;
    void ParticipationOfHydroToReserve(ReserveType type,
                                       unsigned varIndex,
                                       const std::string& clusterName,
                                       const std::string& reserveName) const;
    void ParticipationOfRunningUnitsToReserve(unsigned varIndex,
                                              const std::string& clusterName,
                                              const std::string& reserveName) const;
    void ParticipationOfOffUnitsToReserve(unsigned varIndex,
                                          const std::string& clusterName,
                                          const std::string& reserveName) const;
    void InternalUnsatisfiedReserve(unsigned varIndex, const std::string& reserveName) const;
    void InternalExcessReserve(unsigned varIndex, const std::string& reserveName) const;
    void NODU(unsigned varIndex, const std::string& clusterName) const;
    void NumberStoppingDispatchableUnits(unsigned varIndex, const std::string& clusterName) const;
    void NumberStartingDispatchableUnits(unsigned varIndex, const std::string& clusterName) const;
    void NumberBreakingDownDispatchableUnits(unsigned varIndex,
                                             const std::string& clusterName) const;
    void DirectFlow(unsigned varIndex) const;
    void PositiveDirectFlow(unsigned varIndex) const;
    void PositiveIndirectFlow(unsigned varIndex) const;
    void ShortTermStorageInjection(unsigned varIndex, const std::string& sts_name) const;
    void ShortTermStorageWithdrawal(unsigned varIndex, const std::string& sts_name) const;
    void ShortTermStorageLevel(unsigned varIndex, const std::string& sts_name) const;
    void ShortTermStorageOverflow(unsigned varIndex, const std::string& sts_name) const;
    void ShortTermStorageCostVariationInjection(unsigned varIndex,
                                                const std::string& sts_name) const;
    void ShortTermStorageCostVariationWithdrawal(unsigned varIndex,
                                                 const std::string& sts_name) const;
    void HydProd(unsigned varIndex) const;
    void HydProdDown(unsigned varIndex) const;
    void HydProdUp(unsigned varIndex) const;
    void Pumping(unsigned varIndex) const;
    void HydroLevel(unsigned varIndex) const;
    void Overflow(unsigned varIndex) const;
    void FinalStorage(unsigned varIndex) const;
    void LayerStorage(unsigned varIndex, int layerIndex) const;
    void UnsuppliedEnergy(unsigned varIndex) const;
    void Spillage(unsigned varIndex) const;
    void AreaBalance(unsigned varIndex) const;

private:
    void SetAreaVariableName(unsigned varIndex,
                             const std::string& variableType,
                             int layerIndex) const;
    void SetShortTermStorageVariableName(unsigned varIndex,
                                         const std::string& variableType,
                                         const std::string& sts_name) const;
};

class ConstraintNamer: public Namer
{
public:
    using Namer::Namer;

    void FlowDissociation(unsigned constrIndex) const;
    void AreaBalance(unsigned constrIndex) const;
    void FictiveLoads(unsigned constrIndex) const;
    void MaxUnsuppliedEnergy(unsigned constrIndex) const;
    void HydroPower(unsigned constrIndex) const;
    void HydroPowerSmoothingUsingVariationSum(unsigned constrIndex) const;
    void HydroPowerSmoothingUsingVariationMaxDown(unsigned constrIndex) const;
    void HydroPowerSmoothingUsingVariationMaxUp(unsigned constrIndex) const;
    void MinHydroPower(unsigned constrIndex) const;
    void MaxHydroPower(unsigned constrIndex) const;
    void MaxPumping(unsigned constrIndex) const;
    void AreaHydroLevel(unsigned constrIndex) const;
    void FinalStockEquivalent(unsigned constrIndex) const;
    void FinalStockExpression(unsigned constrIndex) const;
    void NbUnitsOutageLessThanNbUnitsStop(unsigned constrIndex,
                                          const std::string& clusterName) const;
    void NbDispUnitsMinBoundSinceMinUpTime(unsigned constrIndex,
                                           const std::string& clusterName) const;
    void MinDownTime(unsigned constrIndex, const std::string& clusterName) const;
    void PMaxReserve(unsigned constrIndex,
                     const std::string& clusterName,
                     const std::string& reserveName) const;

    void ParticipationOfOffUnitsToReserve(unsigned constrIndex,
                                          const std::string& clusterName,
                                          const std::string& reserveName) const;
    void ParticipationOfUnitsToReserve(unsigned constrIndex,
                                       const std::string& clusterName,
                                       const std::string& reserveName) const;
    void SymmetryReserveParticipation(unsigned constrIndex,
                                      const std::string& clusterName,
                                      const std::string& reserveName1,
                                      const std::string& reserveName2) const;
    void POffUnitsUpperBound(unsigned constrIndex, const std::string& clusterName) const;
    void POutCapacityThresholdInf(unsigned constrIndex, const std::string& clusterName) const;
    void POutCapacityThresholdSup(unsigned constrIndex, const std::string& clusterName) const;
    void POutBoundMin(unsigned constrIndex, const std::string& clusterName) const;
    void POutBoundMax(unsigned constrIndex, const std::string& clusterName) const;
    void STReserveParticipation(unsigned constrIndex,
                                const std::string& clusterName,
                                const std::string& reserveName,
                                ReserveType type) const;
    void STReleaseMaxReserve(unsigned constrIndex,
                             const std::string& clusterName,
                             const std::string& reserveName) const;
    void STStoreMaxReserve(unsigned constrIndex,
                           const std::string& clusterName,
                           const std::string& reserveName) const;
    void STReleaseCapacityThresholdsUp(unsigned constrIndex, const std::string& clusterName) const;
    void STReleaseCapacityThresholdsDown(unsigned constrIndex,
                                         const std::string& clusterName) const;
    void STStoreCapacityThresholdsUp(unsigned constrIndex, const std::string& clusterName) const;
    void STStoreCapacityThresholdsDown(unsigned constrIndex, const std::string& clusterName) const;
    void STStorageLevelReserveParticipation(unsigned constrIndex,
                                            const std::string& clusterName,
                                            ReserveType type) const;
    void STEnergyStockLevelReserveParticipation(unsigned constrIndex,
                                                const std::string& clusterName,
                                                const std::string& reserveName) const;
    void STGlobalEnergyStockLevelReserveParticipation(unsigned constrIndex,
                                                      const std::string& clusterName,
                                                      ReserveType type) const;

    void HydroReserveParticipation(ReserveType type,
                                   unsigned constrIndex,
                                   const std::string& clusterName,
                                   const std::string& reserveName) const;
    void HydroReleaseMaxReserve(unsigned constrIndex,
                                const std::string& clusterName,
                                const std::string& reserveName) const;
    void HydroStoreMaxReserve(unsigned constrIndex,
                              const std::string& clusterName,
                              const std::string& reserveName) const;
    void HydroReleaseCapacityThresholdsUp(unsigned constrIndex,
                                          const std::string& clusterName) const;
    void HydroReleaseCapacityThresholdsDown(unsigned constrIndex,
                                            const std::string& clusterName) const;
    void HydroStoreCapacityThresholdsUp(unsigned constrIndex, const std::string& clusterName) const;
    void HydroStoreCapacityThresholdsDown(unsigned constrIndex,
                                          const std::string& clusterName) const;
    void HydroLevelReserveParticipation(ReserveType type,
                                        unsigned constrIndex,
                                        const std::string& clusterName) const;
    void HydroEnergyLevelReserveParticipation(unsigned constrIndex,
                                              const std::string& clusterName,
                                              const std::string& reserveName) const;
    void HydroGlobalEnergyLevelReserveParticipationDown(unsigned constrIndex,
                                                        const std::string& clusterName) const;
    void HydroGlobalEnergyLevelReserveParticipationUp(unsigned constrIndex,
                                                      const std::string& clusterName) const;

    void ReserveSatisfaction(unsigned constrIndex, const std::string& reserveName) const;
    void PMaxDispatchableGeneration(unsigned constrIndex, const std::string& clusterName) const;
    void PMinDispatchableGeneration(unsigned constrIndex, const std::string& clusterName) const;
    void ConsistenceNODU(unsigned constrIndex, const std::string& clusterName) const;
    void ShortTermStorageLevel(unsigned constrIndex, const std::string& name) const;
    void BindingConstraintHour(unsigned constrIndex, const std::string& name) const;
    void BindingConstraintDay(unsigned constrIndex, const std::string& name) const;
    void BindingConstraintWeek(unsigned constrIndex, const std::string& name) const;
    void CsrFlowDissociation(unsigned constrIndex) const;
    void CsrFictitiousLoad(unsigned constrIndex) const;
    void CsrMaxEnsLoad(unsigned constrIndex) const;
    void CsrAreaBalance(unsigned constrIndex) const;
    void ShortTermStorageCostVariation(const std::string& constrIndex_name,
                                       unsigned constrIndex,
                                       const std::string& sts_name) const;
    void ShortTermStorageCumulation(const std::string& constraint_type,
                                    unsigned constrIndex,
                                    const std::string& sts_name,
                                    const std::string& constrIndex_name) const;

private:
    void BindingConstraint(unsigned constrIndex,
                           const std::string& name,
                           const std::pair<std::string, std::string>& timeGranularity) const;
};
