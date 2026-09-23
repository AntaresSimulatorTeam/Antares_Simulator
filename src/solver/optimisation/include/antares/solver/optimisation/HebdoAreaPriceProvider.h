// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

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
 * \brief Reads the dual value of a legacy area's balance equation from the solved
 * linear problem, for GEMS components connected to a hybrid area (see the 'price'
 * field of a port-type's area-connection).
 *
 * The value follows the same sign convention as the legacy extra-outputs' area
 * price (see LegacyExtraOutputs.cpp), and is 0 for a MIP resolution since
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
