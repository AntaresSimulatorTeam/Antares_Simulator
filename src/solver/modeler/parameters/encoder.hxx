#include <antares/solver/modeler/parameters/modelerParameters.h>

#include "yaml-cpp/yaml.h"

namespace YAML
{
template<>
struct convert<Antares::Solver::ModelerParameters>
{
    static bool decode(const Node& node, Antares::Solver::ModelerParameters& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.solver = node["solver"].as<std::string>();
        rhs.solverLogs = node["solver-logs"].as<bool>(false);
        rhs.solverParameters = node["solver-parameters"].as<std::string>();
        rhs.noOutput = node["no-output"].as<bool>(false);
        return true;
    }
};
} // namespace YAML
