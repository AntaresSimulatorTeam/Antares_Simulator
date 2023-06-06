#pragma once
#include "opt_structure_probleme_a_resoudre.h"
#include "opt_export_structure.h"

const std::string SEPARATOR = "::";
const std::string ZONE_SEPARATOR = "$$";

void RenameLinkVariable(PROBLEME_ANTARES_A_RESOUDRE* problem_hebdo,
                        int var,
                        Antares::Data::Enum::ExportStructDict structDict,
                        int ts,
                        Antares::Data::Enum::ExportStructTimeStepDict timeStepType,
                        const std::string& origin,
                        const std::string& destination);

void RenameAreaVariable(PROBLEME_ANTARES_A_RESOUDRE* problem,
                        int var,
                        Antares::Data::Enum::ExportStructDict structDict,
                        int ts,
                        Antares::Data::Enum::ExportStructTimeStepDict timeStepType,
                        const std::string& zone);
void RenameAreaVariable(PROBLEME_ANTARES_A_RESOUDRE* problem,
                        int var,
                        Antares::Data::Enum::ExportStructDict structDict,
                        int ts,
                        Antares::Data::Enum::ExportStructTimeStepDict timeStepType,
                        const std::string& zone,
                        int layerIndex);

void RenameThermalClusterVariable(PROBLEME_ANTARES_A_RESOUDRE* problem,
                                  int var,
                                  Antares::Data::Enum::ExportStructDict structDict,
                                  int ts,
                                  Antares::Data::Enum::ExportStructTimeStepDict timeStepType,
                                  const std::string& zone,
                                  const std::string& clusterName);
void RenameShortTermStorageVariable(PROBLEME_ANTARES_A_RESOUDRE* problem,
                                    int var,
                                    Antares::Data::Enum::ExportStructDict structDict,
                                    int ts,
                                    Antares::Data::Enum::ExportStructTimeStepDict timeStepType,
                                    const std::string& zone,
                                    const std::string& shortTermStorageName);

std::string BuildName(const std::string& name,
                      const std::string& location,
                      const std::string& timeIdentifier);
inline std::string TimeIdentifier(int timeStep,
                                  Antares::Data::Enum::ExportStructTimeStepDict timeStepType)
{
    return Antares::Data::Enum::toString(timeStepType) + "<" + std::to_string(timeStep) + ">";
}
inline std::string LocationIdentifier(const std::string& location,
                                      Antares::Data::Enum::ExportStructLocationDict locationType)
{
    return Antares::Data::Enum::toString(locationType) + "<" + location + ">";
}