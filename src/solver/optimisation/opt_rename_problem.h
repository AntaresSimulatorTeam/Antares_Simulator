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

    void BindingConstraint(int constraint,
                           const std::string& name,
                           Antares::Data::Enum::ExportStructTimeStepDict type);

public:
    ConstraintNamer(ProblemElementInfo& problemElementInfo) : problemElementInfo(problemElementInfo)
    {
    }
    void FlowDissociation(int constraint);
    void AreaBalance(int constraint);
    void FictiveLoads(int constraint);
    void HydroPower(int constraint);
    void HydroPowerSmoothingUsingVariationSum(int constraint);
    void HydroPowerSmoothingUsingVariationMaxDown(int constraint);
    void HydroPowerSmoothingUsingVariationMaxUp(int constraint);
    void MinHydroPower(int constraint);
    void MaxHydroPower(int constraint);
    void MaxPumping(int constraint);
    void AreaHydroLevel(int constraint);
    void FinalStockEquivalent(int constraint);
    void FinalStockExpression(int constraint);
    void MinUpTime(int constraint);
    void MinDownTime(int constraint);
    void PMaxDispatchableGeneration(int constraint);
    void PMinDispatchableGeneration(int constraint);
    void ConsistenceNODU(int constraint);
    void ShortTermStorageLevel(int constraint);
    void BindingConstraintHour(int constraint, const std::string& name)
    {
        BindingConstraint(constraint, name, Antares::Data::Enum::ExportStructTimeStepDict::hour);
    }
    void BindingConstraintDay(int constraint, const std::string& name)
    {
        BindingConstraint(constraint, name, Antares::Data::Enum::ExportStructTimeStepDict::day);
    }
    void BindingConstraintWeek(int constraint, const std::string& name)
    {
        BindingConstraint(constraint, name, Antares::Data::Enum::ExportStructTimeStepDict::week);
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