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

std::string BuildName(const std::string& name,
                      const std::string& location,
                      const std::string& timeIdentifier)
{
    std::string result = name + SEPARATOR + location + SEPARATOR + timeIdentifier;
    std::replace(result.begin(), result.end(), ' ', '*');
    return result;
}
