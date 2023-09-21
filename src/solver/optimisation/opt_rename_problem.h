#pragma once
#include "opt_structure_probleme_a_resoudre.h"
#include "opt_export_structure.h"

const std::string SEPARATOR = "::";
const std::string AREA_SEP = "$$";

class TargetVectorUpdater
{
public:
    TargetVectorUpdater(std::vector<std::string>& target) :
     target_(target)
    {
    }

    void UpdateTargetAtIndex(const std::string& full_name, unsigned int index)
    {
        target_[index] = full_name;
    }

private:
    std::vector<std::string>& target_;
};

class Namer
{
public:
    Namer(std::vector<std::string>& target) :
     targetUpdater_(target)
    {
#ifdef __linux__
        std::cout << "Namer::Namer :" << __PRETTY_FUNCTION__ << "\n";
#endif
    }

    void UpdateTimeStep(unsigned int timeStep)
    {
        timeStep_ = timeStep;
    }

    void UpdateArea(const std::string& area)
    {
        area_ = area;
    }
    void SetLinkElementName(unsigned int variable, const std::string& variableType);
    void SetAreaElementNameHour(unsigned int variable, const std::string& variableType);
    void SetAreaElementNameWeek(unsigned int variable, const std::string& variableType);
    void SetAreaElementName(unsigned int variable,
                            const std::string& variableType,
                            const std::string& timeStepType);
    void SetThermalClusterElementName(unsigned int variable,
                                      const std::string& variableType,
                                      const std::string& clusterName);

    unsigned int timeStep_ = 0;
    std::string origin_;
    std::string destination_;
    std::string area_;
    TargetVectorUpdater targetUpdater_;
};

class VariableNamer : public Namer
{
public:
    using Namer::Namer;
    void DispatchableProduction(unsigned int variable, const std::string& clusterName);
    void NODU(unsigned int variable, const std::string& clusterName);
    void NumberStoppingDispatchableUnits(unsigned int variable, const std::string& clusterName);
    void NumberStartingDispatchableUnits(unsigned int variable, const std::string& clusterName);
    void NumberBreakingDownDispatchableUnits(unsigned int variable, const std::string& clusterName);
    void NTCDirect(unsigned int variable,
                   const std::string& origin,
                   const std::string& destination);
    void IntercoDirectCost(unsigned int variable,
                           const std::string& origin,
                           const std::string& destination);
    void IntercoIndirectCost(unsigned int variable,
                             const std::string& origin,
                             const std::string& destination);
    void ShortTermStorageInjection(unsigned int variable, const std::string& shortTermStorageName);
    void ShortTermStorageWithdrawal(unsigned int variable, const std::string& shortTermStorageName);
    void ShortTermStorageLevel(unsigned int variable, const std::string& shortTermStorageName);
    void HydProd(unsigned int variable);
    void HydProdDown(unsigned int variable);
    void HydProdUp(unsigned int variable);
    void Pumping(unsigned int variable);
    void HydroLevel(unsigned int variable);
    void Overflow(unsigned int variable);
    void FinalStorage(unsigned int variable);
    void LayerStorage(unsigned int variable, int layerIndex);
    void PositiveUnsuppliedEnergy(unsigned int variable);
    void NegativeUnsuppliedEnergy(unsigned int variable);
    void AreaBalance(unsigned int variable);

private:
    void SetAreaVariableName(unsigned int variable,
                             const std::string& variableType,
                             int layerIndex);
    void SetShortTermStorageVariableName(unsigned int variable,
                                         const std::string& variableType,
                                         const std::string& shortTermStorageName);
};

class ConstraintNamer : public Namer
{
public:
    using Namer::Namer;

    void FlowDissociation(unsigned int constraint,
                          const std::string& origin,
                          const std::string& destination);
    void AreaBalance(unsigned int constraint);
    void FictiveLoads(unsigned int constraint);
    void HydroPower(unsigned int constraint);
    void HydroPowerSmoothingUsingVariationSum(unsigned int constraint);
    void HydroPowerSmoothingUsingVariationMaxDown(unsigned int constraint);
    void HydroPowerSmoothingUsingVariationMaxUp(unsigned int constraint);
    void MinHydroPower(unsigned int constraint);
    void MaxHydroPower(unsigned int constraint);
    void MaxPumping(unsigned int constraint);
    void AreaHydroLevel(unsigned int constraint);
    void FinalStockEquivalent(unsigned int constraint);
    void FinalStockExpression(unsigned int constraint);
    void NbUnitsOutageLessThanNbUnitsStop(unsigned int constraint, const std::string& clusterName);
    void NbDispUnitsMinBoundSinceMinUpTime(unsigned int constraint, const std::string& clusterName);
    void MinDownTime(unsigned int constraint, const std::string& clusterName);
    void PMaxDispatchableGeneration(unsigned int constraint, const std::string& clusterName);
    void PMinDispatchableGeneration(unsigned int constraint, const std::string& clusterName);
    void ConsistenceNODU(unsigned int constraint, const std::string& clusterName);
    void ShortTermStorageLevel(unsigned int constraint, const std::string& name);
    void BindingConstraintHour(unsigned int constraint, const std::string& name);
    void BindingConstraintDay(unsigned int constraint, const std::string& name);
    void BindingConstraintWeek(unsigned int constraint, const std::string& name);

private:
    void nameWithTimeGranularity(unsigned int constraint,
                                 const std::string& name,
                                 const std::string& type);
};

inline std::string TimeIdentifier(unsigned int timeStep, const std::string& timeStepType)
{
    return timeStepType + "<" + std::to_string(timeStep) + ">";
}

inline std::string LocationIdentifier(const std::string& location, const std::string& locationType)
{
    return locationType + "<" + location + ">";
}
