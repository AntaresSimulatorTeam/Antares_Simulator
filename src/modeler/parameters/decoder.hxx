// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/solver/modeler/parameters/modelerParameters.h>

#include "yaml-cpp/yaml.h"

namespace YAML
{
template<>
struct convert<Antares::Solver::ScenarioScope>
{
    static bool decode(const Node& node, Antares::Solver::ScenarioScope& rhs)
    {
        if (node.IsNull() || !node.IsDefined())
        {
            // Empty scenario-scope block -> default (scenario 0 only)
            return true;
        }
        if (!node.IsMap())
        {
            return false;
        }

        const Node& includeNode = node["include"];
        if (includeNode.IsDefined() && !includeNode.IsNull())
        {
            if (!includeNode.IsSequence())
            {
                return false;
            }
            for (const auto& entry: includeNode)
            {
                rhs.include.push_back(scalarsToString(entry));
            }
        }

        const Node& excludeNode = node["exclude"];
        if (excludeNode.IsDefined() && !excludeNode.IsNull())
        {
            if (!excludeNode.IsSequence())
            {
                return false;
            }
            for (const auto& entry: excludeNode)
            {
                rhs.exclude.push_back(scalarsToString(entry));
            }
        }

        const Node& playlistNode = node["playlist-file"];
        if (playlistNode.IsDefined() && !playlistNode.IsNull())
        {
            rhs.playlistFile = playlistNode.as<std::string>();
        }

        return true;
    }

private:
    // Convert a scalar node to its string representation, so that both "5" and 5 become "5".
    static std::string scalarsToString(const Node& node)
    {
        if (node.IsScalar())
        {
            return node.as<std::string>();
        }
        throw YAML::BadConversion(node.Mark());
    }
};

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
        rhs.exportMps = node["export-mps"].as<bool>(false);
        rhs.firstTimeStep = node["first-time-step"].as<unsigned int>(0);
        // TODO check this value
        rhs.lastTimeStep = node["last-time-step"].as<unsigned int>(167);
        if (const Node& scenarioScopeNode = node["scenario-scope"];
            scenarioScopeNode.IsDefined() && !scenarioScopeNode.IsNull())
        {
            rhs.scenarioScope = scenarioScopeNode.as<Antares::Solver::ScenarioScope>();
        }
        return true;
    }
};
} // namespace YAML
