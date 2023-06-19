#include "opt_rename_problem.h"
#include <sstream>

const std::string HOUR("hour");
const std::string DAY("day");
const std::string WEEK("week");
const std::map<std::string, std::string> BindingConstraintTimeGranularity
  = {{HOUR, "hourly"}, {DAY, "daily"}, {WEEK, "weekly"}};
const std::string LINK("link");
const std::string AREA("area");

void IVariableNamer::SetLinkVariableName(int var, const std::string& variableType)
{
    if (problem_->NomDesVariables[var].empty())
    {
        const auto location = origin_ + AREA_SEP + destination_;
        problem_->NomDesVariables[var] = BuildName(
          variableType, LocationIdentifier(location, LINK), TimeIdentifier(timeStep_, HOUR));
    }
}

void IVariableNamer::SetAreaVariableName(int var, const std::string& variableType)
{
    problem_->NomDesVariables[var]
      = BuildName(variableType, LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, HOUR));
}

void IVariableNamer::SetAreaVariableName(int var, const std::string& variableType, int layerIndex)
{
    problem_->NomDesVariables[var] = BuildName(
      variableType,
      LocationIdentifier(area_, AREA) + SEPARATOR + "Layer<" + std::to_string(layerIndex) + ">",
      TimeIdentifier(timeStep_, HOUR));
}

void IVariableNamer::SetThermalClusterVariableName(int var,
                                                   const std::string& variableType,
                                                   const std::string& clusterName)
{
    const auto location
      = LocationIdentifier(area_, AREA) + SEPARATOR + "ThermalCluster" + "<" + clusterName + ">";

    problem_->NomDesVariables[var]
      = BuildName(variableType, location, TimeIdentifier(timeStep_, HOUR));
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

void IVariableNamer::SetShortTermStorageVariableName(int var,
                                                     const std::string& variableType,
                                                     const std::string& shortTermStorageName)
{
    const auto location = LocationIdentifier(area_, AREA) + SEPARATOR + "ShortTermStorage" + "<"
                          + shortTermStorageName + ">";
    auto fullName = BuildName(variableType, location, TimeIdentifier(timeStep_, HOUR));
    problem_->NomDesVariables[var] = fullName;
}

void VariableNamer::ShortTermStorageInjection(int var, const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(var, "ShortTermStorageInjection", shortTermStorageName);
}

void VariableNamer::ShortTermStorageWithdrawal(int var, const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(var, "ShortTermStorageWithdrawal", shortTermStorageName);
}

void VariableNamer::ShortTermStorageLevel(int var, const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(var, "ShortTermStorageLevel", shortTermStorageName);
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
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName("FlowDissociation",
                  LocationIdentifier(origin + AREA_SEP + destination, LINK),
                  TimeIdentifier(timeStep_, HOUR));
}

void ConstraintNamer::AreaBalance()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName("AreaBalance", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, HOUR));
}

void ConstraintNamer::FictiveLoads()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName("FictiveLoads", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, HOUR));
}

void ConstraintNamer::HydroPower()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName("HydroPower", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, WEEK));
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationSum()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName("HydroPowerSmoothingUsingVariationSum",
                  LocationIdentifier(area_, AREA),
                  TimeIdentifier(timeStep_, HOUR));
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxDown()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName("HydroPowerSmoothingUsingVariationMaxDown",
                  LocationIdentifier(area_, AREA),
                  TimeIdentifier(timeStep_, HOUR));
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxUp()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName("HydroPowerSmoothingUsingVariationMaxUp",
                  LocationIdentifier(area_, AREA),
                  TimeIdentifier(timeStep_, HOUR));
}

void ConstraintNamer::MinHydroPower()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1] = BuildName(
      "MinHydroPower", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, WEEK));
}

void ConstraintNamer::MaxHydroPower()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1] = BuildName(
      "MaxHydroPower", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, WEEK));
}

void ConstraintNamer::MaxPumping()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName("MaxPumping", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, WEEK));
}

void ConstraintNamer::AreaHydroLevel()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1] = BuildName(
      "AreaHydroLevel", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, HOUR));
}

void ConstraintNamer::FinalStockEquivalent()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1] = BuildName(
      "FinalStockEquivalent", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, HOUR));
}

void ConstraintNamer::FinalStockExpression()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1] = BuildName(
      "FinalStockExpression", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, HOUR));
}

void IConstraintNamer::nameWithTimeGranularity(const std::string& name, const std::string& type)
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(name, BindingConstraintTimeGranularity.at(type), TimeIdentifier(timeStep_, type));
}

void ConstraintNamer::MinUpTime()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName("MinUpTime", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, HOUR));
}

void ConstraintNamer::MinDownTime()
{
    std::string constraintFullName
      = BuildName("MinDownTime", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, HOUR));
}

void ConstraintNamer::PMaxDispatchableGeneration()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName("PMaxDispatchableGeneration",
                  LocationIdentifier(area_, AREA),
                  TimeIdentifier(timeStep_, HOUR));
}

void ConstraintNamer::PMinDispatchableGeneration()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName("PMinDispatchableGeneration",
                  LocationIdentifier(area_, AREA),
                  TimeIdentifier(timeStep_, HOUR));
}

void ConstraintNamer::ConsistenceNODU()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1] = BuildName(
      "ConsistenceNODU", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, HOUR));
}

void ConstraintNamer::ShortTermStorageLevel()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1] = BuildName(
      "ShortTermStorageLevel", LocationIdentifier(area_, AREA), TimeIdentifier(timeStep_, HOUR));
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

std::string BuildName(const std::string& name,
                      const std::string& location,
                      const std::string& timeIdentifier)
{
    std::string result = name + SEPARATOR + location + SEPARATOR + timeIdentifier;
    std::replace(result.begin(), result.end(), ' ', '*');
    return result;
}

SPConstraintsNamer ConstraintsNamerFactory(PROBLEME_ANTARES_A_RESOUDRE* problem, bool namedProblem)
{
    if (namedProblem)
    {
        return std::make_shared<ConstraintNamer>(problem);
    }
    else
    {
        return std::make_shared<EmptyConstraintNamer>(problem);
    }
}
SPVariableNamer VariablesNamerFactory(PROBLEME_ANTARES_A_RESOUDRE* problem, bool namedProblem)
{
    if (namedProblem)
    {
        return std::make_shared<VariableNamer>(problem);
    }
    else
    {
        return std::make_shared<EmptyVariableNamer>(problem);
    }
}