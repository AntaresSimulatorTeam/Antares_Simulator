#include "opt_rename_problem.h"
#include <sstream>

const std::string HOUR("hour");
const std::string DAY("day");
const std::string WEEK("week");
const std::map<std::string, std::string> BindingConstraintTimeGranularity
  = {{HOUR, "hourly"}, {DAY, "daily"}, {WEEK, "weekly"}};
const std::string LINK("link");
const std::string AREA("area");

std::string BuildName(const std::string& name,
                      const std::string& location,
                      const std::string& timeIdentifier)
{
    std::string result = name + SEPARATOR + location + SEPARATOR + timeIdentifier;
    std::replace(result.begin(), result.end(), ' ', '*');
    return result;
}

void Namer::SetLinkElementName(unsigned int element, const std::string& elementType)
{
    const auto location = origin_ + AREA_SEP + destination_;
    targetUpdater_.UpdateTargetAtIndex(
      BuildName(elementType, LocationIdentifier(location, LINK), TimeIdentifier(timeStep_, HOUR)),
      element);
}

void Namer::SetAreaElementNameHour(unsigned int element, const std::string& elementType)
{
    SetAreaElementName(element, elementType, HOUR);
}

void Namer::SetAreaElementNameWeek(unsigned int element, const std::string& elementType)
{
    SetAreaElementName(element, elementType, WEEK);
}

void Namer::SetAreaElementName(unsigned int element,
                               const std::string& elementType,
                               const std::string& timeStepType)
{
    targetUpdater_.UpdateTargetAtIndex(
      BuildName(
        elementType, LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, timeStepType)),
      element);
}

void VariableNamer::SetAreaVariableName(unsigned int variable,
                                        const std::string& variableType,
                                        int layerIndex)
{
    targetUpdater_.UpdateTargetAtIndex(BuildName(variableType,
                                                 LocationIdentifier(area_, AREA) + SEPARATOR
                                                   + "Layer<" + std::to_string(layerIndex) + ">",
                                                 TimeIdentifier(timeStep_, HOUR)),
                                       variable);
}

void Namer::SetThermalClusterElementName(unsigned int variable,
                                         const std::string& elementType,
                                         const std::string& clusterName)
{
    const auto location
      = LocationIdentifier(area_, AREA) + SEPARATOR + "ThermalCluster" + "<" + clusterName + ">";

    targetUpdater_.UpdateTargetAtIndex(
      BuildName(elementType, location, TimeIdentifier(timeStep_, HOUR)), variable);
}

void VariableNamer::DispatchableProduction(unsigned int variable, const std::string& clusterName)
{
    SetThermalClusterElementName(variable, "DispatchableProduction", clusterName);
}

void VariableNamer::NODU(unsigned int variable, const std::string& clusterName)
{
    SetThermalClusterElementName(variable, "NODU", clusterName);
}

void VariableNamer::NumberStoppingDispatchableUnits(unsigned int variable,
                                                    const std::string& clusterName)
{
    SetThermalClusterElementName(variable, "NumberStoppingDispatchableUnits", clusterName);
}

void VariableNamer::NumberStartingDispatchableUnits(unsigned int variable,
                                                    const std::string& clusterName)
{
    SetThermalClusterElementName(variable, "NumberStartingDispatchableUnits", clusterName);
}

void VariableNamer::NumberBreakingDownDispatchableUnits(unsigned int variable,
                                                        const std::string& clusterName)
{
    SetThermalClusterElementName(variable, "NumberBreakingDownDispatchableUnits", clusterName);
}

void VariableNamer::NTCDirect(unsigned int variable,
                              const std::string& origin,
                              const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
    SetLinkElementName(variable, "NTCDirect");
}

void VariableNamer::IntercoDirectCost(unsigned int variable,
                                      const std::string& origin,
                                      const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
    SetLinkElementName(variable, "IntercoDirectCost");
}

void VariableNamer::IntercoIndirectCost(unsigned int variable,
                                        const std::string& origin,
                                        const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
    SetLinkElementName(variable, "IntercoIndirectCost");
}

void VariableNamer::SetShortTermStorageVariableName(unsigned int variable,
                                                    const std::string& variableType,
                                                    const std::string& shortTermStorageName)
{
    const auto location = LocationIdentifier(area_, AREA) + SEPARATOR + "ShortTermStorage" + "<"
                          + shortTermStorageName + ">";
    targetUpdater_.UpdateTargetAtIndex(
      BuildName(variableType, location, TimeIdentifier(timeStep_, HOUR)), variable);
}

void VariableNamer::ShortTermStorageInjection(unsigned int variable,
                                              const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(variable, "Injection", shortTermStorageName);
}

void VariableNamer::ShortTermStorageWithdrawal(unsigned int variable,
                                               const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(variable, "Withdrawal", shortTermStorageName);
}

void VariableNamer::ShortTermStorageLevel(unsigned int variable,
                                          const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(variable, "Level", shortTermStorageName);
}

void VariableNamer::HydProd(unsigned int variable)
{
    SetAreaElementNameHour(variable, "HydProd");
}

void VariableNamer::HydProdDown(unsigned int variable)
{
    SetAreaElementNameHour(variable, "HydProdDown");
}

void VariableNamer::HydProdUp(unsigned int variable)
{
    SetAreaElementNameHour(variable, "HydProdUp");
}

void VariableNamer::Pumping(unsigned int variable)
{
    SetAreaElementNameHour(variable, "Pumping");
}

void VariableNamer::HydroLevel(unsigned int variable)
{
    SetAreaElementNameHour(variable, "HydroLevel");
}

void VariableNamer::Overflow(unsigned int variable)
{
    SetAreaElementNameHour(variable, "Overflow");
}

void VariableNamer::LayerStorage(unsigned int variable, int layerIndex)
{
    SetAreaVariableName(variable, "LayerStorage", layerIndex);
}

void VariableNamer::FinalStorage(unsigned int variable)
{
    SetAreaElementNameHour(variable, "FinalStorage");
}

void VariableNamer::PositiveUnsuppliedEnergy(unsigned int variable)
{
    SetAreaElementNameHour(variable, "PositiveUnsuppliedEnergy");
}

void VariableNamer::NegativeUnsuppliedEnergy(unsigned int variable)
{
    SetAreaElementNameHour(variable, "NegativeUnsuppliedEnergy");
}

void VariableNamer::AreaBalance(unsigned int variable)
{
    SetAreaElementNameHour(variable, "AreaBalance");
}

void ConstraintNamer::FlowDissociation(unsigned int constraint,
                                       const std::string& origin,
                                       const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
    SetLinkElementName(constraint, "FlowDissociation");
}

void ConstraintNamer::CsrFlowDissociation(unsigned int constraint,
                                          const std::string& origin,
                                          const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
    SetLinkElementName(constraint, "CsrFlowDissociation");
}

void ConstraintNamer::AreaBalance(unsigned int constraint)
{
    SetAreaElementNameHour(constraint, "AreaBalance");
}

void ConstraintNamer::FictiveLoads(unsigned int constraint)
{
    SetAreaElementNameHour(constraint, "FictiveLoads");
}

void ConstraintNamer::HydroPower(unsigned int constraint)
{
    SetAreaElementNameWeek(constraint, "HydroPower");
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationSum(unsigned int constraint)
{
    SetAreaElementNameHour(constraint, "HydroPowerSmoothingUsingVariationSum");
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxDown(unsigned int constraint)
{
    SetAreaElementNameHour(constraint, "HydroPowerSmoothingUsingVariationMaxDown");
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxUp(unsigned int constraint)
{
    SetAreaElementNameHour(constraint, "HydroPowerSmoothingUsingVariationMaxUp");
}

void ConstraintNamer::MinHydroPower(unsigned int constraint)
{
    SetAreaElementNameWeek(constraint, "MinHydroPower");
}

void ConstraintNamer::MaxHydroPower(unsigned int constraint)
{
    SetAreaElementNameWeek(constraint, "MaxHydroPower");
}

void ConstraintNamer::MaxPumping(unsigned int constraint)
{
    SetAreaElementNameWeek(constraint, "MaxPumping");
}

void ConstraintNamer::AreaHydroLevel(unsigned int constraint)
{
    SetAreaElementNameHour(constraint, "AreaHydroLevel");
}

void ConstraintNamer::FinalStockEquivalent(unsigned int constraint)
{
    SetAreaElementNameHour(constraint, "FinalStockEquivalent");
}

void ConstraintNamer::FinalStockExpression(unsigned int constraint)
{
    SetAreaElementNameHour(constraint, "FinalStockExpression");
}

void ConstraintNamer::nameWithTimeGranularity(unsigned int constraint,
                                              const std::string& name,
                                              const std::string& type)
{
    targetUpdater_.UpdateTargetAtIndex(
      BuildName(name, BindingConstraintTimeGranularity.at(type), TimeIdentifier(timeStep_, type)),
      constraint);
}

void ConstraintNamer::NbUnitsOutageLessThanNbUnitsStop(unsigned int constraint,
                                                       const std::string& clusterName)
{
    SetThermalClusterElementName(constraint, "NbUnitsOutageLessThanNbUnitsStop", clusterName);
}

void ConstraintNamer::NbDispUnitsMinBoundSinceMinUpTime(unsigned int constraint,
                                                        const std::string& clusterName)
{
    SetThermalClusterElementName(constraint, "NbDispUnitsMinBoundSinceMinUpTime", clusterName);
}

void ConstraintNamer::MinDownTime(unsigned int constraint, const std::string& clusterName)
{
    SetThermalClusterElementName(constraint, "MinDownTime", clusterName);
}

void ConstraintNamer::PMaxDispatchableGeneration(unsigned int constraint,
                                                 const std::string& clusterName)
{
    SetThermalClusterElementName(constraint, "PMaxDispatchableGeneration", clusterName);
}

void ConstraintNamer::PMinDispatchableGeneration(unsigned int constraint,
                                                 const std::string& clusterName)
{
    SetThermalClusterElementName(constraint, "PMinDispatchableGeneration", clusterName);
}

void ConstraintNamer::ConsistenceNODU(unsigned int constraint, const std::string& clusterName)
{
    SetThermalClusterElementName(constraint, "ConsistenceNODU", clusterName);
}

void ConstraintNamer::ShortTermStorageLevel(unsigned int constraint, const std::string& name)
{
    targetUpdater_.UpdateTargetAtIndex(
      BuildName("Level",
                LocationIdentifier(area_, AREA) + SEPARATOR + "ShortTermStorage" + "<" + name + ">",
                TimeIdentifier(timeStep_, HOUR)),
      constraint);
}

void ConstraintNamer::BindingConstraintHour(unsigned int constraint, const std::string& name)
{
    nameWithTimeGranularity(constraint, name, HOUR);
}

void ConstraintNamer::BindingConstraintDay(unsigned int constraint, const std::string& name)
{
    nameWithTimeGranularity(constraint, name, DAY);
}

void ConstraintNamer::BindingConstraintWeek(unsigned int constraint, const std::string& name)
{
    nameWithTimeGranularity(constraint, name, WEEK);
}
