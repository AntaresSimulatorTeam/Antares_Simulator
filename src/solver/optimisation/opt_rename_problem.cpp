#include "opt_rename_problem.h"
#include <sstream>
void RenameVariable(PROBLEME_ANTARES_A_RESOUDRE* problem,
                    int var,
                    Antares::Data::Enum::ExportStructDict structDict,
                    int ts,
                    const std::string& origin,
                    const std::string& extremite,
                    std::optional<int> secondVal)
{
    auto nvars = problem->NombreDeVariables;
    if (nvars > var)
    {
        // if (problem->NomDesVariables[var][0] != '\0')
        // {
        //     std::cout << "already named variable " << var << " " << problem->NomDesVariables[var]
        //               << "\n";
        // }
        std::stringstream buffer;
        buffer << origin << ZONE_SEPARATOR << extremite << SEPARATOR;
        if (secondVal.has_value())
        {
            buffer << secondVal.value() << SEPARATOR;
        }
        auto full_name
          = BuildName(Antares::Data::Enum::toString(structDict), buffer.str(), std::to_string(ts));
        // std::cout << "*************** RENAME " << problem->NomDesVariables[var] << " to "
        //           << buffer.str() << "***************\n";
        strcpy(problem->NomDesVariables[var], full_name.c_str());
    }
}
void RenameVariable(PROBLEME_ANTARES_A_RESOUDRE* problem,
                    int var,
                    const std::string& varname,
                    int ts,
                    const std::string& zone)
{
    auto nvars = problem->NombreDeVariables;
    if (nvars > var)
    {
        auto full_name = BuildName(varname, zone, std::to_string(ts));
        strcpy(problem->NomDesVariables[var], full_name.c_str());
    }
}
void RenameVariable(PROBLEME_ANTARES_A_RESOUDRE* problem,
                    int var,
                    Antares::Data::Enum::ExportStructDict structDict,
                    int ts,
                    const std::string& zone)
{
    RenameVariable(problem, var, Antares::Data::Enum::toString(structDict), ts, zone);
}
void RenameVariable(PROBLEME_ANTARES_A_RESOUDRE* problem,
                    int var,
                    Antares::Data::Enum::ExportStructDict structDict,
                    int ts,
                    const std::string& zone,
                    int palier)
{
    const auto palier_name
      = Antares::Data::Enum::toString(structDict) + "<" + std::to_string(palier) + ">";
    RenameVariable(problem, var, palier_name, ts, zone);
}

std::string BuildName(const std::string& name,
                      const std::string& location,
                      const std::string& time_identifier)
{
    std::stringstream buffer;
    buffer << name << SEPARATOR << location << SEPARATOR << time_identifier;
    return buffer.str();
}
