#pragma once
#include "opt_structure_probleme_a_resoudre.h"
#include "opt_export_structure.h"

const std::string SEPARATOR = "::";
const std::string AREA_SEP = "$$";
struct BuildNamer
{
    BuildNamer(bool name)
    {
        if (name)
        {
            Run = BuildName;
        }
        else
        {
            Run = ReturnEmptyString;
        }
    }
    std::function<std::string(const std::string&, const std::string&, const std::string&)> Run;

private:
    static std::string BuildName(const std::string& name,
                                 const std::string& location,
                                 const std::string& timeIdentifier);

    static std::string ReturnEmptyString(const std::string& name,
                                         const std::string& location,
                                         const std::string& timeIdentifier)
    {
        return "";
    }
};
struct CurrentAssetsStorage
{
    PROBLEME_ANTARES_A_RESOUDRE* problem_;
    int timeStep_ = 0;
    std::string origin_;
    std::string destination_;
    std::string area_;
    BuildNamer build_namer_;
    CurrentAssetsStorage(PROBLEME_ANTARES_A_RESOUDRE* problem, bool namedProblems) :
     problem_(problem), build_namer_(namedProblems)
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
public:
    using CurrentAssetsStorage::CurrentAssetsStorage;
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

private:
    void SetThermalClusterVariableName(int var,
                                       const std::string& variableType,
                                       const std::string& clusterName);
    void SetAreaVariableName(int var, const std::string& variableType);

    void SetAreaVariableName(int var, const std::string& variableType, int layerIndex);
    void SetLinkVariableName(int var, const std::string& variableType);
    void SetShortTermStorageVariableName(int var,
                                         const std::string& variableType,
                                         const std::string& shortTermStorageName);
};

class ConstraintNamer : public CurrentAssetsStorage
{
public:
    using CurrentAssetsStorage::CurrentAssetsStorage;
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
    void ShortTermStorageLevel(const std::string& name);
    void BindingConstraintHour(const std::string& name);
    void BindingConstraintDay(const std::string& name);
    void BindingConstraintWeek(const std::string& name);

private:
    void nameWithTimeGranularity(const std::string& name, const std::string& type);
};

inline std::string TimeIdentifier(int timeStep, const std::string& timeStepType)
{
    return timeStepType + "<" + std::to_string(timeStep) + ">";
}

inline std::string LocationIdentifier(const std::string& location, const std::string& locationType)
{
    return locationType + "<" + location + ">";
}
