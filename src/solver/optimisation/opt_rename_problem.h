#pragma once
#include "opt_structure_probleme_a_resoudre.h"
#include "opt_export_structure.h"

const std::string SEPARATOR = "::";
const std::string ZONE_SEPARATOR = "$$";
struct ProblemElementInfo
{
    PROBLEME_ANTARES_A_RESOUDRE* problem;
    int timeStep = 0;
    std::string origin;
    std::string destination;
    std::string area;
};
class VariableNamer
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
    ProblemElementInfo& problemElementInfo;

public:
    VariableNamer(ProblemElementInfo& problemElementInfo) : problemElementInfo(problemElementInfo)
    {
    }

    void DispatchableProduction(int var, const std::string& clusterName);
    void NODU(int var, const std::string& clusterName);
    void NumberStoppingDispatchableUnits(int var, const std::string& clusterName);
    void NumberStartingDispatchableUnits(int var, const std::string& clusterName);
    void NumberBreakingDownDispatchableUnits(int var, const std::string& clusterName);
    void NTCValueOriginToDestination(int var);
    void IntercoCostOriginToDestination(int var);
    void IntercoCostDestinationToOrigin(int var);
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
class ConstraintNamer
{
private:
    ProblemElementInfo& problemElementInfo;

    void BindingConstraint(const std::string& name,
                           Antares::Data::Enum::ExportStructTimeStepDict type);

public:
    ConstraintNamer(ProblemElementInfo& problemElementInfo) : problemElementInfo(problemElementInfo)
    {
    }
    void FlowDissociation();
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
        BindingConstraint(name, Antares::Data::Enum::ExportStructTimeStepDict::hour);
    }
    void BindingConstraintDay(const std::string& name)
    {
        BindingConstraint(name, Antares::Data::Enum::ExportStructTimeStepDict::day);
    }
    void BindingConstraintWeek(const std::string& name)
    {
        BindingConstraint(name, Antares::Data::Enum::ExportStructTimeStepDict::week);
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