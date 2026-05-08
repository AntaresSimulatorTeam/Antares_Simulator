// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <map>
#include <string>

#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/optimisation/linear-problem-api/ILinearProblemData.h"
#include "antares/solver/optim-model-filler/TimeDependentLinearExpression.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/study/system-model/system.h"

namespace Antares::Optimization
{

class ComponentToAreaResultFiller
{
public:
    ComponentToAreaResultFiller(PROBLEME_HEBDO* problemeHebdo,
                                const Optimisation::OptimEntityContainer& optimEntityContainer);

    void fillAreaContributions(const Optimisation::LinearProblemApi::FillContext& ctx);

private:
    PROBLEME_HEBDO* problemeHebdo_;
    const ModelerStudy::SystemModel::System* modelerSystem_;
    const Optimisation::OptimEntityContainer& optimEntityContainer_;
    std::map<std::string, int> areaIndices_;

    double evaluateExpressionAtTimestep(const TimeDependentLinearExpression& expr,
                                        unsigned ts) const;
};

} // namespace Antares::Optimization
