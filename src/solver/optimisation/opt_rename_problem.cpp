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
    if (nvars > var && problem->NomDesVariables[var][0] == '\0')
    {
        const auto location = origin + ZONE_SEPARATOR + destination;
        auto fullName = BuildName(
          Antares::Data::Enum::toString(structDict),
          LocationIdentifier(location, Antares::Data::Enum::ExportStructLocationDict::link),
          TimeIdentifier(ts, timeStepType));
        strcpy(problem->NomDesVariables[var], fullName.c_str());
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
        strcpy(problem->NomDesVariables[var], fullName.c_str());
    }
}
void RenameThermalClusterVariable(PROBLEME_ANTARES_A_RESOUDRE* problem,
                                  int var,
                                  Antares::Data::Enum::ExportStructDict structDict,
                                  int ts,
                                  Antares::Data::Enum::ExportStructTimeStepDict timeStepType,
                                  const std::string& zone,
                                  const std::string& namePalier)
{
    auto nvars = problem->NombreDeVariables;

    if (nvars > var)
    {
        const auto location
          = LocationIdentifier(zone, Antares::Data::Enum::ExportStructLocationDict::area)
            + SEPARATOR + Antares::Data::Enum::toString(structDict) + "<" + namePalier + ">";
        auto fullName = BuildName(Antares::Data::Enum::toString(
                                    Antares::Data::Enum::ExportStructDict::DispatchableProduction),
                                  location,
                                  TimeIdentifier(ts, timeStepType));
        strcpy(problem->NomDesVariables[var], fullName.c_str());
    }
}

std::string BuildName(const std::string& name,
                      const std::string& location,
                      const std::string& timeIdentifier)
{
    std::stringstream buffer;
    buffer << name << SEPARATOR << location << SEPARATOR << timeIdentifier;
    return buffer.str();
}
