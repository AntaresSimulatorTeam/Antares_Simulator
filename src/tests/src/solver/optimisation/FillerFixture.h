// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once
#include <memory>
#include <string>

#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"
#include "antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h"
#include "antares/solver/modeler/ModelerData.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/study/system-model/library.h"

struct FillerFixture
{
    std::unique_ptr<PROBLEME_HEBDO> problemeHebdo;
    std::unique_ptr<Antares::Solver::ModelerData> modelerData;
    std::vector<Antares::ModelerStudy::SystemModel::Library> libraries;
    Antares::LinearProblem::MpsolverImpl::OrtoolsLinearProblem linearProblem;
    Antares::LinearProblem::ScenarioGroupRepository scenarioGroupRepository;
    Antares::LinearProblem::DataImpl::LinearProblemData linearProblemData;

    FillerFixture();
    void init(const std::string& systemYaml, const std::string& libraryYaml);

    void setData(const std::string& name, const std::vector<double>& some_param_value);

    void setUpModelerSystem(const std::string& systemYaml, const std::string& libraryYaml);
    void addLegacyVariables(const std::vector<std::string>& variableNames);

    void addModelerVariables(unsigned int ts_start,
                             unsigned int ts_end,
                             Antares::LinearProblem::OptimEntityContainer& optimEntityContainer);

    void addEmptyConstraintsToLinearProblem(std::vector<std::string>& names, double rhs);

    void addEmptyConstraints(std::vector<std::string>& constraintNames, double rhs);

    void addLegacyLp(std::vector<std::string>& constraintNames, double rhs);

    void fillProblemWithThermalCapacityConnectionFiller(
      const Antares::LinearProblem::Api::FillContext& fillCtx,
      Antares::LinearProblem::OptimEntityContainer& optimEntityContainer) const;
};
