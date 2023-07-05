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

void VariableNamer::SetLinkVariableName(unsigned int var, const std::string& variableType)
{
    const auto location = origin_ + AREA_SEP + destination_;
    targetUpdater_.UpdateTargetAtIndex(
      BuildName(variableType, LocationIdentifier(location, LINK), TimeIdentifier(timeStep_, HOUR)),
      var);
}

void VariableNamer::SetAreaVariableName(unsigned int var, const std::string& variableType)
{
    targetUpdater_.UpdateTargetAtIndex(
      BuildName(variableType, LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, HOUR)),
      var);
}

void VariableNamer::SetAreaVariableName(unsigned int var, const std::string& variableType, int layerIndex)
{
    targetUpdater_.UpdateTargetAtIndex(BuildName(variableType,
                                                 LocationIdentifier(area_, AREA) + SEPARATOR
                                                   + "Layer<" + std::to_string(layerIndex) + ">",
                                                 TimeIdentifier(timeStep_, HOUR)),
                                       var);
}

void VariableNamer::SetThermalClusterVariableName(unsigned int var,
                                                  const std::string& variableType,
                                                  const std::string& clusterName)
{
    const auto location
      = LocationIdentifier(area_, AREA) + SEPARATOR + "ThermalCluster" + "<" + clusterName + ">";

    targetUpdater_.UpdateTargetAtIndex(
      BuildName(variableType, location, TimeIdentifier(timeStep_, HOUR)), var);
}

void VariableNamer::DispatchableProduction(unsigned int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(var, "DispatchableProduction", clusterName);
}

void VariableNamer::NODU(unsigned int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(var, "NODU", clusterName);
}

void VariableNamer::NumberStoppingDispatchableUnits(unsigned int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(var, "NumberStoppingDispatchableUnits", clusterName);
}

void VariableNamer::NumberStartingDispatchableUnits(unsigned int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(var, "NumberStartingDispatchableUnits", clusterName);
}

void VariableNamer::NumberBreakingDownDispatchableUnits(unsigned int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(var, "NumberBreakingDownDispatchableUnits", clusterName);
}

void VariableNamer::NTCDirect(unsigned int var, const std::string& origin, const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
    SetLinkVariableName(var, "NTCDirect");
}

void VariableNamer::IntercoDirectCost(unsigned int var,
                                      const std::string& origin,
                                      const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
    SetLinkVariableName(var, "IntercoDirectCost");
}

void VariableNamer::IntercoIndirectCost(unsigned int var,
                                        const std::string& origin,
                                        const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
    SetLinkVariableName(var, "IntercoIndirectCost");
}

void VariableNamer::SetShortTermStorageVariableName(unsigned int var,
                                                    const std::string& variableType,
                                                    const std::string& shortTermStorageName)
{
    const auto location = LocationIdentifier(area_, AREA) + SEPARATOR + "ShortTermStorage" + "<"
                          + shortTermStorageName + ">";
    targetUpdater_.UpdateTargetAtIndex(
      BuildName(variableType, location, TimeIdentifier(timeStep_, HOUR)), var);
}

void VariableNamer::ShortTermStorageInjection(unsigned int var, const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(var, "Injection", shortTermStorageName);
}

void VariableNamer::ShortTermStorageWithdrawal(unsigned int var, const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(var, "Withdrawal", shortTermStorageName);
}

void VariableNamer::ShortTermStorageLevel(unsigned int var, const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(var, "Level", shortTermStorageName);
}

void VariableNamer::HydProd(unsigned int var)
{
    SetAreaVariableName(var, "HydProd");
}

void VariableNamer::HydProdDown(unsigned int var)
{
    SetAreaVariableName(var, "HydProdDown");
}

void VariableNamer::HydProdUp(unsigned int var)
{
    SetAreaVariableName(var, "HydProdUp");
}

void VariableNamer::Pumping(unsigned int var)
{
    SetAreaVariableName(var, "Pumping");
}

void VariableNamer::HydroLevel(unsigned int var)
{
    SetAreaVariableName(var, "HydroLevel");
}

void VariableNamer::Overflow(unsigned int var)
{
    SetAreaVariableName(var, "Overflow");
}

void VariableNamer::LayerStorage(unsigned int var, int layerIndex)
{
    SetAreaVariableName(var, "LayerStorage", layerIndex);
}

void VariableNamer::FinalStorage(unsigned int var)
{
    SetAreaVariableName(var, "FinalStorage");
}

void VariableNamer::PositiveUnsuppliedEnergy(unsigned int var)
{
    SetAreaVariableName(var, "PositiveUnsuppliedEnergy");
}

void VariableNamer::NegativeUnsuppliedEnergy(unsigned int var)
{
    SetAreaVariableName(var, "NegativeUnsuppliedEnergy");
}

void VariableNamer::AreaBalance(unsigned int var)
{
    SetAreaVariableName(var, "AreaBalance");
}

void ConstraintNamer::FlowDissociation(unsigned int numConstraint,
                                       const std::string& origin,
                                       const std::string& destination)
{
    targetUpdater_.UpdateTargetAtIndex(
      BuildName("FlowDissociation",
                LocationIdentifier(origin + AREA_SEP + destination, LINK),
                TimeIdentifier(timeStep_, HOUR)),
      numConstraint);
}

void ConstraintNamer::AreaBalance(unsigned int numConstraint)
{
    targetUpdater_.UpdateTargetAtIndex(
      BuildName("AreaBalance", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, HOUR)),
      numConstraint);
}

void ConstraintNamer::FictiveLoads(unsigned int numConstraint)
{
    targetUpdater_.UpdateTargetAtIndex(
      BuildName("FictiveLoads", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, HOUR)),
      numConstraint);
}

void ConstraintNamer::HydroPower(unsigned int numConstraint)
{
    targetUpdater_.UpdateTargetAtIndex(
      BuildName("HydroPower", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, WEEK)),
      numConstraint);
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationSum(unsigned int numConstraint)
{
    targetUpdater_.UpdateTargetAtIndex(BuildName("HydroPowerSmoothingUsingVariationSum",
                                                 LocationIdentifier(area_, AREA),
                                                 TimeIdentifier(timeStep_, HOUR)),
                                       numConstraint);
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxDown(unsigned int numConstraint)
{
    targetUpdater_.UpdateTargetAtIndex(BuildName("HydroPowerSmoothingUsingVariationMaxDown",
                                                 LocationIdentifier(area_, AREA),
                                                 TimeIdentifier(timeStep_, HOUR)),
                                       numConstraint);
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxUp(unsigned int numConstraint)
{
    targetUpdater_.UpdateTargetAtIndex(BuildName("HydroPowerSmoothingUsingVariationMaxUp",
                                                 LocationIdentifier(area_, AREA),
                                                 TimeIdentifier(timeStep_, HOUR)),
                                       numConstraint);
}

void ConstraintNamer::MinHydroPower(unsigned int numConstraint)
{
    targetUpdater_.UpdateTargetAtIndex(
      BuildName("MinHydroPower", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, WEEK)),
      numConstraint);
}

void ConstraintNamer::MaxHydroPower(unsigned int numConstraint)
{
    targetUpdater_.UpdateTargetAtIndex(
      BuildName("MaxHydroPower", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, WEEK)),
      numConstraint);
}

void ConstraintNamer::MaxPumping(unsigned int numConstraint)
{
    targetUpdater_.UpdateTargetAtIndex(
      BuildName("MaxPumping", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, WEEK)),
      numConstraint);
}

void ConstraintNamer::AreaHydroLevel(unsigned int numConstraint)
{
    targetUpdater_.UpdateTargetAtIndex(
      BuildName("AreaHydroLevel", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, HOUR)),
      numConstraint);
}

void ConstraintNamer::FinalStockEquivalent(unsigned int numConstraint)
{
    targetUpdater_.UpdateTargetAtIndex(
      BuildName(
        "FinalStockEquivalent", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, HOUR)),
      numConstraint);
}

void ConstraintNamer::FinalStockExpression(unsigned int numConstraint )
{
    targetUpdater_.UpdateTargetAtIndex(
      BuildName(
        "FinalStockExpression", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, HOUR)),
      numConstraint);
}

void ConstraintNamer::nameWithTimeGranularity(unsigned int numConstraint,
                                              const std::string& name,
                                              const std::string& type)
{
    targetUpdater_.UpdateTargetAtIndex(
      BuildName(name, BindingConstraintTimeGranularity.at(type), TimeIdentifier(timeStep_, type)),
      numConstraint);
}

void ConstraintNamer::NbUnitsOutageLessThanNbUnitsStop(unsigned int numConstraint)
{
    targetUpdater_.UpdateTargetAtIndex(BuildName("NbUnitsOutageLessThanNbUnitsStop",
                                                 LocationIdentifier(area_, AREA),
                                                 TimeIdentifier(timeStep_, HOUR)),
                                       numConstraint);
}

void ConstraintNamer::NbDispUnitsMinBoundSinceMinUpTime(unsigned int numConstraint)
{
    targetUpdater_.UpdateTargetAtIndex(BuildName("NbDispUnitsMinBoundSinceMinUpTime",
                                                 LocationIdentifier(area_, AREA),
                                                 TimeIdentifier(timeStep_, HOUR)),
                                       numConstraint);
}

void ConstraintNamer::MinDownTime(unsigned int numConstraint)
{
    targetUpdater_.UpdateTargetAtIndex(
      BuildName("MinDownTime", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, HOUR)),
      numConstraint);
}

void ConstraintNamer::PMaxDispatchableGeneration(unsigned int numConstraint)
{
    targetUpdater_.UpdateTargetAtIndex(BuildName("PMaxDispatchableGeneration",
                                                 LocationIdentifier(area_, AREA),
                                                 TimeIdentifier(timeStep_, HOUR)),
                                       numConstraint);
}

void ConstraintNamer::PMinDispatchableGeneration(unsigned int numConstraint)
{
    targetUpdater_.UpdateTargetAtIndex(BuildName("PMinDispatchableGeneration",
                                                 LocationIdentifier(area_, AREA),
                                                 TimeIdentifier(timeStep_, HOUR)),
                                       numConstraint);
}

void ConstraintNamer::ConsistenceNODU(unsigned int numConstraint)
{
    targetUpdater_.UpdateTargetAtIndex(
      BuildName(
        "ConsistenceNODU", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, HOUR)),
      numConstraint);
}

void ConstraintNamer::ShortTermStorageLevel(unsigned int numConstraint, const std::string& name)
{
    targetUpdater_.UpdateTargetAtIndex(
      BuildName("Level",
                LocationIdentifier(area_, AREA) + SEPARATOR + "ShortTermStorage" + "<" + name + ">",
                TimeIdentifier(timeStep_, HOUR)),
      numConstraint);
}

void ConstraintNamer::BindingConstraintHour(unsigned int numConstraint, const std::string& name)
{
    nameWithTimeGranularity(numConstraint, name, HOUR);
}

void ConstraintNamer::BindingConstraintDay(unsigned int numConstraint, const std::string& name)
{
    nameWithTimeGranularity(numConstraint, name, DAY);
}

void ConstraintNamer::BindingConstraintWeek(unsigned int numConstraint, const std::string& name)
{
    nameWithTimeGranularity(numConstraint, name, WEEK);
}
