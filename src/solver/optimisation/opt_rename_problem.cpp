#include "opt_rename_problem.h"
#include <sstream>
void RenameLinkVariable(PROBLEME_ANTARES_A_RESOUDRE* problem,
                        int var,
                        Antares::Data::Enum::ExportStructDict structDict,
                        int ts,
                        Antares::Data::Enum::ExportStructTimeStepDict time_step_type,
                        const std::string& origin,
                        const std::string& extremite)
{
    auto nvars = problem->NombreDeVariables;
    if (nvars > var && problem->NomDesVariables[var][0] == '\0')
    {
        const auto location = origin + ZONE_SEPARATOR + extremite;
        auto full_name = BuildName(
          Antares::Data::Enum::toString(structDict),
          location_identifier(location, Antares::Data::Enum::ExportStructLocationDict::link),
          time_identifier(ts, time_step_type));
        strcpy(problem->NomDesVariables[var], full_name.c_str());
    }
}

void RenameZoneVariable(PROBLEME_ANTARES_A_RESOUDRE* problem,
                        int var,
                        Antares::Data::Enum::ExportStructDict structDict,
                        int ts,
                        Antares::Data::Enum::ExportStructTimeStepDict time_step_type,
                        const std::string& zone)
{
    auto nvars = problem->NombreDeVariables;
    if (nvars > var)
    {
        auto full_name = BuildName(
          Antares::Data::Enum::toString(structDict),
          location_identifier(zone, Antares::Data::Enum::ExportStructLocationDict::area),
          time_identifier(ts, time_step_type));
        strcpy(problem->NomDesVariables[var], full_name.c_str());
    }
}
void RenameThermalClusterVariable(PROBLEME_ANTARES_A_RESOUDRE* problem,
                                  int var,
                                  Antares::Data::Enum::ExportStructDict structDict,
                                  int ts,
                                  Antares::Data::Enum::ExportStructTimeStepDict time_step_type,
                                  const std::string& zone,
                                  int palier)
{
    auto nvars = problem->NombreDeVariables;

    if (nvars > var)
    {
        const auto location
          = location_identifier(zone, Antares::Data::Enum::ExportStructLocationDict::area)
            + SEPARATOR + Antares::Data::Enum::toString(structDict) + "<" + std::to_string(palier)
            + ">";
        auto full_name = BuildName(Antares::Data::Enum::toString(
                                     Antares::Data::Enum::ExportStructDict::DispatchableProduction),
                                   location,
                                   time_identifier(ts, time_step_type));
        strcpy(problem->NomDesVariables[var], full_name.c_str());
    }
}

std::string BuildName(const std::string& name,
                      const std::string& location,
                      const std::string& time_identifier)
{
    std::stringstream buffer;
    buffer << name << SEPARATOR << location << SEPARATOR << time_identifier;
    return buffer.str();
}
