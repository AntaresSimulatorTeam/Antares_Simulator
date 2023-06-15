#include "opt_rename_problem.h"
#include <sstream>

using namespace Antares::Data::Enum;
void VariableNamer::SetLinkVariableName(int var, ExportStructDict structDict)
{
    if (problem_->NomDesVariables[var].empty())
    {
        const auto location = origin_ + AREA_SEP + destination_;
        problem_->NomDesVariables[var]
          = BuildName(toString(structDict),
                      LocationIdentifier(location, ExportStructLocationDict::link),
                      TimeIdentifier(timeStep_, ExportStructTimeStepDict::hour));
    }
}

void VariableNamer::SetAreaVariableName(int var, ExportStructDict structDict)
{
    problem_->NomDesVariables[var]
      = BuildName(toString(structDict),
                  LocationIdentifier(area_, ExportStructLocationDict::area),
                  TimeIdentifier(timeStep_, ExportStructTimeStepDict::hour));
}

void VariableNamer::SetAreaVariableName(int var, ExportStructDict structDict, int layerIndex)
{
    problem_->NomDesVariables[var]
      = BuildName(toString(structDict),
                  LocationIdentifier(area_, ExportStructLocationDict::area) + SEPARATOR + "Layer<"
                    + std::to_string(layerIndex) + ">",
                  TimeIdentifier(timeStep_, ExportStructTimeStepDict::hour));
}

void VariableNamer::SetThermalClusterVariableName(int var,
                                                  ExportStructDict structDict,
                                                  const std::string& clusterName)
{
    const auto location = LocationIdentifier(area_, ExportStructLocationDict::area) + SEPARATOR
                          + toString(ExportStructDict::PalierThermique) + "<" + clusterName + ">";

    problem_->NomDesVariables[var] = BuildName(
      toString(structDict), location, TimeIdentifier(timeStep_, ExportStructTimeStepDict::hour));
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

void VariableNamer::NTCDirect(int var, const std::string& origin, const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
    SetLinkVariableName(var, ExportStructDict::ValeurDeNTCOrigineVersExtremite);
}

void VariableNamer::IntercoDirectCost(int var,
                                      const std::string& origin,
                                      const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
    SetLinkVariableName(var, ExportStructDict::CoutOrigineVersExtremiteDeLInterconnexion);
}

void VariableNamer::IntercoIndirectCost(int var,
                                        const std::string& origin,
                                        const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
    SetLinkVariableName(var, ExportStructDict::CoutExtremiteVersOrigineDeLInterconnexion);
}

void VariableNamer::SetShortTermStorageVariableName(int var,
                                                    ExportStructDict structDict,
                                                    const std::string& shortTermStorageName)
{
    const auto location = LocationIdentifier(area_, ExportStructLocationDict::area) + SEPARATOR
                          + toString(ExportStructDict::ShortTermStorage) + "<"
                          + shortTermStorageName + ">";
    auto fullName = BuildName(
      toString(structDict), location, TimeIdentifier(timeStep_, ExportStructTimeStepDict::hour));
    problem_->NomDesVariables[var] = fullName;
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

void ConstraintNamer::FlowDissociation(const std::string& origin, const std::string& destination)
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1] = BuildName(
      toString(ExportStructConstraintsDict::FlowDissociation),
      LocationIdentifier(origin + AREA_SEP + destination, ExportStructLocationDict::link),
      TimeIdentifier(timeStep_, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::AreaBalance()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::AreaBalance),
                  LocationIdentifier(area_, ExportStructLocationDict::area),
                  TimeIdentifier(timeStep_, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::FictiveLoads()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::FictiveLoads),
                  LocationIdentifier(area_, ExportStructLocationDict::area),
                  TimeIdentifier(timeStep_, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::HydroPower()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::HydroPower),
                  LocationIdentifier(area_, ExportStructLocationDict::area),
                  TimeIdentifier(timeStep_, ExportStructTimeStepDict::week));
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationSum()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::HydroPowerSmoothingUsingVariationSum),
                  LocationIdentifier(area_, ExportStructLocationDict::area),
                  TimeIdentifier(timeStep_, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxDown()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::HydroPowerSmoothingUsingVariationMaxDown),
                  LocationIdentifier(area_, ExportStructLocationDict::area),
                  TimeIdentifier(timeStep_, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxUp()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::HydroPowerSmoothingUsingVariationMaxUp),
                  LocationIdentifier(area_, ExportStructLocationDict::area),
                  TimeIdentifier(timeStep_, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::MinHydroPower()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::MinHydroPower),
                  LocationIdentifier(area_, ExportStructLocationDict::area),
                  TimeIdentifier(timeStep_, ExportStructTimeStepDict::week));
}

void ConstraintNamer::MaxHydroPower()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::MaxHydroPower),
                  LocationIdentifier(area_, ExportStructLocationDict::area),
                  TimeIdentifier(timeStep_, ExportStructTimeStepDict::week));
}

void ConstraintNamer::MaxPumping()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::MaxPumping),
                  LocationIdentifier(area_, ExportStructLocationDict::area),
                  TimeIdentifier(timeStep_, ExportStructTimeStepDict::week));
}

void ConstraintNamer::AreaHydroLevel()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::AreaHydroLevel),
                  LocationIdentifier(area_, ExportStructLocationDict::area),
                  TimeIdentifier(timeStep_, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::FinalStockEquivalent()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::FinalStockEquivalent),
                  LocationIdentifier(area_, ExportStructLocationDict::area),
                  TimeIdentifier(timeStep_, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::FinalStockExpression()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::FinalStockExpression),
                  LocationIdentifier(area_, ExportStructLocationDict::area),
                  TimeIdentifier(timeStep_, ExportStructTimeStepDict::hour));
}

void IConstraintNamer::nameWithTimeGranularity(const std::string& name,
                                               ExportStructTimeStepDict type)
{
    /*TODO convert to map*/
    auto timeStepType
      = (type == ExportStructTimeStepDict::hour)  ? ExportStructBindingConstraintType::hourly
        : (type == ExportStructTimeStepDict::day) ? ExportStructBindingConstraintType::daily
                                                  : ExportStructBindingConstraintType::weekly;
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(name, toString(timeStepType), TimeIdentifier(timeStep_, type));
}

void ConstraintNamer::MinUpTime()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::MinUpTime),
                  LocationIdentifier(area_, ExportStructLocationDict::area),
                  TimeIdentifier(timeStep_, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::MinDownTime()
{
    std::string constraintFullName
      = BuildName(toString(ExportStructConstraintsDict::MinDownTime),
                  LocationIdentifier(area_, ExportStructLocationDict::area),
                  TimeIdentifier(timeStep_, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::PMaxDispatchableGeneration()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::PMaxDispatchableGeneration),
                  LocationIdentifier(area_, ExportStructLocationDict::area),
                  TimeIdentifier(timeStep_, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::PMinDispatchableGeneration()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::PMinDispatchableGeneration),
                  LocationIdentifier(area_, ExportStructLocationDict::area),
                  TimeIdentifier(timeStep_, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::ConsistenceNODU()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::ConsistenceNODU),
                  LocationIdentifier(area_, ExportStructLocationDict::area),
                  TimeIdentifier(timeStep_, ExportStructTimeStepDict::hour));
}

void ConstraintNamer::ShortTermStorageLevel()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ExportStructConstraintsDict::ShortTermStorageLevel),
                  LocationIdentifier(area_, ExportStructLocationDict::area),
                  TimeIdentifier(timeStep_, ExportStructTimeStepDict::hour));
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