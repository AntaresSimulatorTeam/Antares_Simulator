#include "opt_rename_problem.h"
#include <sstream>
using namespace Antares::Data;
void VariableNamer::SetLinkVariableName(int var, Antares::Data::Enum::ExportStructDict structDict)
{
    auto nvars = problemElementInfo.problem->NombreDeVariables;
    if (nvars > var && problemElementInfo.problem->NomDesVariables[var].empty())
    {
        const auto location
          = problemElementInfo.origin + ZONE_SEPARATOR + problemElementInfo.destination;
        auto fullName = BuildName(
          Antares::Data::Enum::toString(structDict),
          LocationIdentifier(location, Antares::Data::Enum::ExportStructLocationDict::link),
          TimeIdentifier(problemElementInfo.timeStep,
                         Antares::Data::Enum::ExportStructTimeStepDict::hour));
        problemElementInfo.problem->NomDesVariables[var] = fullName;
    }
}

void VariableNamer::SetAreaVariableName(int var, Antares::Data::Enum::ExportStructDict structDict)
{
    auto nvars = problemElementInfo.problem->NombreDeVariables;
    if (nvars > var)
    {
        auto fullName
          = BuildName(Antares::Data::Enum::toString(structDict),
                      LocationIdentifier(problemElementInfo.area,
                                         Antares::Data::Enum::ExportStructLocationDict::area),
                      TimeIdentifier(problemElementInfo.timeStep,
                                     Antares::Data::Enum::ExportStructTimeStepDict::hour));
        problemElementInfo.problem->NomDesVariables[var] = fullName;
    }
}
void VariableNamer::SetAreaVariableName(int var,
                                        Antares::Data::Enum::ExportStructDict structDict,
                                        int layerIndex)
{
    auto nvars = problemElementInfo.problem->NombreDeVariables;
    if (nvars > var)
    {
        auto fullName
          = BuildName(Antares::Data::Enum::toString(structDict),
                      LocationIdentifier(problemElementInfo.area,
                                         Antares::Data::Enum::ExportStructLocationDict::area)
                        + SEPARATOR + "Layer<" + std::to_string(layerIndex) + ">",
                      TimeIdentifier(problemElementInfo.timeStep,
                                     Antares::Data::Enum::ExportStructTimeStepDict::hour));
        problemElementInfo.problem->NomDesVariables[var] = fullName;
    }
}
void VariableNamer::SetThermalClusterVariableName(int var,
                                                  Antares::Data::Enum::ExportStructDict structDict,
                                                  const std::string& clusterName)
{
    auto nvars = problemElementInfo.problem->NombreDeVariables;

    if (nvars > var)
    {
        const auto location
          = LocationIdentifier(problemElementInfo.area,
                               Antares::Data::Enum::ExportStructLocationDict::area)
            + SEPARATOR
            + Antares::Data::Enum::toString(Antares::Data::Enum::ExportStructDict::PalierThermique)
            + "<" + clusterName + ">";

        auto fullName
          = BuildName(Antares::Data::Enum::toString(structDict),
                      location,
                      TimeIdentifier(problemElementInfo.timeStep,
                                     Antares::Data::Enum::ExportStructTimeStepDict::hour));
        problemElementInfo.problem->NomDesVariables[var] = fullName;
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
    auto nvars = problemElementInfo.problem->NombreDeVariables;

    if (nvars > var)
    {
        const auto location
          = LocationIdentifier(problemElementInfo.area,
                               Antares::Data::Enum::ExportStructLocationDict::area)
            + SEPARATOR
            + Antares::Data::Enum::toString(Antares::Data::Enum::ExportStructDict::ShortTermStorage)
            + "<" + shortTermStorageName + ">";
        auto fullName
          = BuildName(Antares::Data::Enum::toString(structDict),
                      location,
                      TimeIdentifier(problemElementInfo.timeStep,
                                     Antares::Data::Enum::ExportStructTimeStepDict::hour));
        problemElementInfo.problem->NomDesVariables[var] = fullName;
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

void ConstraintNamer::FlowDissociation(int constraint)
{
    problemElementInfo.problem->NomDesContraintes[constraint] = BuildName(
      Antares::Data::Enum::toString(Enum::ExportStructConstraintsDict::FlowDissociation),
      LocationIdentifier(
        problemElementInfo.origin + ZONE_SEPARATOR + problemElementInfo.destination,
        Enum::ExportStructLocationDict::link),
      TimeIdentifier(problemElementInfo.timeStep, Enum::ExportStructTimeStepDict::hour));
}
void ConstraintNamer::AreaBalance(int constraint)
{
    problemElementInfo.problem->NomDesContraintes[constraint] = BuildName(
      Enum::toString(Enum::ExportStructConstraintsDict::AreaBalance),
      LocationIdentifier(problemElementInfo.area, Enum::ExportStructLocationDict::area),
      TimeIdentifier(problemElementInfo.timeStep, Enum::ExportStructTimeStepDict::hour));
}
void ConstraintNamer::FictiveLoads(int constraint)
{
    problemElementInfo.problem->NomDesContraintes[constraint] = BuildName(
      Enum::toString(Enum::ExportStructConstraintsDict::FictiveLoads),
      LocationIdentifier(problemElementInfo.area, Enum::ExportStructLocationDict::area),
      TimeIdentifier(problemElementInfo.timeStep, Enum::ExportStructTimeStepDict::hour));
}
void ConstraintNamer::HydroPower(int constraint)
{
    problemElementInfo.problem->NomDesContraintes[constraint] = BuildName(
      Enum::toString(Enum::ExportStructConstraintsDict::HydroPower),
      LocationIdentifier(problemElementInfo.area, Enum::ExportStructLocationDict::area),
      TimeIdentifier(problemElementInfo.timeStep, Enum::ExportStructTimeStepDict::week));
}
void ConstraintNamer::HydroPowerSmoothingUsingVariationSum(int constraint)
{
    problemElementInfo.problem->NomDesContraintes[constraint] = BuildName(
      Enum::toString(Enum::ExportStructConstraintsDict::HydroPowerSmoothingUsingVariationSum),
      LocationIdentifier(problemElementInfo.area, Enum::ExportStructLocationDict::area),
      TimeIdentifier(problemElementInfo.timeStep, Enum::ExportStructTimeStepDict::hour));
}
void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxDown(int constraint)
{
    problemElementInfo.problem->NomDesContraintes[constraint] = BuildName(
      Enum::toString(Enum::ExportStructConstraintsDict::HydroPowerSmoothingUsingVariationMaxDown),
      LocationIdentifier(problemElementInfo.area, Enum::ExportStructLocationDict::area),
      TimeIdentifier(problemElementInfo.timeStep, Enum::ExportStructTimeStepDict::hour));
}
void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxUp(int constraint)
{
    problemElementInfo.problem->NomDesContraintes[constraint] = BuildName(
      Enum::toString(Enum::ExportStructConstraintsDict::HydroPowerSmoothingUsingVariationMaxUp),
      LocationIdentifier(problemElementInfo.area, Enum::ExportStructLocationDict::area),
      TimeIdentifier(problemElementInfo.timeStep, Enum::ExportStructTimeStepDict::hour));
}
void ConstraintNamer::MinHydroPower(int constraint)
{
    problemElementInfo.problem->NomDesContraintes[constraint] = BuildName(
      Enum::toString(Enum::ExportStructConstraintsDict::MinHydroPower),
      LocationIdentifier(problemElementInfo.area, Enum::ExportStructLocationDict::area),
      TimeIdentifier(problemElementInfo.timeStep, Enum::ExportStructTimeStepDict::week));
}
void ConstraintNamer::MaxHydroPower(int constraint)
{
    problemElementInfo.problem->NomDesContraintes[constraint] = BuildName(
      Enum::toString(Enum::ExportStructConstraintsDict::MaxHydroPower),
      LocationIdentifier(problemElementInfo.area, Enum::ExportStructLocationDict::area),
      TimeIdentifier(problemElementInfo.timeStep, Enum::ExportStructTimeStepDict::week));
}
void ConstraintNamer::MaxPumping(int constraint)
{
    problemElementInfo.problem->NomDesContraintes[constraint] = BuildName(
      Enum::toString(Enum::ExportStructConstraintsDict::MaxPumping),
      LocationIdentifier(problemElementInfo.area, Enum::ExportStructLocationDict::area),
      TimeIdentifier(problemElementInfo.timeStep, Enum::ExportStructTimeStepDict::week));
}
void ConstraintNamer::AreaHydroLevel(int constraint)
{
    problemElementInfo.problem->NomDesContraintes[constraint] = BuildName(
      Enum::toString(Enum::ExportStructConstraintsDict::AreaHydroLevel),
      LocationIdentifier(problemElementInfo.area, Enum::ExportStructLocationDict::area),
      TimeIdentifier(problemElementInfo.timeStep, Enum::ExportStructTimeStepDict::hour));
}
void ConstraintNamer::FinalStockEquivalent(int constraint)
{
    problemElementInfo.problem->NomDesContraintes[constraint] = BuildName(
      Enum::toString(Enum::ExportStructConstraintsDict::FinalStockEquivalent),
      LocationIdentifier(problemElementInfo.area, Enum::ExportStructLocationDict::area),
      TimeIdentifier(problemElementInfo.timeStep, Enum::ExportStructTimeStepDict::hour));
}
void ConstraintNamer::FinalStockExpression(int constraint)
{
    problemElementInfo.problem->NomDesContraintes[constraint] = BuildName(
      Enum::toString(Enum::ExportStructConstraintsDict::FinalStockExpression),
      LocationIdentifier(problemElementInfo.area, Enum::ExportStructLocationDict::area),
      TimeIdentifier(problemElementInfo.timeStep, Enum::ExportStructTimeStepDict::hour));
}
void ConstraintNamer::BindingConstraint(int constraint,
                                        const std::string& name,
                                        Enum::ExportStructTimeStepDict type)
{
    auto timeStepType = (type == Enum::ExportStructTimeStepDict::hour)
                          ? Enum::ExportStructBindingConstraintType::hourly
                        : (type == Enum::ExportStructTimeStepDict::day)
                          ? Enum::ExportStructBindingConstraintType::daily
                          : Enum::ExportStructBindingConstraintType::weekly;
    problemElementInfo.problem->NomDesContraintes[constraint] = BuildName(
      name, Enum::toString(timeStepType), TimeIdentifier(problemElementInfo.timeStep, type));
}
void ConstraintNamer::MinUpTime(int constraint)
{
    problemElementInfo.problem->NomDesContraintes[constraint] = BuildName(
      Enum::toString(Enum::ExportStructConstraintsDict::MinUpTime),
      LocationIdentifier(problemElementInfo.area, Enum::ExportStructLocationDict::area),
      TimeIdentifier(problemElementInfo.timeStep, Enum::ExportStructTimeStepDict::hour));
}
void ConstraintNamer::MinDownTime(int constraint)
{
    std::string constraintFullName = BuildName(
      Enum::toString(Enum::ExportStructConstraintsDict::MinDownTime),
      LocationIdentifier(problemElementInfo.area, Enum::ExportStructLocationDict::area),
      TimeIdentifier(problemElementInfo.timeStep, Enum::ExportStructTimeStepDict::hour));
}
void ConstraintNamer::PMaxDispatchableGeneration(int constraint)
{
    problemElementInfo.problem->NomDesContraintes[constraint] = BuildName(
      Enum::toString(Enum::ExportStructConstraintsDict::PMaxDispatchableGeneration),
      LocationIdentifier(problemElementInfo.area, Enum::ExportStructLocationDict::area),
      TimeIdentifier(problemElementInfo.timeStep, Enum::ExportStructTimeStepDict::hour));
}
void ConstraintNamer::PMinDispatchableGeneration(int constraint)
{
    problemElementInfo.problem->NomDesContraintes[constraint] = BuildName(
      Enum::toString(Enum::ExportStructConstraintsDict::PMinDispatchableGeneration),
      LocationIdentifier(problemElementInfo.area, Enum::ExportStructLocationDict::area),
      TimeIdentifier(problemElementInfo.timeStep, Enum::ExportStructTimeStepDict::hour));
}
void ConstraintNamer::ConsistenceNODU(int constraint)
{
    problemElementInfo.problem->NomDesContraintes[constraint] = BuildName(
      Enum::toString(Enum::ExportStructConstraintsDict::ConsistenceNODU),
      LocationIdentifier(problemElementInfo.area, Enum::ExportStructLocationDict::area),
      TimeIdentifier(problemElementInfo.timeStep, Enum::ExportStructTimeStepDict::hour));
}
void ConstraintNamer::ShortTermStorageLevel(int constraint)
{
    problemElementInfo.problem->NomDesContraintes[constraint]
      = BuildName(Antares::Data::Enum::toString(
                    Antares::Data::Enum::ExportStructConstraintsDict::ShortTermStorageLevel),
                  LocationIdentifier(problemElementInfo.area,
                                     Antares::Data::Enum::ExportStructLocationDict::area),
                  TimeIdentifier(problemElementInfo.timeStep,
                                 Antares::Data::Enum::ExportStructTimeStepDict::hour));
}
std::string BuildName(const std::string& name,
                      const std::string& location,
                      const std::string& timeIdentifier)
{
    std::string result = name + SEPARATOR + location + SEPARATOR + timeIdentifier;
    std::replace(result.begin(), result.end(), ' ', '*');
    return result;
}
