#include "opt_rename_problem.h"
#include <sstream>

const std::string HOUR("hour");
const std::string DAY("day");
const std::string WEEK("week");
const std::map<std::string, std::string> BindingConstraintTimeGranularity
  = {{HOUR, "hourly"}, {DAY, "daily"}, {WEEK, "weekly"}};
const std::string LINK("link");
const std::string AREA("currentAssetsStorage.area");

void VariableNamer::SetLinkVariableName(int var, const std::string& variableType)
{
    if (name_updater.StringAtIndex(var).empty())
    {
        const auto location
          = currentAssetsStorage.origin + AREA_SEP + currentAssetsStorage.destination;
        name_updater.UpdateTargetAtIndex(
          BuildName(variableType,
                    LocationIdentifier(location, LINK),
                    TimeIdentifier(currentAssetsStorage.timeStep, HOUR)),
          var);
    }
}

void VariableNamer::SetAreaVariableName(int var, const std::string& variableType)
{
    name_updater.UpdateTargetAtIndex(BuildName(variableType,
                                               LocationIdentifier(currentAssetsStorage.area, AREA),
                                               TimeIdentifier(currentAssetsStorage.timeStep, HOUR)),
                                     var);
}

void VariableNamer::SetAreaVariableName(int var, const std::string& variableType, int layerIndex)
{
    name_updater.UpdateTargetAtIndex(
      BuildName(variableType,
                LocationIdentifier(currentAssetsStorage.area, AREA) + SEPARATOR + "Layer<"
                  + std::to_string(layerIndex) + ">",
                TimeIdentifier(currentAssetsStorage.timeStep, HOUR)),
      var);
}

void VariableNamer::SetThermalClusterVariableName(int var,
                                                  const std::string& variableType,
                                                  const std::string& clusterName)
{
    const auto location = LocationIdentifier(currentAssetsStorage.area, AREA) + SEPARATOR
                          + "ThermalCluster" + "<" + clusterName + ">";

    name_updater.UpdateTargetAtIndex(
      BuildName(variableType, location, TimeIdentifier(currentAssetsStorage.timeStep, HOUR)), var);
}

void VariableNamer::DispatchableProduction(int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(var, "DispatchableProduction", clusterName);
}

void VariableNamer::NODU(int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(var, "NODU", clusterName);
}

void VariableNamer::NumberStoppingDispatchableUnits(int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(var, "NumberStoppingDispatchableUnits", clusterName);
}

void VariableNamer::NumberStartingDispatchableUnits(int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(var, "NumberStartingDispatchableUnits", clusterName);
}

void VariableNamer::NumberBreakingDownDispatchableUnits(int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(var, "NumberBreakingDownDispatchableUnits", clusterName);
}

void VariableNamer::NTCDirect(int var, const std::string& origin, const std::string& destination)
{
    currentAssetsStorage.origin = origin;
    currentAssetsStorage.destination = destination;
    SetLinkVariableName(var, "NTCDirect");
}

void VariableNamer::IntercoDirectCost(int var,
                                      const std::string& origin,
                                      const std::string& destination)
{
    currentAssetsStorage.origin = origin;
    currentAssetsStorage.destination = destination;
    SetLinkVariableName(var, "IntercoDirectCost");
}

void VariableNamer::IntercoIndirectCost(int var,
                                        const std::string& origin,
                                        const std::string& destination)
{
    currentAssetsStorage.origin = origin;
    currentAssetsStorage.destination = destination;
    SetLinkVariableName(var, "IntercoIndirectCost");
}

void VariableNamer::SetShortTermStorageVariableName(int var,
                                                    const std::string& variableType,
                                                    const std::string& shortTermStorageName)
{
    const auto location = LocationIdentifier(currentAssetsStorage.area, AREA) + SEPARATOR
                          + "ShortTermStorage" + "<" + shortTermStorageName + ">";
    name_updater.UpdateTargetAtIndex(
      BuildName(variableType, location, TimeIdentifier(currentAssetsStorage.timeStep, HOUR)), var);
}

void VariableNamer::ShortTermStorageInjection(int var, const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(var, "Injection", shortTermStorageName);
}

void VariableNamer::ShortTermStorageWithdrawal(int var, const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(var, "Withdrawal", shortTermStorageName);
}

void VariableNamer::ShortTermStorageLevel(int var, const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(var, "Level", shortTermStorageName);
}

void VariableNamer::HydProd(int var)
{
    SetAreaVariableName(var, "HydProd");
}

void VariableNamer::HydProdDown(int var)
{
    SetAreaVariableName(var, "HydProdDown");
}

void VariableNamer::HydProdUp(int var)
{
    SetAreaVariableName(var, "HydProdUp");
}

void VariableNamer::Pumping(int var)
{
    SetAreaVariableName(var, "Pumping");
}

void VariableNamer::HydroLevel(int var)
{
    SetAreaVariableName(var, "HydroLevel");
}

void VariableNamer::Overflow(int var)
{
    SetAreaVariableName(var, "Overflow");
}

void VariableNamer::LayerStorage(int var, int layerIndex)
{
    SetAreaVariableName(var, "LayerStorage", layerIndex);
}

void VariableNamer::FinalStorage(int var)
{
    SetAreaVariableName(var, "FinalStorage");
}

void VariableNamer::PositiveUnsuppliedEnergy(int var)
{
    SetAreaVariableName(var, "PositiveUnsuppliedEnergy");
}

void VariableNamer::NegativeUnsuppliedEnergy(int var)
{
    SetAreaVariableName(var, "NegativeUnsuppliedEnergy");
}

void VariableNamer::AreaBalance(int var)
{
    SetAreaVariableName(var, "AreaBalance");
}

void ConstraintNamer::FlowDissociation(int numConstraint,
                                       const std::string& origin,
                                       const std::string& destination)
{
    name_updater.UpdateTargetAtIndex(
      BuildName("FlowDissociation",
                LocationIdentifier(origin + AREA_SEP + destination, LINK),
                TimeIdentifier(currentAssetsStorage.timeStep, HOUR)),
      numConstraint);
}

void ConstraintNamer::AreaBalance(int numConstraint)
{
    name_updater.UpdateTargetAtIndex(BuildName("AreaBalance",
                                               LocationIdentifier(currentAssetsStorage.area, AREA),
                                               TimeIdentifier(currentAssetsStorage.timeStep, HOUR)),
                                     numConstraint);
}

void ConstraintNamer::FictiveLoads(int numConstraint)
{
    name_updater.UpdateTargetAtIndex(BuildName("FictiveLoads",
                                               LocationIdentifier(currentAssetsStorage.area, AREA),
                                               TimeIdentifier(currentAssetsStorage.timeStep, HOUR)),
                                     numConstraint);
}

void ConstraintNamer::HydroPower(int numConstraint)
{
    name_updater.UpdateTargetAtIndex(BuildName("HydroPower",
                                               LocationIdentifier(currentAssetsStorage.area, AREA),
                                               TimeIdentifier(currentAssetsStorage.timeStep, WEEK)),
                                     numConstraint);
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationSum(int numConstraint)
{
    name_updater.UpdateTargetAtIndex(BuildName("HydroPowerSmoothingUsingVariationSum",
                                               LocationIdentifier(currentAssetsStorage.area, AREA),
                                               TimeIdentifier(currentAssetsStorage.timeStep, HOUR)),
                                     numConstraint);
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxDown(int numConstraint)
{
    name_updater.UpdateTargetAtIndex(BuildName("HydroPowerSmoothingUsingVariationMaxDown",
                                               LocationIdentifier(currentAssetsStorage.area, AREA),
                                               TimeIdentifier(currentAssetsStorage.timeStep, HOUR)),
                                     numConstraint);
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxUp(int numConstraint)
{
    name_updater.UpdateTargetAtIndex(BuildName("HydroPowerSmoothingUsingVariationMaxUp",
                                               LocationIdentifier(currentAssetsStorage.area, AREA),
                                               TimeIdentifier(currentAssetsStorage.timeStep, HOUR)),
                                     numConstraint);
}

void ConstraintNamer::MinHydroPower(int numConstraint)
{
    name_updater.UpdateTargetAtIndex(BuildName("MinHydroPower",
                                               LocationIdentifier(currentAssetsStorage.area, AREA),
                                               TimeIdentifier(currentAssetsStorage.timeStep, WEEK)),
                                     numConstraint);
}

void ConstraintNamer::MaxHydroPower(int numConstraint)
{
    name_updater.UpdateTargetAtIndex(BuildName("MaxHydroPower",
                                               LocationIdentifier(currentAssetsStorage.area, AREA),
                                               TimeIdentifier(currentAssetsStorage.timeStep, WEEK)),
                                     numConstraint);
}

void ConstraintNamer::MaxPumping(int numConstraint)
{
    name_updater.UpdateTargetAtIndex(BuildName("MaxPumping",
                                               LocationIdentifier(currentAssetsStorage.area, AREA),
                                               TimeIdentifier(currentAssetsStorage.timeStep, WEEK)),
                                     numConstraint);
}

void ConstraintNamer::AreaHydroLevel(int numConstraint)
{
    name_updater.UpdateTargetAtIndex(BuildName("AreaHydroLevel",
                                               LocationIdentifier(currentAssetsStorage.area, AREA),
                                               TimeIdentifier(currentAssetsStorage.timeStep, HOUR)),
                                     numConstraint);
}

void ConstraintNamer::FinalStockEquivalent(int numConstraint)
{
    name_updater.UpdateTargetAtIndex(BuildName("FinalStockEquivalent",
                                               LocationIdentifier(currentAssetsStorage.area, AREA),
                                               TimeIdentifier(currentAssetsStorage.timeStep, HOUR)),
                                     numConstraint);
}

void ConstraintNamer::FinalStockExpression(int numConstraint )
{
    name_updater.UpdateTargetAtIndex(BuildName("FinalStockExpression",
                                               LocationIdentifier(currentAssetsStorage.area, AREA),
                                               TimeIdentifier(currentAssetsStorage.timeStep, HOUR)),
                                     numConstraint);
}

void ConstraintNamer::nameWithTimeGranularity(int numConstraint,
                                              const std::string& name,
                                              const std::string& type)
{
    name_updater.UpdateTargetAtIndex(BuildName(name,
                                               BindingConstraintTimeGranularity.at(type),
                                               TimeIdentifier(currentAssetsStorage.timeStep, type)),
                                     numConstraint);
}

void ConstraintNamer::MinUpTime(int numConstraint)
{
    name_updater.UpdateTargetAtIndex(BuildName("MinUpTime",
                                               LocationIdentifier(currentAssetsStorage.area, AREA),
                                               TimeIdentifier(currentAssetsStorage.timeStep, HOUR)),
                                     numConstraint);
}

void ConstraintNamer::MinDownTime(int numConstraint)
{
    name_updater.UpdateTargetAtIndex(BuildName("MinDownTime",
                                               LocationIdentifier(currentAssetsStorage.area, AREA),
                                               TimeIdentifier(currentAssetsStorage.timeStep, HOUR)),
                                     numConstraint);
}

void ConstraintNamer::PMaxDispatchableGeneration(int numConstraint)
{
    name_updater.UpdateTargetAtIndex(BuildName("PMaxDispatchableGeneration",
                                               LocationIdentifier(currentAssetsStorage.area, AREA),
                                               TimeIdentifier(currentAssetsStorage.timeStep, HOUR)),
                                     numConstraint);
}

void ConstraintNamer::PMinDispatchableGeneration(int numConstraint)
{
    name_updater.UpdateTargetAtIndex(BuildName("PMinDispatchableGeneration",
                                               LocationIdentifier(currentAssetsStorage.area, AREA),
                                               TimeIdentifier(currentAssetsStorage.timeStep, HOUR)),
                                     numConstraint);
}

void ConstraintNamer::ConsistenceNODU(int numConstraint)
{
    name_updater.UpdateTargetAtIndex(BuildName("ConsistenceNODU",
                                               LocationIdentifier(currentAssetsStorage.area, AREA),
                                               TimeIdentifier(currentAssetsStorage.timeStep, HOUR)),
                                     numConstraint);
}

void ConstraintNamer::ShortTermStorageLevel(int numConstraint, const std::string& name)
{
    name_updater.UpdateTargetAtIndex(
      BuildName("Level",
                LocationIdentifier(currentAssetsStorage.area, AREA) + SEPARATOR + "ShortTermStorage"
                  + "<" + name + ">",
                TimeIdentifier(currentAssetsStorage.timeStep, HOUR)),
      numConstraint);
}

void ConstraintNamer::BindingConstraintHour(int numConstraint, const std::string& name)
{
    nameWithTimeGranularity(numConstraint, name, HOUR);
}

void ConstraintNamer::BindingConstraintDay(int numConstraint, const std::string& name)
{
    nameWithTimeGranularity(numConstraint, name, DAY);
}

void ConstraintNamer::BindingConstraintWeek(int numConstraint, const std::string& name)
{
    nameWithTimeGranularity(numConstraint, name, WEEK);
}

std::string BuildName(const std::string& name,
                      const std::string& location,
                      const std::string& timeIdentifier)
{
    std::string result = name + SEPARATOR + location + SEPARATOR + timeIdentifier;
    std::replace(result.begin(), result.end(), ' ', '*');
    return result;
}
