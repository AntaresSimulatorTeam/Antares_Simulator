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

class ConstraintNamer : public CurrentAssetsStorage
{
private:
    void nameWithTimeGranularity(const std::string& name,
                                 Antares::Data::Enum::ExportStructTimeStepDict type);

public:
    ConstraintNamer(PROBLEME_ANTARES_A_RESOUDRE* problem) : CurrentAssetsStorage(problem)
    {
    }
    void FlowDissociation(const std::string& origin, const std::string& destination);
    void AreaBalance();
    void FictiveLoads();
    void HydroPower();
    void HydroPowerSmoothingUsingVariationSum();
    void HydroPowerSmoothingUsingVariationMaxDown();
    void HydroPowerSmoothingUsingVariationMaxUp();
    void MinHydroPower();
    void MaxHydroPower();
    void MaxPumping();
    void AreaHydroLevel();
    void FinalStockEquivalent();
    void FinalStockExpression();
    void MinUpTime();
    void MinDownTime();
    void PMaxDispatchableGeneration();
    void PMinDispatchableGeneration();
    void ConsistenceNODU();
    void ShortTermStorageLevel();
    void BindingConstraintHour(const std::string& name)
    {
        nameWithTimeGranularity(name, Antares::Data::Enum::ExportStructTimeStepDict::hour);
    }
    void BindingConstraintDay(const std::string& name)
    {
        nameWithTimeGranularity(name, Antares::Data::Enum::ExportStructTimeStepDict::day);
    }
    void BindingConstraintWeek(const std::string& name)
    {
        nameWithTimeGranularity(name, Antares::Data::Enum::ExportStructTimeStepDict::week);
    }
};

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
