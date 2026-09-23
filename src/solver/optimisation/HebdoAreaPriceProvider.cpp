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

HebdoAreaPriceProvider::HebdoAreaPriceProvider(const PROBLEME_HEBDO& problemeHebdo,
                                               const LinearProblem::Api::ILinearProblem& problem):
    problemeHebdo_(problemeHebdo),
    problem_(problem),
    areaIndices_(buildAreaIndices(problemeHebdo))
{
}

double HebdoAreaPriceProvider::getAreaPrice(const std::string& areaId, unsigned hour) const
{
    const auto it = areaIndices_.find(areaId);
    if (it == areaIndices_.end())
    {
        return 0.0;
    }

    const unsigned constraintIndex = problemeHebdo_.CorrespondanceCntNativesCntOptim[hour]
                                       .NumeroDeContrainteDesBilansPays[it->second];
    // Same sign convention as the legacy extra-outputs' area price.
    return -problem_.getConstraint(constraintIndex)->dual();
}

} // namespace Antares::Optimization
