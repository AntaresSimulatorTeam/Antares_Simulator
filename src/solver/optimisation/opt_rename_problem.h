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

class VariableNamer : public CurrentAssetsStorage
{
private:
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
    VariableNamer(PROBLEME_ANTARES_A_RESOUDRE* problem) : CurrentAssetsStorage(problem)
    {
    }

    void DispatchableProduction(int var, const std::string& clusterName);
    void NODU(int var, const std::string& clusterName);
    void NumberStoppingDispatchableUnits(int var, const std::string& clusterName);
    void NumberStartingDispatchableUnits(int var, const std::string& clusterName);
    void NumberBreakingDownDispatchableUnits(int var, const std::string& clusterName);
    void NTCDirect(int var, const std::string& origin, const std::string& destination);
    void IntercoDirectCost(int var, const std::string& origin, const std::string& destination);
    void IntercoIndirectCost(int var, const std::string& origin, const std::string& destination);
    void ShortTermStorageInjection(int var, const std::string& shortTermStorageName);
    void ShortTermStorageWithdrawal(int var, const std::string& shortTermStorageName);
    void ShortTermStorageLevel(int var, const std::string& shortTermStorageName);
    void HydProd(int var);
    void HydProdDown(int var);
    void HydProdUp(int var);
    void Pumping(int var);
    void HydroLevel(int var);
    void Overflow(int var);
    void FinalStorage(int var);
    void LayerStorage(int var, int layerIndex);
    void PositiveUnsuppliedEnergy(int var);
    void NegativeUnsuppliedEnergy(int var);
    void AreaBalance(int var);
};

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
