// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <antares/modeler-optimisation-container/scenarioGroupRepo.h>
#include <antares/optimisation/linear-problem-data-impl/linearProblemData.h>
#include <antares/solver/modeler/parameters/scenarioScope.h>
#include <antares/solver/optim-model-filler/BendersDecomposition.h>
#include <antares/study/system-model/heuristic.h>
#include <antares/study/system-model/library.h>
#include <antares/study/system-model/system.h>

namespace Antares::Solver
{

enum class ResolutionMode : unsigned
{
    BENDERS_DECOMPOSITION = 0,
    SEQUENTIAL_SUBPROBLEMS = 1
};

using ComponentProperty = std::pair<std::string, std::string>; // (id, value)

/// Heuristics declared in optim-config.yml, by model id (format: libraryId.modelId)
using HeuristicsByModel = std::map<std::string,
                                   std::vector<ModelerStudy::SystemModel::HeuristicDefinition>>;

struct ModelerData
{
    std::vector<ModelerStudy::SystemModel::Library> libraries;
    std::unique_ptr<ModelerStudy::SystemModel::System> system;
    std::unique_ptr<LinearProblem::Api::ILinearProblemData> dataSeries;
    LinearProblem::ScenarioGroupRepository scenarioGroupRepository;
    ResolutionMode resolutionMode = ResolutionMode::SEQUENTIAL_SUBPROBLEMS;
    ScenarioScope scenarioScope;
    LinearProblem::BendersDecomposition bendersDecomposition;
    std::map<std::string, std::vector<ComponentProperty>> componentProperties;
    HeuristicsByModel heuristicsByModel;

    /// The heuristic definition to apply to a component, nullptr if it is not handled by a
    /// heuristic
    const ModelerStudy::SystemModel::HeuristicDefinition* heuristicFor(
      const ModelerStudy::SystemModel::Component& component) const
    {
        const auto type = component.heuristicType();
        if (!type)
        {
            return nullptr;
        }
        const auto* model = component.getModel();
        const auto it = heuristicsByModel.find(model->LibraryId() + "." + model->Id());
        if (it == heuristicsByModel.end())
        {
            return nullptr;
        }
        for (const auto& heuristic: it->second)
        {
            if (heuristic.type == *type)
            {
                return &heuristic;
            }
        }
        return nullptr;
    }

    bool hasHeuristicComponents() const
    {
        return system
               && std::ranges::any_of(system->Components(),
                                      [](const auto& c) { return c.heuristicType().has_value(); });
    }
};

// Operator for printing ResolutionMode in logs and tests
inline std::ostream& operator<<(std::ostream& os, ResolutionMode mode)
{
    switch (mode)
    {
    case ResolutionMode::SEQUENTIAL_SUBPROBLEMS:
        os << "SEQUENTIAL_SUBPROBLEMS";
        break;
    case ResolutionMode::BENDERS_DECOMPOSITION:
        os << "BENDERS_DECOMPOSITION";
        break;
    default:
        os << "UNKNOWN";
        break;
    }
    return os;
}

} // namespace Antares::Solver
