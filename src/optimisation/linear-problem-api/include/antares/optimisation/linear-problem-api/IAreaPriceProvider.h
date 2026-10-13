// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>

namespace Antares::LinearProblem::Api
{

/**
 * \brief Provides the dual value of a legacy area's power balance equation, for
 * GEMS components connected to a hybrid area (see the 'price' field of a
 * port-type's area-connection).
 *
 * The value is only meaningful once the linear problem has been solved: it is
 * the dual of the area's balance equation, expressed as a price. It is 0
 * whenever no such dual is available (e.g. a MIP resolution, where duals are
 * not computed), exactly like the 'dual' and 'reduced_cost' functions used
 * for full-GEMS constraints.
 */
class IAreaPriceProvider
{
public:
    virtual ~IAreaPriceProvider() = default;

    /// \param areaId legacy area id.
    /// \param hour local hour index within the fill context's time window.
    [[nodiscard]] virtual double getAreaPrice(const std::string& areaId, unsigned hour) const = 0;
};

} // namespace Antares::LinearProblem::Api
