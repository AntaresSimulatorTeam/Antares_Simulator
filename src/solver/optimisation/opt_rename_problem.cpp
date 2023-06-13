#include "opt_rename_problem.h"
#include <sstream>

using namespace Antares::Data::Enum;

void VariableNamer::SetLinkVariableName(int var, ExportStructDict structDict)
{
    auto nvars = currentAssetsStorage.problem->NombreDeVariables;
    if (nvars > var && currentAssetsStorage.problem->NomDesVariables[var].empty())
    {
        const auto location
          = currentAssetsStorage.origin + AREA_SEP + currentAssetsStorage.destination;
        auto fullName = BuildName(
          toString(structDict),
          LocationIdentifier(location, ExportStructLocationDict::link),
          TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::hour));
        currentAssetsStorage.problem->NomDesVariables[var] = fullName;
    }
}

void VariableNamer::SetAreaVariableName(int var, ExportStructDict structDict)
{
    auto nvars = currentAssetsStorage.problem->NombreDeVariables;
    if (nvars > var)
    {
        auto fullName = BuildName(
          toString(structDict),
          LocationIdentifier(currentAssetsStorage.area, ExportStructLocationDict::area),
          TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::hour));
        currentAssetsStorage.problem->NomDesVariables[var] = fullName;
    }
}

void VariableNamer::SetAreaVariableName(int var, ExportStructDict structDict, int layerIndex)
{
    auto nvars = currentAssetsStorage.problem->NombreDeVariables;
    if (nvars > var)
    {
        auto fullName = BuildName(
          toString(structDict),
          LocationIdentifier(currentAssetsStorage.area, ExportStructLocationDict::area) + SEPARATOR
            + "Layer<" + std::to_string(layerIndex) + ">",
          TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::hour));
        currentAssetsStorage.problem->NomDesVariables[var] = fullName;
    }
}

void VariableNamer::SetThermalClusterVariableName(int var,
                                                  ExportStructDict structDict,
                                                  const std::string& clusterName)
{
    auto nvars = currentAssetsStorage.problem->NombreDeVariables;

    if (nvars > var)
    {
        const auto location
          = LocationIdentifier(currentAssetsStorage.area, ExportStructLocationDict::area)
            + SEPARATOR + toString(ExportStructDict::PalierThermique) + "<" + clusterName + ">";

        auto fullName = BuildName(
          toString(structDict),
          location,
          TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::hour));
        currentAssetsStorage.problem->NomDesVariables[var] = fullName;
    }
}

void VariableNamer::DispatchableProduction(int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(var, ExportStructDict::DispatchableProduction, clusterName);
}

void VariableNamer::NODU(int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(
      var, ExportStructDict::NombreDeGroupesEnMarcheDuPalierThermique, clusterName);
}

void VariableNamer::NumberStoppingDispatchableUnits(int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(
      var, ExportStructDict::NombreDeGroupesQuiDemarrentDuPalierThermique, clusterName);
}

void VariableNamer::NumberStartingDispatchableUnits(int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(
      var, ExportStructDict::NombreDeGroupesQuiDemarrentDuPalierThermique, clusterName);
}

void VariableNamer::NumberBreakingDownDispatchableUnits(int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(
      var, ExportStructDict::NombreDeGroupesQuiTombentEnPanneDuPalierThermique, clusterName);
}

void VariableNamer::NTCValueOriginToDestination(int var)
{
    SetLinkVariableName(var, ExportStructDict::ValeurDeNTCOrigineVersExtremite);
}

void VariableNamer::IntercoCostOriginToDestination(int var)
{
    SetLinkVariableName(var, ExportStructDict::CoutOrigineVersExtremiteDeLInterconnexion);
}

void VariableNamer::IntercoCostDestinationToOrigin(int var)
{
    SetLinkVariableName(var, ExportStructDict::CoutExtremiteVersOrigineDeLInterconnexion);
}

void VariableNamer::SetShortTermStorageVariableName(int var,
                                                    ExportStructDict structDict,
                                                    const std::string& shortTermStorageName)
{
    auto nvars = currentAssetsStorage.problem->NombreDeVariables;

    if (nvars > var)
    {
        const auto location
          = LocationIdentifier(currentAssetsStorage.area, ExportStructLocationDict::area)
            + SEPARATOR + toString(ExportStructDict::ShortTermStorage) + "<" + shortTermStorageName
            + ">";
        auto fullName = BuildName(
          toString(structDict),
          location,
          TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::hour));
        currentAssetsStorage.problem->NomDesVariables[var] = fullName;
    }
}

void VariableNamer::ShortTermStorageInjection(int var, const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(
      var, ExportStructDict::ShortTermStorageInjection, shortTermStorageName);
}

void VariableNamer::ShortTermStorageWithdrawal(int var, const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(
      var, ExportStructDict::ShortTermStorageWithdrawal, shortTermStorageName);
}

void VariableNamer::ShortTermStorageLevel(int var, const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(
      var, ExportStructDict::ShortTermStorageLevel, shortTermStorageName);
}

void VariableNamer::HydProd(int var)
{
    SetAreaVariableName(var, ExportStructDict::ProdHyd);
}

void VariableNamer::HydProdDown(int var)
{
    SetAreaVariableName(var, ExportStructDict::ProdHydALaBaisse);
}

void VariableNamer::HydProdUp(int var)
{
    SetAreaVariableName(var, ExportStructDict::ProdHydALaHausse);
}

void VariableNamer::Pumping(int var)
{
    SetAreaVariableName(var, ExportStructDict::Pompage);
}

void VariableNamer::HydroLevel(int var)
{
    SetAreaVariableName(var, ExportStructDict::NiveauHydro);
}

void VariableNamer::Overflow(int var)
{
    SetAreaVariableName(var, ExportStructDict::Debordement);
}

void VariableNamer::LayerStorage(int var, int layerIndex)
{
    SetAreaVariableName(var, ExportStructDict::TrancheDeStock, layerIndex);
}

void VariableNamer::FinalStorage(int var)
{
    SetAreaVariableName(var, ExportStructDict::StockFinal);
}

void VariableNamer::PositiveUnsuppliedEnergy(int var)
{
    SetAreaVariableName(var, ExportStructDict::DefaillancePositive);
}

void VariableNamer::NegativeUnsuppliedEnergy(int var)
{
    SetAreaVariableName(var, ExportStructDict::DefaillanceNegative);
}

void VariableNamer::AreaBalance(int var)
{
    SetAreaVariableName(var, ExportStructDict::BilansPays);
}

void ConstraintNamer::FlowDissociation()
{
    currentAssetsStorage.problem
      ->NomDesContraintes[currentAssetsStorage.problem->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::FlowDissociation),
                  LocationIdentifier(
                    currentAssetsStorage.origin + AREA_SEP + currentAssetsStorage.destination,
                    ExportStructLocationDict::link),
                  TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::AreaBalance()
{
    currentAssetsStorage.problem
      ->NomDesContraintes[currentAssetsStorage.problem->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::AreaBalance),
                  LocationIdentifier(currentAssetsStorage.area, ExportStructLocationDict::area),
                  TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::FictiveLoads()
{
    currentAssetsStorage.problem
      ->NomDesContraintes[currentAssetsStorage.problem->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::FictiveLoads),
                  LocationIdentifier(currentAssetsStorage.area, ExportStructLocationDict::area),
                  TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::HydroPower()
{
    currentAssetsStorage.problem
      ->NomDesContraintes[currentAssetsStorage.problem->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::HydroPower),
                  LocationIdentifier(currentAssetsStorage.area, ExportStructLocationDict::area),
                  TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::week));
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationSum()
{
    currentAssetsStorage.problem
      ->NomDesContraintes[currentAssetsStorage.problem->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::HydroPowerSmoothingUsingVariationSum),
                  LocationIdentifier(currentAssetsStorage.area, ExportStructLocationDict::area),
                  TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxDown()
{
    currentAssetsStorage.problem
      ->NomDesContraintes[currentAssetsStorage.problem->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::HydroPowerSmoothingUsingVariationMaxDown),
                  LocationIdentifier(currentAssetsStorage.area, ExportStructLocationDict::area),
                  TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxUp()
{
    currentAssetsStorage.problem
      ->NomDesContraintes[currentAssetsStorage.problem->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::HydroPowerSmoothingUsingVariationMaxUp),
                  LocationIdentifier(currentAssetsStorage.area, ExportStructLocationDict::area),
                  TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::MinHydroPower()
{
    currentAssetsStorage.problem
      ->NomDesContraintes[currentAssetsStorage.problem->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::MinHydroPower),
                  LocationIdentifier(currentAssetsStorage.area, ExportStructLocationDict::area),
                  TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::week));
}

void ConstraintNamer::MaxHydroPower()
{
    currentAssetsStorage.problem
      ->NomDesContraintes[currentAssetsStorage.problem->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::MaxHydroPower),
                  LocationIdentifier(currentAssetsStorage.area, ExportStructLocationDict::area),
                  TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::week));
}

void ConstraintNamer::MaxPumping()
{
    currentAssetsStorage.problem
      ->NomDesContraintes[currentAssetsStorage.problem->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::MaxPumping),
                  LocationIdentifier(currentAssetsStorage.area, ExportStructLocationDict::area),
                  TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::week));
}

void ConstraintNamer::AreaHydroLevel()
{
    currentAssetsStorage.problem
      ->NomDesContraintes[currentAssetsStorage.problem->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::AreaHydroLevel),
                  LocationIdentifier(currentAssetsStorage.area, ExportStructLocationDict::area),
                  TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::FinalStockEquivalent()
{
    currentAssetsStorage.problem
      ->NomDesContraintes[currentAssetsStorage.problem->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::FinalStockEquivalent),
                  LocationIdentifier(currentAssetsStorage.area, ExportStructLocationDict::area),
                  TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::FinalStockExpression()
{
    currentAssetsStorage.problem
      ->NomDesContraintes[currentAssetsStorage.problem->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::FinalStockExpression),
                  LocationIdentifier(currentAssetsStorage.area, ExportStructLocationDict::area),
                  TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::BindingConstraint(const std::string& name, ExportStructTimeStepDict type)
{
    auto timeStepType
      = (type == ExportStructTimeStepDict::hour)  ? ExportStructBindingConstraintType::hourly
        : (type == ExportStructTimeStepDict::day) ? ExportStructBindingConstraintType::daily
                                                  : ExportStructBindingConstraintType::weekly;
    currentAssetsStorage.problem
      ->NomDesContraintes[currentAssetsStorage.problem->NombreDeContraintes - 1]
      = BuildName(
        name, toString(timeStepType), TimeIdentifier(currentAssetsStorage.timeStep, type));
}

void ConstraintNamer::MinUpTime()
{
    currentAssetsStorage.problem
      ->NomDesContraintes[currentAssetsStorage.problem->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::MinUpTime),
                  LocationIdentifier(currentAssetsStorage.area, ExportStructLocationDict::area),
                  TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::MinDownTime()
{
    std::string constraintFullName
      = BuildName(toString(ExportStructConstraintsDict::MinDownTime),
                  LocationIdentifier(currentAssetsStorage.area, ExportStructLocationDict::area),
                  TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::PMaxDispatchableGeneration()
{
    currentAssetsStorage.problem
      ->NomDesContraintes[currentAssetsStorage.problem->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::PMaxDispatchableGeneration),
                  LocationIdentifier(currentAssetsStorage.area, ExportStructLocationDict::area),
                  TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::PMinDispatchableGeneration()
{
    currentAssetsStorage.problem
      ->NomDesContraintes[currentAssetsStorage.problem->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::PMinDispatchableGeneration),
                  LocationIdentifier(currentAssetsStorage.area, ExportStructLocationDict::area),
                  TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::ConsistenceNODU()
{
    currentAssetsStorage.problem
      ->NomDesContraintes[currentAssetsStorage.problem->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::ConsistenceNODU),
                  LocationIdentifier(currentAssetsStorage.area, ExportStructLocationDict::area),
                  TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::ShortTermStorageLevel()
{
    currentAssetsStorage.problem
      ->NomDesContraintes[currentAssetsStorage.problem->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::ShortTermStorageLevel),
                  LocationIdentifier(currentAssetsStorage.area, ExportStructLocationDict::area),
                  TimeIdentifier(currentAssetsStorage.timeStep, ExportStructTimeStepDict::hour));
}

std::string BuildName(const std::string& name,
                      const std::string& location,
                      const std::string& timeIdentifier)
{
    std::string result = name + SEPARATOR + location + SEPARATOR + timeIdentifier;
    std::replace(result.begin(), result.end(), ' ', '*');
    return result;
}
