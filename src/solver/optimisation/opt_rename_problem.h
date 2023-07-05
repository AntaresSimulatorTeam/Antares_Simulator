#pragma once
#include "opt_structure_probleme_a_resoudre.h"
#include "opt_export_structure.h"

const std::string SEPARATOR = "::";
const std::string AREA_SEP = "$$";

class TargetVectorUpdater
{
public:
    TargetVectorUpdater(bool isRenamingProcessed, std::vector<std::string>& target) :
    target_(target), isRenamingProcessed_(isRenamingProcessed)
    {
    }

    void UpdateTargetAtIndex(const std::string& full_name, unsigned int index)
    {
    if (isRenamingProcessed_)
    {
          target_[index] = full_name;
    }
    }

private:
    std::vector<std::string>& target_;
    bool isRenamingProcessed_;
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
    void DispatchableProduction(unsigned int var, const std::string& clusterName);
    void NODU(unsigned int var, const std::string& clusterName);
    void NumberStoppingDispatchableUnits(unsigned int var, const std::string& clusterName);
    void NumberStartingDispatchableUnits(unsigned int var, const std::string& clusterName);
    void NumberBreakingDownDispatchableUnits(unsigned int var, const std::string& clusterName);
    void NTCDirect(unsigned int var, const std::string& origin, const std::string& destination);
    void IntercoDirectCost(unsigned int var,
                           const std::string& origin,
                           const std::string& destination);
    void IntercoIndirectCost(unsigned int var,
                             const std::string& origin,
                             const std::string& destination);
    void ShortTermStorageInjection(unsigned int var, const std::string& shortTermStorageName);
    void ShortTermStorageWithdrawal(unsigned int var, const std::string& shortTermStorageName);
    void ShortTermStorageLevel(unsigned int var, const std::string& shortTermStorageName);
    void HydProd(unsigned int var);
    void HydProdDown(unsigned int var);
    void HydProdUp(unsigned int var);
    void Pumping(unsigned int var);
    void HydroLevel(unsigned int var);
    void Overflow(unsigned int var);
    void FinalStorage(unsigned int var);
    void LayerStorage(unsigned int var, int layerIndex);
    void PositiveUnsuppliedEnergy(unsigned int var);
    void NegativeUnsuppliedEnergy(unsigned int var);
    void AreaBalance(unsigned int var);

private:
    void SetThermalClusterVariableName(unsigned int var,
                                       const std::string& variableType,
                                       const std::string& clusterName);
    void SetAreaVariableName(unsigned int var, const std::string& variableType);

    void SetAreaVariableName(unsigned int var, const std::string& variableType, int layerIndex);
    void SetLinkVariableName(unsigned int var, const std::string& variableType);
    void SetShortTermStorageVariableName(unsigned int var,
                                         const std::string& variableType,
                                         const std::string& shortTermStorageName);
};

class ConstraintNamer : public Namer
{
public:
    using Namer::Namer;

    void FlowDissociation(unsigned int numConstraint,
                          const std::string& origin,
                          const std::string& destination);
    void AreaBalance(unsigned int numConstraint);
    void FictiveLoads(unsigned int numConstraint);
    void HydroPower(unsigned int numConstraint);
    void HydroPowerSmoothingUsingVariationSum(unsigned int numConstraint);
    void HydroPowerSmoothingUsingVariationMaxDown(unsigned int numConstraint);
    void HydroPowerSmoothingUsingVariationMaxUp(unsigned int numConstraint);
    void MinHydroPower(unsigned int numConstraint);
    void MaxHydroPower(unsigned int numConstraint);
    void MaxPumping(unsigned int numConstraint);
    void AreaHydroLevel(unsigned int numConstraint);
    void FinalStockEquivalent(unsigned int numConstraint);
    void FinalStockExpression(unsigned int numConstraint);
    void NbUnitsOutageLessThanNbUnitsStop(unsigned int numConstraint);
    void NbDispUnitsMinBoundSinceMinUpTime(unsigned int numConstraint);
    void MinDownTime(unsigned int numConstraint);
    void PMaxDispatchableGeneration(unsigned int numConstraint);
    void PMinDispatchableGeneration(unsigned int numConstraint);
    void ConsistenceNODU(unsigned int numConstraint);
    void ShortTermStorageLevel(unsigned int numConstraint, const std::string& name);
    void BindingConstraintHour(unsigned int numConstraint, const std::string& name);
    void BindingConstraintDay(unsigned int numConstraint, const std::string& name);
    void BindingConstraintWeek(unsigned int numConstraint, const std::string& name);

private:
    void nameWithTimeGranularity(unsigned int numConstraint,
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
