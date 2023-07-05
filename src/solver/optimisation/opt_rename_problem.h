#pragma once
#include "opt_structure_probleme_a_resoudre.h"
#include "opt_export_structure.h"

const std::string SEPARATOR = "::";
const std::string AREA_SEP = "$$";

class TargetVectorUpdater
{
public:
    TargetVectorUpdater(bool isRenamingProcessed, std::vector<std::string>& target) :
     target_(target)
    {
        if (isRenamingProcessed)
        {
            UpdateTargetAtIndex = std::bind(
              &TargetVectorUpdater::UpdateTargetAtIndexImpl, this, std::placeholders::_1, std::placeholders::_2);
        }
    }

    std::function<void(const std::string&, unsigned int index)> UpdateTargetAtIndex
      = [](const std::string&, unsigned int index) {};

private:
    std::vector<std::string>& target_;
    void UpdateTargetAtIndexImpl(const std::string& full_name, unsigned int index)
    {
        target_[index] = full_name;
    }
};

class Namer
{
public:
    Namer(std::vector<std::string>& target, bool namedProblems) :
     targetUpdater_(namedProblems, target)
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

    int timeStep_ = 0;
    std::string origin_;
    std::string destination_;
    std::string area_;
    TargetVectorUpdater targetUpdater_;
};

class VariableNamer : public Namer
{
public:
    using Namer::Namer;
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

class ConstraintNamer : public Namer
{
public:
    using Namer::Namer;

    void FlowDissociation(int numConstraint,
                          const std::string& origin,
                          const std::string& destination);
    void AreaBalance(int numConstraint);
    void FictiveLoads(int numConstraint);
    void HydroPower(int numConstraint);
    void HydroPowerSmoothingUsingVariationSum(int numConstraint);
    void HydroPowerSmoothingUsingVariationMaxDown(int numConstraint);
    void HydroPowerSmoothingUsingVariationMaxUp(int numConstraint);
    void MinHydroPower(int numConstraint);
    void MaxHydroPower(int numConstraint);
    void MaxPumping(int numConstraint);
    void AreaHydroLevel(int numConstraint);
    void FinalStockEquivalent(int numConstraint);
    void FinalStockExpression(int numConstraint);
    void NbUnitsOutageLessThanNbUnitsStop(int numConstraint);
    void NbDispUnitsMinBoundSinceMinUpTime(int numConstraint);
    void MinDownTime(int numConstraint);
    void PMaxDispatchableGeneration(int numConstraint);
    void PMinDispatchableGeneration(int numConstraint);
    void ConsistenceNODU(int numConstraint);
    void ShortTermStorageLevel(int numConstraint, const std::string& name);
    void BindingConstraintHour(int numConstraint, const std::string& name);
    void BindingConstraintDay(int numConstraint, const std::string& name);
    void BindingConstraintWeek(int numConstraint, const std::string& name);

private:
    void nameWithTimeGranularity(int numConstraint,
                                 const std::string& name,
                                 const std::string& type);
};

inline std::string TimeIdentifier(int timeStep, const std::string& timeStepType)
{
    return timeStepType + "<" + std::to_string(timeStep) + ">";
}

inline std::string LocationIdentifier(const std::string& location, const std::string& locationType)
{
    return locationType + "<" + location + ">";
}
