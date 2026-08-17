// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/optimisation/linear-problem-api/linearProblemFiller.h"
#include "antares/solver/optim-model-filler/TimeDependentLinearExpression.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/study/system-model/system.h"

namespace Antares::LinearProblem
{
namespace Api
{
class ILinearProblem;
class IMipConstraint;
} // namespace Api
class OptimEntityContainer;
} // namespace Antares::LinearProblem

namespace Antares::Optimization
{
using ConstraintIndicesHelper = std::function<
  std::vector<unsigned>(const PROBLEME_HEBDO* problemeHebdo,
                        const LinearProblem::Api::FillContext& ctx,
                        const unsigned& areaIndex)>;

/**
 * \brief Fills the linear problem with constraints and variables related to component-to-area
 * connections.
 *
 * This class is responsible for adding variables, constraints, and objectives to the linear problem
 * based on the connections between components and areas in the Antares study.
 */
class ComponentToAreaConnectionFiller final: public LinearProblem::Api::LinearProblemFiller
{
public:
    explicit ComponentToAreaConnectionFiller(
      const PROBLEME_HEBDO* problemeHebdo,
      LinearProblem::OptimEntityContainer& optimContainer,
      const LinearProblem::Api::ILinearProblemData* data,
      const LinearProblem::ScenarioGroupRepository& scenarioGroupRepository);

    void addVariables(const LinearProblem::Api::FillContext& ctx) override;
    void addConstraints(const LinearProblem::Api::FillContext& ctx) override;
    void addObjectives(const LinearProblem::Api::FillContext& ctx) override;

private:
    // Data members
    const PROBLEME_HEBDO* problemeHebdo_;
    const ModelerStudy::SystemModel::System* modelerSystem_;
    LinearProblem::OptimEntityContainer& optimEntityContainer_;
    LinearProblem::Api::ILinearProblem& pb_;
    const LinearProblem::Api::ILinearProblemData* data_;
    const LinearProblem::ScenarioGroupRepository& scenarioGroupRepo_;

    std::map<std::string, unsigned> areaIndices_;

    // Function members
    void checkAreasFromConnexionsExist();

    std::vector<LinearProblem::Api::IMipConstraint*> fetchConstraints(
      const LinearProblem::Api::FillContext& ctx,
      const std::vector<unsigned>& constraintsIndices);

    void addExpressionToConstraint(
      const TimeDependentLinearExpression& linearExpression,
      const LinearProblem::Api::FillContext& ctx,
      const std::vector<LinearProblem::Api::IMipConstraint*>& constraintIndices) const;

    TimeDependentLinearExpression linearExpressionAtPortField(
      const std::string& portId,
      const std::string& fieldId,
      const ModelerStudy::SystemModel::Component& component,
      const LinearProblem::Api::FillContext& ctx);

    void addPortContributionToLinearPb(const LinearProblem::Api::FillContext& ctx,
                                       const ModelerStudy::SystemModel::Component& component,
                                       const std::string& portId,
                                       const std::string& portField,
                                       const unsigned& areaIndex,
                                       const ConstraintIndicesHelper& helper);
};

} // namespace Antares::Optimization
