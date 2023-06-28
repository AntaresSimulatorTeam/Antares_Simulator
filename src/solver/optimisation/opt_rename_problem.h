#pragma once
#include "opt_structure_probleme_a_resoudre.h"
#include "opt_export_structure.h"

const std::string SEPARATOR = "::";
const std::string AREA_SEP = "$$";
struct NameUpdater
{
    std::vector<std::string>& target_;
    NameUpdater(bool name, std::vector<std::string>& target) : target_(target)
    {
        if (name)
        {
            Run = std::bind(&NameUpdater::BuildName,
                            this,
                            std::placeholders::_1,
                            std::placeholders::_2,
                            std::placeholders::_3,
                            std::placeholders::_4);
        }
    }
    std::function<void(const std::string&, const std::string&, const std::string&, unsigned index)>
      Run = [](const std::string&, const std::string&, const std::string&, unsigned index) {};

private:
    void BuildName(const std::string& name,
                   const std::string& location,
                   const std::string& timeIdentifier,
                   unsigned index);
};

struct CurrentAssetsStorage
{
    PROBLEME_ANTARES_A_RESOUDRE* problem_;
    int timeStep_ = 0;
    std::string origin_;
    std::string destination_;
    std::string area_;
    NameUpdater build_namer_;
    CurrentAssetsStorage(PROBLEME_ANTARES_A_RESOUDRE* problem,
                         bool namedProblems,
                         std::vector<std::string>& target) :
     problem_(problem), build_namer_(namedProblems, target)
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
    VariableNamer(PROBLEME_ANTARES_A_RESOUDRE* problem, bool namedProblems) :
     CurrentAssetsStorage(problem, namedProblems, problem->NomDesVariables)
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
    ConstraintNamer(PROBLEME_ANTARES_A_RESOUDRE* problem, bool namedProblems) :
     CurrentAssetsStorage(problem, namedProblems, problem->NomDesContraintes)
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
