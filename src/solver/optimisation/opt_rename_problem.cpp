#include "opt_rename_problem.h"
#include <sstream>

const std::string HOUR("hour");
const std::string DAY("day");
const std::string WEEK("week");
const std::map<std::string, std::string> BindingConstraintTimeGranularity
  = {{HOUR, "hourly"}, {DAY, "daily"}, {WEEK, "weekly"}};
const std::string LINK("link");
const std::string AREA("area");

void VariableNamer::SetLinkVariableName(int var, const std::string& variableType)
{
    if (problem_->NomDesVariables[var].empty())
    {
        const auto location = origin_ + AREA_SEP + destination_;
        name_updater_.Run(
          variableType, LocationIdentifier(location, LINK), TimeIdentifier(timeStep_, HOUR), var);
    }
}

void VariableNamer::SetAreaVariableName(int var, const std::string& variableType)
{
    name_updater_.Run(
      variableType, LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, HOUR), var);
}

void VariableNamer::SetAreaVariableName(int var, const std::string& variableType, int layerIndex)
{
    name_updater_.Run(
      variableType,
      LocationIdentifier(area_, AREA) + SEPARATOR + "Layer<" + std::to_string(layerIndex) + ">",
      TimeIdentifier(timeStep_, HOUR),
      var);
}

void VariableNamer::SetThermalClusterVariableName(int var,
                                                  const std::string& variableType,
                                                  const std::string& clusterName)
{
    const auto location
      = LocationIdentifier(area_, AREA) + SEPARATOR + "ThermalCluster" + "<" + clusterName + ">";

    name_updater_.Run(variableType, location, TimeIdentifier(timeStep_, HOUR), var);
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
    origin_ = origin;
    destination_ = destination;
    SetLinkVariableName(var, "NTCDirect");
}

void VariableNamer::IntercoDirectCost(int var,
                                      const std::string& origin,
                                      const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
    SetLinkVariableName(var, "IntercoDirectCost");
}

void VariableNamer::IntercoIndirectCost(int var,
                                        const std::string& origin,
                                        const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
    SetLinkVariableName(var, "IntercoIndirectCost");
}

void VariableNamer::SetShortTermStorageVariableName(int var,
                                                    const std::string& variableType,
                                                    const std::string& shortTermStorageName)
{
    const auto location = LocationIdentifier(area_, AREA) + SEPARATOR + "ShortTermStorage" + "<"
                          + shortTermStorageName + ">";
    name_updater_.Run(variableType, location, TimeIdentifier(timeStep_, HOUR), var);
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

void ConstraintNamer::FlowDissociation(const std::string& origin, const std::string& destination)
{
    name_updater_.Run("FlowDissociation",
                      LocationIdentifier(origin + AREA_SEP + destination, LINK),
                      TimeIdentifier(timeStep_, HOUR),
                      problem_->NombreDeContraintes - 1);
}

void ConstraintNamer::AreaBalance()
{
    name_updater_.Run("AreaBalance",
                      LocationIdentifier(area_, AREA),
                      TimeIdentifier(timeStep_, HOUR),
                      problem_->NombreDeContraintes - 1);
}

void ConstraintNamer::FictiveLoads()
{
    name_updater_.Run("FictiveLoads",
                      LocationIdentifier(area_, AREA),
                      TimeIdentifier(timeStep_, HOUR),
                      problem_->NombreDeContraintes - 1);
}

void ConstraintNamer::HydroPower()
{
    name_updater_.Run("HydroPower",
                      LocationIdentifier(area_, AREA),
                      TimeIdentifier(timeStep_, WEEK),
                      problem_->NombreDeContraintes - 1);
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationSum()
{
    name_updater_.Run("HydroPowerSmoothingUsingVariationSum",
                      LocationIdentifier(area_, AREA),
                      TimeIdentifier(timeStep_, HOUR),
                      problem_->NombreDeContraintes - 1);
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxDown()
{
    name_updater_.Run("HydroPowerSmoothingUsingVariationMaxDown",
                      LocationIdentifier(area_, AREA),
                      TimeIdentifier(timeStep_, HOUR),
                      problem_->NombreDeContraintes - 1);
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxUp()
{
    name_updater_.Run("HydroPowerSmoothingUsingVariationMaxUp",
                      LocationIdentifier(area_, AREA),
                      TimeIdentifier(timeStep_, HOUR),
                      problem_->NombreDeContraintes - 1);
}

void ConstraintNamer::MinHydroPower()
{
    name_updater_.Run("MinHydroPower",
                      LocationIdentifier(area_, AREA),
                      TimeIdentifier(timeStep_, WEEK),
                      problem_->NombreDeContraintes - 1);
}

void ConstraintNamer::MaxHydroPower()
{
    name_updater_.Run("MaxHydroPower",
                      LocationIdentifier(area_, AREA),
                      TimeIdentifier(timeStep_, WEEK),
                      problem_->NombreDeContraintes - 1);
}

void ConstraintNamer::MaxPumping()
{
      name_updater_.Run("MaxPumping",
                        LocationIdentifier(area_, AREA),
                        TimeIdentifier(timeStep_, WEEK),
                        problem_->NombreDeContraintes - 1);
}

void ConstraintNamer::AreaHydroLevel()
{
     name_updater_.Run("AreaHydroLevel",
                       LocationIdentifier(area_, AREA),
                       TimeIdentifier(timeStep_, HOUR),
                       problem_->NombreDeContraintes - 1);
}

void ConstraintNamer::FinalStockEquivalent()
{
     name_updater_.Run("FinalStockEquivalent",
                       LocationIdentifier(area_, AREA),
                       TimeIdentifier(timeStep_, HOUR),
                       problem_->NombreDeContraintes - 1);
}

void ConstraintNamer::FinalStockExpression()
{
     name_updater_.Run("FinalStockExpression",
                       LocationIdentifier(area_, AREA),
                       TimeIdentifier(timeStep_, HOUR),
                       problem_->NombreDeContraintes - 1);
}

void ConstraintNamer::nameWithTimeGranularity(const std::string& name, const std::string& type)
{
      name_updater_.Run(name,
                        BindingConstraintTimeGranularity.at(type),
                        TimeIdentifier(timeStep_, type),
                        problem_->NombreDeContraintes - 1);
}

void ConstraintNamer::MinUpTime()
{
      name_updater_.Run("MinUpTime",
                        LocationIdentifier(area_, AREA),
                        TimeIdentifier(timeStep_, HOUR),
                        problem_->NombreDeContraintes - 1);
}

void ConstraintNamer::MinDownTime()
{
      name_updater_.Run("MinDownTime",
                        LocationIdentifier(area_, AREA),
                        TimeIdentifier(timeStep_, HOUR),
                        problem_->NombreDeContraintes - 1);
}

void ConstraintNamer::PMaxDispatchableGeneration()
{
     name_updater_.Run("PMaxDispatchableGeneration",
                       LocationIdentifier(area_, AREA),
                       TimeIdentifier(timeStep_, HOUR),
                       problem_->NombreDeContraintes - 1);
}

void ConstraintNamer::PMinDispatchableGeneration()
{
     name_updater_.Run("PMinDispatchableGeneration",
                       LocationIdentifier(area_, AREA),
                       TimeIdentifier(timeStep_, HOUR),
                       problem_->NombreDeContraintes - 1);
}

void ConstraintNamer::ConsistenceNODU()
{
     name_updater_.Run("ConsistenceNODU",
                       LocationIdentifier(area_, AREA),
                       TimeIdentifier(timeStep_, HOUR),
                       problem_->NombreDeContraintes - 1);
}

void ConstraintNamer::ShortTermStorageLevel(const std::string& name)
{
     name_updater_.Run(
       "Level",
       LocationIdentifier(area_, AREA) + SEPARATOR + "ShortTermStorage" + "<" + name + ">",
       TimeIdentifier(timeStep_, HOUR),
       problem_->NombreDeContraintes - 1);
}

void ConstraintNamer::BindingConstraintHour(const std::string& name)
{
    nameWithTimeGranularity(name, HOUR);
}

void ConstraintNamer::BindingConstraintDay(const std::string& name)
{
    nameWithTimeGranularity(name, DAY);
}

void ConstraintNamer::BindingConstraintWeek(const std::string& name)
{
    nameWithTimeGranularity(name, WEEK);
}

void NameUpdater::BuildName(const std::string& name,
                            const std::string& location,
                            const std::string& timeIdentifier,
                            unsigned index)
{
    std::string result = name + SEPARATOR + location + SEPARATOR + timeIdentifier;
    std::replace(result.begin(), result.end(), ' ', '*');
    target_[index] = result;
}
