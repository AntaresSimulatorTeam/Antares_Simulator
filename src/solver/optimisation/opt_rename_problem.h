#pragma once
#include "opt_structure_probleme_a_resoudre.h"
#include "opt_export_structure.h"

const std::string SEPARATOR = "::";
const std::string AREA_SEP = "$$";

struct CurrentAssetsStorage
{
    PROBLEME_ANTARES_A_RESOUDRE* problem_;
    int timeStep_ = 0;
    std::string origin_;
    std::string destination_;
    std::string area_;
    CurrentAssetsStorage(PROBLEME_ANTARES_A_RESOUDRE* problem) : problem_(problem)
    {
    }
    void UpdateTimeStep(int timeStep)
    {
        timeStep_ = timeStep;
    }

    void UpdateArea(const std::string& area)
    {
        area_ = area;
    }
};

class IVariableNamer : public CurrentAssetsStorage
{
protected:
    void SetThermalClusterVariableName(int var,
                                       Antares::Data::Enum::ExportStructDict structDict,
                                       const std::string& clusterName);
    void SetLinkVariableName(int var, Antares::Data::Enum::ExportStructDict structDict);
    void SetShortTermStorageVariableName(int var,
                                         Antares::Data::Enum::ExportStructDict structDict,
                                         const std::string& shortTermStorageName);
    void SetAreaVariableName(int var, Antares::Data::Enum::ExportStructDict structDict);
    void SetAreaVariableName(int var,
                             Antares::Data::Enum::ExportStructDict structDict,
                             int layerIndex);

public:
    IVariableNamer(PROBLEME_ANTARES_A_RESOUDRE* problem) : CurrentAssetsStorage(problem)
    {
    }

    virtual void DispatchableProduction(int var, const std::string& clusterName) = 0;
    virtual void NODU(int var, const std::string& clusterName) = 0;
    virtual void NumberStoppingDispatchableUnits(int var, const std::string& clusterName) = 0;
    virtual void NumberStartingDispatchableUnits(int var, const std::string& clusterName) = 0;
    virtual void NumberBreakingDownDispatchableUnits(int var, const std::string& clusterName) = 0;
    virtual void NTCDirect(int var, const std::string& origin, const std::string& destination) = 0;
    virtual void IntercoDirectCost(int var,
                                   const std::string& origin,
                                   const std::string& destination)
      = 0;
    virtual void IntercoIndirectCost(int var,
                                     const std::string& origin,
                                     const std::string& destination)
      = 0;
    virtual void ShortTermStorageInjection(int var, const std::string& shortTermStorageName) = 0;
    virtual void ShortTermStorageWithdrawal(int var, const std::string& shortTermStorageName) = 0;
    virtual void ShortTermStorageLevel(int var, const std::string& shortTermStorageName) = 0;
    virtual void HydProd(int var) = 0;
    virtual void HydProdDown(int var) = 0;
    virtual void HydProdUp(int var) = 0;
    virtual void Pumping(int var) = 0;
    virtual void HydroLevel(int var) = 0;
    virtual void Overflow(int var) = 0;
    virtual void FinalStorage(int var) = 0;
    virtual void LayerStorage(int var, int layerIndex) = 0;
    virtual void PositiveUnsuppliedEnergy(int var) = 0;
    virtual void NegativeUnsuppliedEnergy(int var) = 0;
    virtual void AreaBalance(int var) = 0;
};
class VariableNamer : public IVariableNamer
{
public:
    using IVariableNamer::IVariableNamer;
    void DispatchableProduction(int var, const std::string& clusterName) override;
    void NODU(int var, const std::string& clusterName) override;
    void NumberStoppingDispatchableUnits(int var, const std::string& clusterName) override;
    void NumberStartingDispatchableUnits(int var, const std::string& clusterName) override;
    void NumberBreakingDownDispatchableUnits(int var, const std::string& clusterName) override;
    void NTCDirect(int var, const std::string& origin, const std::string& destination) override;
    void IntercoDirectCost(int var,
                           const std::string& origin,
                           const std::string& destination) override;
    void IntercoIndirectCost(int var,
                             const std::string& origin,
                             const std::string& destination) override;
    void ShortTermStorageInjection(int var, const std::string& shortTermStorageName) override;
    void ShortTermStorageWithdrawal(int var, const std::string& shortTermStorageName) override;
    void ShortTermStorageLevel(int var, const std::string& shortTermStorageName) override;
    void HydProd(int var) override;
    void HydProdDown(int var) override;
    void HydProdUp(int var) override;
    void Pumping(int var) override;
    void HydroLevel(int var) override;
    void Overflow(int var) override;
    void FinalStorage(int var) override;
    void LayerStorage(int var, int layerIndex) override;
    void PositiveUnsuppliedEnergy(int var) override;
    void NegativeUnsuppliedEnergy(int var) override;
    void AreaBalance(int var) override;
};
class EmptyVariableNamer : public IVariableNamer
{
public:
    using IVariableNamer::IVariableNamer;
    void DispatchableProduction(int var, const std::string& clusterName) override
    {
        // keep empty
    }
    void NODU(int var, const std::string& clusterName) override
    {
        // keep empty
    }
    void NumberStoppingDispatchableUnits(int var, const std::string& clusterName) override
    {
        // keep empty
    }
    void NumberStartingDispatchableUnits(int var, const std::string& clusterName) override
    {
        // keep empty
    }
    void NumberBreakingDownDispatchableUnits(int var, const std::string& clusterName) override
    {
        // keep empty
    }
    void NTCDirect(int var, const std::string& origin, const std::string& destination) override
    {
        // keep empty
    }
    void IntercoDirectCost(int var,
                           const std::string& origin,
                           const std::string& destination) override
    {
        // keep empty
    }
    void IntercoIndirectCost(int var,
                             const std::string& origin,
                             const std::string& destination) override
    {
        // keep empty
    }
    void ShortTermStorageInjection(int var, const std::string& shortTermStorageName) override
    {
        // keep empty
    }
    void ShortTermStorageWithdrawal(int var, const std::string& shortTermStorageName) override
    {
        // keep empty
    }
    void ShortTermStorageLevel(int var, const std::string& shortTermStorageName) override
    {
        // keep empty
    }
    void HydProd(int var) override
    {
        // keep empty
    }
    void HydProdDown(int var) override
    {
        // keep empty
    }
    void HydProdUp(int var) override
    {
        // keep empty
    }
    void Pumping(int var) override
    {
        // keep empty
    }
    void HydroLevel(int var) override
    {
        // keep empty
    }
    void Overflow(int var) override
    {
        // keep empty
    }
    void FinalStorage(int var) override
    {
        // keep empty
    }
    void LayerStorage(int var, int layerIndex) override
    {
        // keep empty
    }
    void PositiveUnsuppliedEnergy(int var) override
    {
        // keep empty
    }
    void NegativeUnsuppliedEnergy(int var) override
    {
        // keep empty
    }
    void AreaBalance(int var) override
    {
        // keep empty
    }
};
using SPVariableNamer = std::shared_ptr<IVariableNamer>;
SPVariableNamer VariablesNamerFactory(PROBLEME_ANTARES_A_RESOUDRE* problem, bool namedProblem);

class IConstraintNamer : public CurrentAssetsStorage
{
protected:
    void nameWithTimeGranularity(const std::string& name,
                                 Antares::Data::Enum::ExportStructTimeStepDict type);

public:
    IConstraintNamer(PROBLEME_ANTARES_A_RESOUDRE* problem) : CurrentAssetsStorage(problem)
    {
    }
    virtual void FlowDissociation(const std::string& origin, const std::string& destination) = 0;
    virtual void AreaBalance() = 0;
    virtual void FictiveLoads() = 0;
    virtual void HydroPower() = 0;
    virtual void HydroPowerSmoothingUsingVariationSum() = 0;
    virtual void HydroPowerSmoothingUsingVariationMaxDown() = 0;
    virtual void HydroPowerSmoothingUsingVariationMaxUp() = 0;
    virtual void MinHydroPower() = 0;
    virtual void MaxHydroPower() = 0;
    virtual void MaxPumping() = 0;
    virtual void AreaHydroLevel() = 0;
    virtual void FinalStockEquivalent() = 0;
    virtual void FinalStockExpression() = 0;
    virtual void MinUpTime() = 0;
    virtual void MinDownTime() = 0;
    virtual void PMaxDispatchableGeneration() = 0;
    virtual void PMinDispatchableGeneration() = 0;
    virtual void ConsistenceNODU() = 0;
    virtual void ShortTermStorageLevel() = 0;
    virtual void BindingConstraintHour(const std::string& name) = 0;
    virtual void BindingConstraintDay(const std::string& name) = 0;
    virtual void BindingConstraintWeek(const std::string& name) = 0;
};
class ConstraintNamer : public IConstraintNamer
{
public:
    using IConstraintNamer::IConstraintNamer;
    void FlowDissociation(const std::string& origin, const std::string& destination) override;
    void AreaBalance() override;
    void FictiveLoads() override;
    void HydroPower() override;
    void HydroPowerSmoothingUsingVariationSum() override;
    void HydroPowerSmoothingUsingVariationMaxDown() override;
    void HydroPowerSmoothingUsingVariationMaxUp() override;
    void MinHydroPower() override;
    void MaxHydroPower() override;
    void MaxPumping() override;
    void AreaHydroLevel() override;
    void FinalStockEquivalent() override;
    void FinalStockExpression() override;
    void MinUpTime() override;
    void MinDownTime() override;
    void PMaxDispatchableGeneration() override;
    void PMinDispatchableGeneration() override;
    void ConsistenceNODU() override;
    void ShortTermStorageLevel() override;
    void BindingConstraintHour(const std::string& name) override
    {
        nameWithTimeGranularity(name, Antares::Data::Enum::ExportStructTimeStepDict::hour);
    }
    void BindingConstraintDay(const std::string& name) override
    {
        nameWithTimeGranularity(name, Antares::Data::Enum::ExportStructTimeStepDict::day);
    }
    void BindingConstraintWeek(const std::string& name) override
    {
        nameWithTimeGranularity(name, Antares::Data::Enum::ExportStructTimeStepDict::week);
    }
};
class EmptyConstraintNamer : public IConstraintNamer
{
public:
    using IConstraintNamer::IConstraintNamer;
    void FlowDissociation(const std::string& origin, const std::string& destination) override
    {
        // keep empty
    }
    void AreaBalance() override
    {
        // keep empty
    }
    void FictiveLoads() override
    {
        // keep empty
    }
    void HydroPower() override
    {
        // keep empty
    }
    void HydroPowerSmoothingUsingVariationSum() override
    {
        // keep empty
    }
    void HydroPowerSmoothingUsingVariationMaxDown() override
    {
        // keep empty
    }
    void HydroPowerSmoothingUsingVariationMaxUp() override
    {
        // keep empty
    }
    void MinHydroPower() override
    {
        // keep empty
    }
    void MaxHydroPower() override
    {
        // keep empty
    }
    void MaxPumping() override
    {
        // keep empty
    }
    void AreaHydroLevel() override
    {
        // keep empty
    }
    void FinalStockEquivalent() override
    {
        // keep empty
    }
    void FinalStockExpression() override
    {
        // keep empty
    }
    void MinUpTime() override
    {
        // keep empty
    }
    void MinDownTime() override
    {
        // keep empty
    }
    void PMaxDispatchableGeneration() override
    {
        // keep empty
    }
    void PMinDispatchableGeneration() override
    {
        // keep empty
    }
    void ConsistenceNODU() override
    {
        // keep empty
    }
    void ShortTermStorageLevel() override
    {
        // keep empty
    }
    void BindingConstraintHour(const std::string& name) override
    {
        // keep empty
    }
    void BindingConstraintDay(const std::string& name) override
    {
        // keep empty
    }
    void BindingConstraintWeek(const std::string& name) override
    {
        // keep empty
    }
};
using SPConstraintsNamer = std::shared_ptr<IConstraintNamer>;
SPConstraintsNamer ConstraintsNamerFactory(PROBLEME_ANTARES_A_RESOUDRE* problem, bool namedProblem);

std::string BuildName(const std::string& name,
                      const std::string& location,
                      const std::string& timeIdentifier);
inline std::string TimeIdentifier(int timeStep,
                                  Antares::Data::Enum::ExportStructTimeStepDict timeStepType)
{
    return Antares::Data::Enum::toString(timeStepType) + "<" + std::to_string(timeStep) + ">";
}

inline std::string LocationIdentifier(const std::string& location,
                                      Antares::Data::Enum::ExportStructLocationDict locationType)
{
    return Antares::Data::Enum::toString(locationType) + "<" + location + ">";
}
