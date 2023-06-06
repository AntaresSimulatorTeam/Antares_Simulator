#include "opt_rename_problem.h"
#include <sstream>
void RenameLinkVariable(PROBLEME_ANTARES_A_RESOUDRE* problem,
                        int var,
                        Antares::Data::Enum::ExportStructDict structDict,
                        int ts,
                        Antares::Data::Enum::ExportStructTimeStepDict timeStepType,
                        const std::string& origin,
                        const std::string& destination)
{
    auto nvars = problem->NombreDeVariables;
    if (nvars > var && problem->NomDesVariables[var].empty())
    {
        const auto location = origin + ZONE_SEPARATOR + destination;
        auto fullName = BuildName(
          Antares::Data::Enum::toString(structDict),
          LocationIdentifier(location, Antares::Data::Enum::ExportStructLocationDict::link),
          TimeIdentifier(ts, timeStepType));
        problem->NomDesVariables[var] = fullName;
    }
}

void RenameZoneVariable(PROBLEME_ANTARES_A_RESOUDRE* problem,
                        int var,
                        Antares::Data::Enum::ExportStructDict structDict,
                        int ts,
                        Antares::Data::Enum::ExportStructTimeStepDict timeStepType,
                        const std::string& zone)
{
    auto nvars = problem->NombreDeVariables;
    if (nvars > var)
    {
        auto fullName
          = BuildName(Antares::Data::Enum::toString(structDict),
                      LocationIdentifier(zone, Antares::Data::Enum::ExportStructLocationDict::area),
                      TimeIdentifier(ts, timeStepType));
        problem->NomDesVariables[var] = fullName;
    }
}
void RenameZoneVariable(PROBLEME_ANTARES_A_RESOUDRE* problem,
                        int var,
                        Antares::Data::Enum::ExportStructDict structDict,
                        int ts,
                        Antares::Data::Enum::ExportStructTimeStepDict timeStepType,
                        const std::string& zone,
                        int layerIndex)
{
    auto nvars = problem->NombreDeVariables;
    if (nvars > var)
    {
        auto fullName
          = BuildName(Antares::Data::Enum::toString(structDict),
                      LocationIdentifier(zone, Antares::Data::Enum::ExportStructLocationDict::area)
                        + SEPARATOR + "Layer<" + std::to_string(layerIndex) + ">",
                      TimeIdentifier(ts, timeStepType));
        problem->NomDesVariables[var] = fullName;
    }
}
void RenameThermalClusterVariable(PROBLEME_ANTARES_A_RESOUDRE* problem,
                                  int var,
                                  Antares::Data::Enum::ExportStructDict structDict,
                                  int ts,
                                  Antares::Data::Enum::ExportStructTimeStepDict timeStepType,
                                  const std::string& zone,
                                  const std::string& clusterName)
{
    auto nvars = problem->NombreDeVariables;

    if (nvars > var)
    {
        const auto location
          = LocationIdentifier(zone, Antares::Data::Enum::ExportStructLocationDict::area)
            + SEPARATOR
            + Antares::Data::Enum::toString(Antares::Data::Enum::ExportStructDict::PalierThermique)
            + "<" + clusterName + ">";

        auto fullName = BuildName(
          Antares::Data::Enum::toString(structDict), location, TimeIdentifier(ts, timeStepType));
        problem->NomDesVariables[var] = fullName;
    }
}
void RenameShortTermStorageVariable(PROBLEME_ANTARES_A_RESOUDRE* problem,
                                    int var,
                                    Antares::Data::Enum::ExportStructDict structDict,
                                    int ts,
                                    Antares::Data::Enum::ExportStructTimeStepDict timeStepType,
                                    const std::string& zone,
                                    const std::string& shortTermStorageName)
{
    auto nvars = problem->NombreDeVariables;

    if (nvars > var)
    {
        const auto location
          = LocationIdentifier(zone, Antares::Data::Enum::ExportStructLocationDict::area)
            + SEPARATOR
            + Antares::Data::Enum::toString(Antares::Data::Enum::ExportStructDict::ShortTermStorage)
            + "<" + shortTermStorageName + ">";
        auto fullName = BuildName(
          Antares::Data::Enum::toString(structDict), location, TimeIdentifier(ts, timeStepType));
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
