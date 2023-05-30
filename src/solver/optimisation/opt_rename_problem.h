#pragma once
#include "opt_structure_probleme_a_resoudre.h"
#include "opt_export_structure.h"

const std::string SEPARATOR = "::";
const std::string ZONE_SEPARATOR = "$$";

void RenameLinkVariable(PROBLEME_ANTARES_A_RESOUDRE* problem_hebdo,
                        int var,
                        Antares::Data::Enum::ExportStructDict structDict,
                        int ts,
                        Antares::Data::Enum::ExportStructTimeStepDict time_step_type,
                        const std::string& origin,
                        const std::string& extremite);

void RenameZoneVariable(PROBLEME_ANTARES_A_RESOUDRE* problem,
                        int var,
                        Antares::Data::Enum::ExportStructDict structDict,
                        int ts,
                        Antares::Data::Enum::ExportStructTimeStepDict time_step_type,
                        const std::string& zone);

void RenameThermalClusterVariable(PROBLEME_ANTARES_A_RESOUDRE* problem,
                                  int var,
                                  Antares::Data::Enum::ExportStructDict structDict,
                                  int ts,
                                  Antares::Data::Enum::ExportStructTimeStepDict time_step_type,
                                  const std::string& zone,
                                  const std::string& palier_name);

std::string BuildName(const std::string& name,
                      const std::string& location,
                      const std::string& time_identifier);
inline std::string time_identifier(int time_step,
                                   Antares::Data::Enum::ExportStructTimeStepDict time_step_type)
{
    return Antares::Data::Enum::toString(time_step_type) + "<" + std::to_string(time_step) + ">";
}
inline std::string location_identifier(const std::string& location,
                                       Antares::Data::Enum::ExportStructLocationDict location_type)
{
    return Antares::Data::Enum::toString(location_type) + "<" + location + ">";
}