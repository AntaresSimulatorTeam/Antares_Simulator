#pragma once
#include "opt_structure_probleme_a_resoudre.h"
#include "opt_export_structure.h"

const std::string SEPARATOR = "::";
const std::string ZONE_SEPARATOR = "$$";
class VariableNamer
{
private:
    PROBLEME_ANTARES_A_RESOUDRE* problem;
    int timeStep = 0;
    std::string origin;
    std::string destination;
    std::string area;

public:
    VariableNamer(PROBLEME_ANTARES_A_RESOUDRE* problem) : problem(problem)
    {
    }
    void SetTimeStep(int ts)
    {
        timeStep = ts;
    }
    void SetAreaName(const std::string& areaName)
    {
        area = areaName;
    }
    std::string AreaName() const
    {
        return area;
    }
    void SetOrigin(const std::string& linkOrigin)
    {
        origin = linkOrigin;
    }
    void SetDestination(const std::string& linkDestination)
    {
        destination = linkDestination;
    }
    int TimeStep() const
    {
        return timeStep;
    }
    void SetLinkVariableName(int var, Antares::Data::Enum::ExportStructDict structDict);

    void SetAreaVariableName(int var, Antares::Data::Enum::ExportStructDict structDict);
    void SetAreaVariableName(int var,
                             Antares::Data::Enum::ExportStructDict structDict,
                             int layerIndex);

    void SetThermalClusterVariableName(int var,
                                       Antares::Data::Enum::ExportStructDict structDict,
                                       const std::string& clusterName);
    void SetShortTermStorageVariableName(int var,
                                         Antares::Data::Enum::ExportStructDict structDict,
                                         const std::string& shortTermStorageName);
};
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