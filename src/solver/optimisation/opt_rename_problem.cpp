#include "opt_rename_problem.h"
#include <sstream>
void VariableNamer::SetLinkVariableName(int var, Antares::Data::Enum::ExportStructDict structDict)
{
    auto nvars = problem->NombreDeVariables;
    if (nvars > var && problem->NomDesVariables[var].empty())
    {
        const auto location = origin + ZONE_SEPARATOR + destination;
        auto fullName = BuildName(
          Antares::Data::Enum::toString(structDict),
          LocationIdentifier(location, Antares::Data::Enum::ExportStructLocationDict::link),
          TimeIdentifier(timeStep, Antares::Data::Enum::ExportStructTimeStepDict::hour));
        problem->NomDesVariables[var] = fullName;
    }
}

void VariableNamer::SetAreaVariableName(int var, Antares::Data::Enum::ExportStructDict structDict)
{
    auto nvars = problem->NombreDeVariables;
    if (nvars > var)
    {
        auto fullName = BuildName(
          Antares::Data::Enum::toString(structDict),
          LocationIdentifier(area, Antares::Data::Enum::ExportStructLocationDict::area),
          TimeIdentifier(timeStep, Antares::Data::Enum::ExportStructTimeStepDict::hour));
        problem->NomDesVariables[var] = fullName;
    }
}
void VariableNamer::SetAreaVariableName(int var,
                                        Antares::Data::Enum::ExportStructDict structDict,
                                        int layerIndex)
{
    auto nvars = problem->NombreDeVariables;
    if (nvars > var)
    {
        auto fullName = BuildName(
          Antares::Data::Enum::toString(structDict),
          LocationIdentifier(area, Antares::Data::Enum::ExportStructLocationDict::area) + SEPARATOR
            + "Layer<" + std::to_string(layerIndex) + ">",
          TimeIdentifier(timeStep, Antares::Data::Enum::ExportStructTimeStepDict::hour));
        problem->NomDesVariables[var] = fullName;
    }
}
void VariableNamer::SetThermalClusterVariableName(int var,
                                                  Antares::Data::Enum::ExportStructDict structDict,
                                                  const std::string& clusterName)
{
    auto nvars = problem->NombreDeVariables;

    if (nvars > var)
    {
        const auto location
          = LocationIdentifier(area, Antares::Data::Enum::ExportStructLocationDict::area)
            + SEPARATOR
            + Antares::Data::Enum::toString(Antares::Data::Enum::ExportStructDict::PalierThermique)
            + "<" + clusterName + ">";

        auto fullName = BuildName(
          Antares::Data::Enum::toString(structDict),
          location,
          TimeIdentifier(timeStep, Antares::Data::Enum::ExportStructTimeStepDict::hour));
        problem->NomDesVariables[var] = fullName;
    }
}
void VariableNamer::DispatchableProduction(int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(
      var, Antares::Data::Enum::ExportStructDict::DispatchableProduction, clusterName);
}
void VariableNamer::NODU(int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(
      var,
      Antares::Data::Enum::ExportStructDict::NombreDeGroupesEnMarcheDuPalierThermique,
      clusterName);
}
void VariableNamer::NumberStoppingDispatchableUnits(int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(
      var,
      Antares::Data::Enum::ExportStructDict::NombreDeGroupesQuiDemarrentDuPalierThermique,
      clusterName);
}
void VariableNamer::NumberStartingDispatchableUnits(int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(
      var,
      Antares::Data::Enum::ExportStructDict::NombreDeGroupesQuiDemarrentDuPalierThermique,
      clusterName);
}
void VariableNamer::NumberBreakingDownDispatchableUnits(int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(
      var,
      Antares::Data::Enum::ExportStructDict::NombreDeGroupesQuiTombentEnPanneDuPalierThermique,
      clusterName);
}
void VariableNamer::NTCValueOriginToDestination(int var)
{
    SetLinkVariableName(var,
                        Antares::Data::Enum::ExportStructDict::ValeurDeNTCOrigineVersExtremite);
}
void VariableNamer::IntercoCostOriginToDestination(int var)
{
    SetLinkVariableName(
      var, Antares::Data::Enum::ExportStructDict::CoutOrigineVersExtremiteDeLInterconnexion);
}
void VariableNamer::IntercoCostDestinationToOrigin(int var)
{
    SetLinkVariableName(
      var, Antares::Data::Enum::ExportStructDict::CoutExtremiteVersOrigineDeLInterconnexion);
}

void VariableNamer::SetShortTermStorageVariableName(
  int var,
  Antares::Data::Enum::ExportStructDict structDict,
  const std::string& shortTermStorageName)
{
    auto nvars = problem->NombreDeVariables;

    if (nvars > var)
    {
        const auto location
          = LocationIdentifier(area, Antares::Data::Enum::ExportStructLocationDict::area)
            + SEPARATOR
            + Antares::Data::Enum::toString(Antares::Data::Enum::ExportStructDict::ShortTermStorage)
            + "<" + shortTermStorageName + ">";
        auto fullName = BuildName(
          Antares::Data::Enum::toString(structDict),
          location,
          TimeIdentifier(timeStep, Antares::Data::Enum::ExportStructTimeStepDict::hour));
        problem->NomDesVariables[var] = fullName;
    }
}
void VariableNamer::ShortTermStorageInjection(int var, const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(
      var, Antares::Data::Enum::ExportStructDict::ShortTermStorageInjection, shortTermStorageName);
}
void VariableNamer::ShortTermStorageWithdrawal(int var, const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(
      var, Antares::Data::Enum::ExportStructDict::ShortTermStorageWithdrawal, shortTermStorageName);
}
void VariableNamer::ShortTermStorageLevel(int var, const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(
      var, Antares::Data::Enum::ExportStructDict::ShortTermStorageLevel, shortTermStorageName);
}
void VariableNamer::HydProd(int var)
{
    SetAreaVariableName(var, Antares::Data::Enum::ExportStructDict::ProdHyd);
}
void VariableNamer::HydProdDown(int var)
{
    SetAreaVariableName(var, Antares::Data::Enum::ExportStructDict::ProdHydALaBaisse);
}
void VariableNamer::HydProdUp(int var)
{
    SetAreaVariableName(var, Antares::Data::Enum::ExportStructDict::ProdHydALaHausse);
}
void VariableNamer::Pumping(int var)
{
    SetAreaVariableName(var, Antares::Data::Enum::ExportStructDict::Pompage);
}
void VariableNamer::HydroLevel(int var)
{
    SetAreaVariableName(var, Antares::Data::Enum::ExportStructDict::NiveauHydro);
}
void VariableNamer::Overflow(int var)
{
    SetAreaVariableName(var, Antares::Data::Enum::ExportStructDict::Debordement);
}
void VariableNamer::LayerStorage(int var, int layerIndex)
{
    SetAreaVariableName(var, Antares::Data::Enum::ExportStructDict::TrancheDeStock, layerIndex);
}
void VariableNamer::FinalStorage(int var)
{
    SetAreaVariableName(var, Antares::Data::Enum::ExportStructDict::StockFinal);
}
void VariableNamer::PositiveUnsuppliedEnergy(int var)
{
    SetAreaVariableName(var, Antares::Data::Enum::ExportStructDict::DefaillancePositive);
}
void VariableNamer::NegativeUnsuppliedEnergy(int var)
{
    SetAreaVariableName(var, Antares::Data::Enum::ExportStructDict::DefaillanceNegative);
}
void VariableNamer::AreaBalance(int var)
{
    SetAreaVariableName(var, Antares::Data::Enum::ExportStructDict::BilansPays);
}

std::string BuildName(const std::string& name,
                      const std::string& location,
                      const std::string& timeIdentifier)
{
    std::string result = name + SEPARATOR + location + SEPARATOR + timeIdentifier;
    std::replace(result.begin(), result.end(), ' ', '*');
    return result;
}
