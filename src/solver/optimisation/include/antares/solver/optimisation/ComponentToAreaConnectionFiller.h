// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/optimisation/linear-problem-api/linearProblemFiller.h"
#include "antares/solver/optim-model-filler/TimeDependentLinearExpression.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/study/system-model/system.h"

namespace Antares::Optimisation
{
namespace LinearProblemApi
{
class ILinearProblem;
class IMipConstraint;
} // namespace LinearProblemApi
class OptimEntityContainer;
} // namespace Antares::Optimisation

namespace Antares::Optimization
{
using ConstraintIndicesHelper = std::function<
  std::vector<unsigned>(const PROBLEME_HEBDO* problemeHebdo,
                        const Optimisation::LinearProblemApi::FillContext& ctx,
                        const unsigned& areaIndex)>;

/**
 * \brief Fills the linear problem with constraints and variables related to component-to-area
 * connections.
 *
 * This class is responsible for adding variables, constraints, and objectives to the linear problem
 * based on the connections between components and areas in the Antares study.
 */
class ComponentToAreaConnectionFiller final
    : public Optimisation::LinearProblemApi::LinearProblemFiller
{
public:
    explicit ComponentToAreaConnectionFiller(
      const PROBLEME_HEBDO* problemeHebdo,
      Optimisation::OptimEntityContainer& optimContainer,
      const Optimisation::LinearProblemApi::ILinearProblemData* data,
      const Optimisation::ScenarioGroupRepository& scenarioGroupRepository);

    void addVariables(const Optimisation::LinearProblemApi::FillContext& ctx) override;
    void addConstraints(const Optimisation::LinearProblemApi::FillContext& ctx) override;
    void addObjectives(const Optimisation::LinearProblemApi::FillContext& ctx) override;

private:
    // Data members
    const PROBLEME_HEBDO* problemeHebdo_;
    const ModelerStudy::SystemModel::System* modelerSystem_;
    Optimisation::OptimEntityContainer& optimEntityContainer_;
    Optimisation::LinearProblemApi::ILinearProblem& pb_;
    const Optimisation::LinearProblemApi::ILinearProblemData* data_;
    const Optimisation::ScenarioGroupRepository& scenarioGroupRepo_;

    std::map<std::string, unsigned> areaIndices_;

    // Function members
    void checkAreasFromConnexionsExist();

    std::vector<Optimisation::LinearProblemApi::IMipConstraint*> fetchConstraints(
      const Optimisation::LinearProblemApi::FillContext& ctx,
      const std::vector<unsigned>& constraintsIndices);

    void addExpressionToConstraint(
      const Antares::Optimization::TimeDependentLinearExpression& linearExpression,
      const Optimisation::LinearProblemApi::FillContext& ctx,
      const std::vector<Optimisation::LinearProblemApi::IMipConstraint*>& constraintIndices) const;

    TimeDependentLinearExpression linearExpressionAtPortField(
      const std::string& portId,
      const std::string& fieldId,
      const ModelerStudy::SystemModel::Component& component,
      const Optimisation::LinearProblemApi::FillContext& ctx);

    void addPortContributionToLinearPb(const Optimisation::LinearProblemApi::FillContext& ctx,
                                       const ModelerStudy::SystemModel::Component& component,
                                       const std::string& portId,
                                       const std::string& portField,
                                       const unsigned& areaIndex,
                                       const ConstraintIndicesHelper& helper);
};

} // namespace Antares::Optimization
