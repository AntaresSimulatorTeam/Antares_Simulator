// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <functional>
#include <map>
#include <string>

#include <antares/optimisation/linear-problem-api/IAreaPriceProvider.h>

struct PROBLEME_HEBDO;

namespace Antares::LinearProblem::Api
{
class ILinearProblem;
}

namespace Antares::Optimization
{

/**
 * \brief Price of a legacy area: the opposite of the dual of its balance equation.
 *
 * Single implementation shared by the legacy extra-outputs and by the GEMS components'
 * area-connection 'price' field, so both always agree on the constraint and sign convention.
 *
 * \param area index of the area in problemeHebdo.NomsDesPays
 * \param hour hour of the optimization window (index of CorrespondanceCntNativesCntOptim)
 * \param dualOf returns the dual value of a constraint, given its index in the problem
 */
double legacyAreaPrice(const PROBLEME_HEBDO& problemeHebdo,
                       unsigned area,
                       unsigned hour,
                       const std::function<double(int)>& dualOf);

/**
 * \brief Reads the dual value of a legacy area's balance equation from the solved
 * linear problem, for GEMS components connected to a hybrid area (see the 'price'
 * field of a port-type's area-connection).
 *
 * The value is computed by legacyAreaPrice(), and is 0 for a MIP resolution since
 * duals are not computed in that case.
 */
class HebdoAreaPriceProvider final: public LinearProblem::Api::IAreaPriceProvider
{
public:
    HebdoAreaPriceProvider(const PROBLEME_HEBDO& problemeHebdo,
                           const LinearProblem::Api::ILinearProblem& problem);

    [[nodiscard]] double getAreaPrice(const std::string& areaId, unsigned hour) const override;

private:
    const PROBLEME_HEBDO& problemeHebdo_;
    const LinearProblem::Api::ILinearProblem& problem_;
    std::map<std::string, unsigned> areaIndices_;
};

} // namespace Antares::Optimization
