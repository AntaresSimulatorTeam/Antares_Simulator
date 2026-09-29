// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/HebdoAreaPriceProvider.h"

#include <antares/optimisation/linear-problem-api/linearProblem.h>
#include <antares/optimisation/linear-problem-api/mipConstraint.h>
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares::Optimization
{

namespace
{
std::map<std::string, unsigned> buildAreaIndices(const PROBLEME_HEBDO& problemeHebdo)
{
    std::map<std::string, unsigned> indices;
    unsigned index = 0;
    for (const auto& name: problemeHebdo.NomsDesPays)
    {
        indices.try_emplace(name, index++);
    }
    return indices;
}
} // namespace

double legacyAreaPrice(const PROBLEME_HEBDO& problemeHebdo,
                       unsigned area,
                       unsigned hour,
                       const std::function<double(int)>& dualOf)
{
    const int constraintIndex = problemeHebdo.CorrespondanceCntNativesCntOptim[hour]
                                  .NumeroDeContrainteDesBilansPays[area];
    return -dualOf(constraintIndex);
}

HebdoAreaPriceProvider::HebdoAreaPriceProvider(const PROBLEME_HEBDO& problemeHebdo,
                                               const LinearProblem::Api::ILinearProblem& problem):
    problemeHebdo_(problemeHebdo),
    problem_(problem),
    areaIndices_(buildAreaIndices(problemeHebdo))
{
}

double HebdoAreaPriceProvider::getAreaPrice(const std::string& areaId, unsigned hour) const
{
    // Dual values are not available on MIP problems (OR-Tools' MPConstraint::dual_value() fails).
    // Same behavior as the legacy extraction, where marginal costs are 0 for MIP.
    if (!problem_.isLP())
    {
        return 0.0;
    }

    const auto it = areaIndices_.find(areaId);
    if (it == areaIndices_.end())
    {
        return 0.0;
    }

    return legacyAreaPrice(problemeHebdo_,
                           it->second,
                           hour,
                           [this](int constraintIndex)
                           { return problem_.getConstraint(constraintIndex)->dual(); });
}

} // namespace Antares::Optimization
