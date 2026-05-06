// Copyright 2007-2026, RTE
// SPDX-License-Identifier: MPL-2.0

#include "antares/expressions/visitors/TimeSumUtils.h"

#include <fmt/format.h>

#include "antares/exception/InvalidArgumentError.hpp"
#include "antares/expressions/visitors/EvalVisitor.h"
#include "antares/expressions/visitors/PrintVisitor.h"
#include "antares/expressions/visitors/VariabilityVisitor.h"

namespace Antares::Expressions::Visitors
{
int normalizeTimeIndex(int timeIndex, int size)
{
    int r = timeIndex % size;
    if (r < 0)
    {
        r += size;
    }
    return r;
}

int resolveTimeSumBound(const Nodes::Node* bound, EvalVisitor& visitor, unsigned localTimeStep)
{
    if (const auto* tPlusNode = dynamic_cast<const Nodes::TPlusNode*>(bound))
    {
        const auto offset = static_cast<int>(visitor.dispatch(tPlusNode->child()).valueAsDouble());
        return static_cast<int>(localTimeStep) + offset;
    }
    return static_cast<int>(visitor.dispatch(bound).valueAsDouble());
}

void checkTimeSumBoundVariability(const Nodes::Node* bound, VariabilityVisitor& variabilityVisitor)
{
    const auto variability = variabilityVisitor.dispatch(bound);
    if (isTimeDependent(variability))
    {
        PrintVisitor printVisitor;
        throw Error::InvalidArgumentError(
          fmt::format("A sum bound must be fixed in time in '{}'.", printVisitor.dispatch(bound)));
    }
}

} // namespace Antares::Expressions::Visitors
