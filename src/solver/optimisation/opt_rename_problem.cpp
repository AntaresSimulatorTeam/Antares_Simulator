#include "opt_rename_problem.h"
#include <sstream>
void RenameVariable(PROBLEME_ANTARES_A_RESOUDRE* problem,
                    int var,
                    Antares::Data::Enum::ExportStructDict structDict,
                    int ts,
                    const std::string& origin,
                    const std::string& extremite,
                    int secondVal)
{
    auto nvars = problem->NombreDeVariables;
    if (nvars > var)
    {
        std::stringstream buffer;
        buffer << Antares::Data::Enum::toString(structDict) << SEPARATOR;
        buffer << origin << ZONE_SEPARATOR << extremite << SEPARATOR;
        buffer << secondVal << SEPARATOR;
        buffer << ts;
        // std::cout << "*************** RENAME " << problem->NomDesVariables[var] << " to "
        //           << buffer.str() << "***************\n";
        strcpy(problem->NomDesVariables[var], buffer.str().c_str());
    }
}
void RenameVariable(PROBLEME_ANTARES_A_RESOUDRE* problem,
                    int var,
                    Antares::Data::Enum::ExportStructDict structDict,
                    int ts,
                    const std::string& zone)
{
    auto nvars = problem->NombreDeVariables;
    if (nvars > var)
    {
        std::stringstream buffer;
        buffer << Antares::Data::Enum::toString(structDict) << SEPARATOR;
        buffer << zone << SEPARATOR;

        buffer << ts;
        strcpy(problem->NomDesVariables[var], buffer.str().c_str());
    }
}
