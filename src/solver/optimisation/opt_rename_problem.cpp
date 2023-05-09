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
        std::stringstream buffer;
        buffer << Antares::Data::Enum::toString(structDict) << SEPARATOR;
        buffer << origin << ZONE_SEPARATOR << extremite;
        if (secondVal.has_value())
        {
            buffer << secondVal.value() << SEPARATOR;
        }
        buffer << ts;
        problem->NomDesVariables[var] = buffer.str();
    }
}
