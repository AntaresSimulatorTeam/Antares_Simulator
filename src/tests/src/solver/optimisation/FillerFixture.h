// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once
#include <memory>
#include <string>

#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"
#include "antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/study/system-model/library.h"

struct FillerFixture
{
    std::unique_ptr<PROBLEME_HEBDO> problemeHebdo;
    std::unique_ptr<Solver::ModelerData> modelerData;
    std::vector<ModelerStudy::SystemModel::Library> libraries;
    Optimisation::LinearProblemMpsolverImpl::OrtoolsLinearProblem linearProblem;
    Optimisation::ScenarioGroupRepository scenarioGroupRepository;
    std::remove_reference_t<Antares::Optimisation::LinearProblemDataImpl::LinearProblemData&> data;

    FillerFixture();
    void init(const std::string& systemYaml, const std::string& libraryYaml);

    void setData(const std::vector<double>& some_param_value);

    void setUpModelerSystem(const std::string& systemYaml, const std::string& libraryYaml);

    void setUpModelerVariables(unsigned int ts_start,
                               unsigned int ts_end,
                               Optimisation::OptimEntityContainer& optimEntityContainer);

    void addEmptyConstraintsToLinearProblem(std::vector<std::string>& names, double rhs);

    void addEmptyConstraints(std::vector<std::string>& constraintNames,
                             bool useNamedProblems,
                             double rhs);

    void setUpLegacyLp(std::vector<std::string>& constraintNames,
                       bool useNamedProblems,
                       double rhs);

    void fillProblem(const Antares::Optimisation::LinearProblemApi::FillContext& fillCtx,
                     Antares::Optimisation::OptimEntityContainer& optimEntityContainer);
};
