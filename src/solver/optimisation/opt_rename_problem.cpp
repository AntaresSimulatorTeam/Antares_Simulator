#include "opt_rename_problem.h"
#include <sstream>

using namespace Antares::Data::Enum;
void IVariableNamer::SetLinkVariableName(int var, VariableType structDict)
{
    if (problem_->NomDesVariables[var].empty())
    {
        const auto location = origin_ + AREA_SEP + destination_;
        problem_->NomDesVariables[var] = BuildName(toString(structDict),
                                                   LocationIdentifier(location, LocationType::link),
                                                   TimeIdentifier(timeStep_, TimeStepType::hour));
    }
}

void IVariableNamer::SetAreaVariableName(int var, VariableType structDict)
{
    problem_->NomDesVariables[var] = BuildName(toString(structDict),
                                               LocationIdentifier(area_, LocationType::area),
                                               TimeIdentifier(timeStep_, TimeStepType::hour));
}

void IVariableNamer::SetAreaVariableName(int var, VariableType structDict, int layerIndex)
{
    problem_->NomDesVariables[var]
      = BuildName(toString(structDict),
                  LocationIdentifier(area_, LocationType::area) + SEPARATOR + "Layer<"
                    + std::to_string(layerIndex) + ">",
                  TimeIdentifier(timeStep_, TimeStepType::hour));
}

void IVariableNamer::SetThermalClusterVariableName(int var,
                                                   VariableType structDict,
                                                   const std::string& clusterName)
{
    const auto location = LocationIdentifier(area_, LocationType::area) + SEPARATOR
                          + toString(VariableType::PalierThermique) + "<" + clusterName + ">";

    problem_->NomDesVariables[var]
      = BuildName(toString(structDict), location, TimeIdentifier(timeStep_, TimeStepType::hour));
}

void VariableNamer::DispatchableProduction(int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(var, VariableType::DispatchableProduction, clusterName);
}

void VariableNamer::NODU(int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(
      var, VariableType::NombreDeGroupesEnMarcheDuPalierThermique, clusterName);
}

void VariableNamer::NumberStoppingDispatchableUnits(int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(
      var, VariableType::NombreDeGroupesQuiDemarrentDuPalierThermique, clusterName);
}

void VariableNamer::NumberStartingDispatchableUnits(int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(
      var, VariableType::NombreDeGroupesQuiDemarrentDuPalierThermique, clusterName);
}

void VariableNamer::NumberBreakingDownDispatchableUnits(int var, const std::string& clusterName)
{
    SetThermalClusterVariableName(
      var, VariableType::NombreDeGroupesQuiTombentEnPanneDuPalierThermique, clusterName);
}

void VariableNamer::NTCDirect(int var, const std::string& origin, const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
    SetLinkVariableName(var, VariableType::ValeurDeNTCOrigineVersExtremite);
}

void VariableNamer::IntercoDirectCost(int var,
                                      const std::string& origin,
                                      const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
    SetLinkVariableName(var, VariableType::CoutOrigineVersExtremiteDeLInterconnexion);
}

void VariableNamer::IntercoIndirectCost(int var,
                                        const std::string& origin,
                                        const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
    SetLinkVariableName(var, VariableType::CoutExtremiteVersOrigineDeLInterconnexion);
}

void IVariableNamer::SetShortTermStorageVariableName(int var,
                                                     VariableType structDict,
                                                     const std::string& shortTermStorageName)
{
    const auto location = LocationIdentifier(area_, LocationType::area) + SEPARATOR
                          + toString(VariableType::ShortTermStorage) + "<" + shortTermStorageName
                          + ">";
    auto fullName
      = BuildName(toString(structDict), location, TimeIdentifier(timeStep_, TimeStepType::hour));
    problem_->NomDesVariables[var] = fullName;
}

void VariableNamer::ShortTermStorageInjection(int var, const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(
      var, VariableType::ShortTermStorageInjection, shortTermStorageName);
}

void VariableNamer::ShortTermStorageWithdrawal(int var, const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(
      var, VariableType::ShortTermStorageWithdrawal, shortTermStorageName);
}

void VariableNamer::ShortTermStorageLevel(int var, const std::string& shortTermStorageName)
{
    SetShortTermStorageVariableName(var, VariableType::ShortTermStorageLevel, shortTermStorageName);
}

void VariableNamer::HydProd(int var)
{
    SetAreaVariableName(var, VariableType::ProdHyd);
}

void VariableNamer::HydProdDown(int var)
{
    SetAreaVariableName(var, VariableType::ProdHydALaBaisse);
}

void VariableNamer::HydProdUp(int var)
{
    SetAreaVariableName(var, VariableType::ProdHydALaHausse);
}

void VariableNamer::Pumping(int var)
{
    SetAreaVariableName(var, VariableType::Pompage);
}

void VariableNamer::HydroLevel(int var)
{
    SetAreaVariableName(var, VariableType::NiveauHydro);
}

void VariableNamer::Overflow(int var)
{
    SetAreaVariableName(var, VariableType::Debordement);
}

void VariableNamer::LayerStorage(int var, int layerIndex)
{
    SetAreaVariableName(var, VariableType::TrancheDeStock, layerIndex);
}

void VariableNamer::FinalStorage(int var)
{
    SetAreaVariableName(var, VariableType::StockFinal);
}

void VariableNamer::PositiveUnsuppliedEnergy(int var)
{
    SetAreaVariableName(var, VariableType::DefaillancePositive);
}

void VariableNamer::NegativeUnsuppliedEnergy(int var)
{
    SetAreaVariableName(var, VariableType::DefaillanceNegative);
}

void VariableNamer::AreaBalance(int var)
{
    SetAreaVariableName(var, VariableType::BilansPays);
}

void ConstraintNamer::FlowDissociation(const std::string& origin, const std::string& destination)
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ConstraintType::FlowDissociation),
                  LocationIdentifier(origin + AREA_SEP + destination, LocationType::link),
                  TimeIdentifier(timeStep_, TimeStepType::hour));
}

void ConstraintNamer::AreaBalance()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ConstraintType::AreaBalance),
                  LocationIdentifier(area_, LocationType::area),
                  TimeIdentifier(timeStep_, TimeStepType::hour));
}

void ConstraintNamer::FictiveLoads()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ConstraintType::FictiveLoads),
                  LocationIdentifier(area_, LocationType::area),
                  TimeIdentifier(timeStep_, TimeStepType::hour));
}

void ConstraintNamer::HydroPower()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ConstraintType::HydroPower),
                  LocationIdentifier(area_, LocationType::area),
                  TimeIdentifier(timeStep_, TimeStepType::week));
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationSum()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ConstraintType::HydroPowerSmoothingUsingVariationSum),
                  LocationIdentifier(area_, LocationType::area),
                  TimeIdentifier(timeStep_, TimeStepType::hour));
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxDown()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ConstraintType::HydroPowerSmoothingUsingVariationMaxDown),
                  LocationIdentifier(area_, LocationType::area),
                  TimeIdentifier(timeStep_, TimeStepType::hour));
}

void ConstraintNamer::HydroPowerSmoothingUsingVariationMaxUp()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ConstraintType::HydroPowerSmoothingUsingVariationMaxUp),
                  LocationIdentifier(area_, LocationType::area),
                  TimeIdentifier(timeStep_, TimeStepType::hour));
}

void ConstraintNamer::MinHydroPower()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ConstraintType::MinHydroPower),
                  LocationIdentifier(area_, LocationType::area),
                  TimeIdentifier(timeStep_, TimeStepType::week));
}

void ConstraintNamer::MaxHydroPower()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ConstraintType::MaxHydroPower),
                  LocationIdentifier(area_, LocationType::area),
                  TimeIdentifier(timeStep_, TimeStepType::week));
}

void ConstraintNamer::MaxPumping()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ConstraintType::MaxPumping),
                  LocationIdentifier(area_, LocationType::area),
                  TimeIdentifier(timeStep_, TimeStepType::week));
}

void ConstraintNamer::AreaHydroLevel()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ConstraintType::AreaHydroLevel),
                  LocationIdentifier(area_, LocationType::area),
                  TimeIdentifier(timeStep_, TimeStepType::hour));
}

void ConstraintNamer::FinalStockEquivalent()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ConstraintType::FinalStockEquivalent),
                  LocationIdentifier(area_, LocationType::area),
                  TimeIdentifier(timeStep_, TimeStepType::hour));
}

void ConstraintNamer::FinalStockExpression()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ConstraintType::FinalStockExpression),
                  LocationIdentifier(area_, LocationType::area),
                  TimeIdentifier(timeStep_, TimeStepType::hour));
}

void IConstraintNamer::nameWithTimeGranularity(const std::string& name, TimeStepType type)
{
    /*TODO convert to map*/
    auto bcType = (type == TimeStepType::hour)  ? BindingConstraintType::hourly
                  : (type == TimeStepType::day) ? BindingConstraintType::daily
                                                : BindingConstraintType::weekly;
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(name, toString(bcType), TimeIdentifier(timeStep_, type));
}

void ConstraintNamer::MinUpTime()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ConstraintType::MinUpTime),
                  LocationIdentifier(area_, LocationType::area),
                  TimeIdentifier(timeStep_, TimeStepType::hour));
}

void ConstraintNamer::MinDownTime()
{
    std::string constraintFullName = BuildName(toString(ConstraintType::MinDownTime),
                                               LocationIdentifier(area_, LocationType::area),
                                               TimeIdentifier(timeStep_, TimeStepType::hour));
}

void ConstraintNamer::PMaxDispatchableGeneration()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ConstraintType::PMaxDispatchableGeneration),
                  LocationIdentifier(area_, LocationType::area),
                  TimeIdentifier(timeStep_, TimeStepType::hour));
}

void ConstraintNamer::PMinDispatchableGeneration()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ConstraintType::PMinDispatchableGeneration),
                  LocationIdentifier(area_, LocationType::area),
                  TimeIdentifier(timeStep_, TimeStepType::hour));
}

void ConstraintNamer::ConsistenceNODU()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ConstraintType::ConsistenceNODU),
                  LocationIdentifier(area_, LocationType::area),
                  TimeIdentifier(timeStep_, TimeStepType::hour));
}

void ConstraintNamer::ShortTermStorageLevel()
{
    problem_->NomDesContraintes[problem_->NombreDeContraintes - 1]
      = BuildName(toString(ConstraintType::ShortTermStorageLevel),
                  LocationIdentifier(area_, LocationType::area),
                  TimeIdentifier(timeStep_, TimeStepType::hour));
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