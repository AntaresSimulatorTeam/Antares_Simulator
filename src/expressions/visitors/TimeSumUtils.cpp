// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/expressions/visitors/TimeSumUtils.h"

#include <cmath>
#include <fmt/format.h>
#include <limits>
#include <string_view>

#include "antares/exception/InvalidArgumentError.hpp"
#include "antares/expressions/visitors/EvalVisitor.h"
#include "antares/expressions/visitors/PrintVisitor.h"
#include "antares/expressions/visitors/VariabilityVisitor.h"

namespace Antares::Expressions::Visitors
{
namespace
{
int checkedTimeIndex(double value, const Nodes::Node* bound, std::string_view what = "A sum bound")
{
    if (!std::isfinite(value) || std::trunc(value) != value
        || value < std::numeric_limits<int>::min()
        || value >= -static_cast<double>(std::numeric_limits<int>::min()))
    {
        PrintVisitor printVisitor;
        throw Error::InvalidArgumentError(
          fmt::format("{} must resolve to an integer, got '{}' in '{}'.",
                      what,
                      value,
                      printVisitor.dispatch(bound)));
    }
    return static_cast<int>(value);
}
} // namespace

int normalizeTimeIndex(int timeIndex, int size)
{
    int r = timeIndex % size;
    if (r < 0)
    {
        r += size;
    }
    return r;
}

int resolveTimeSumBound(const Nodes::Node* bound,
                        EvalVisitor& visitor,
                        unsigned localTimeStep,
                        int numberOfTimeSteps)
{
    int result;
    if (const auto* tPlusNode = dynamic_cast<const Nodes::TPlusNode*>(bound))
    {
        const auto offset = checkedTimeIndex(visitor.dispatch(tPlusNode->child()).valueAsDouble(),
                                             tPlusNode->child());
        const auto index = static_cast<long long>(localTimeStep) + offset;
        if (index < std::numeric_limits<int>::min() || index > std::numeric_limits<int>::max())
        {
            PrintVisitor printVisitor;
            throw Error::InvalidArgumentError(
              fmt::format("Resolved sum bound '{}' is outside the supported index range.",
                          printVisitor.dispatch(bound)));
        }
        result = static_cast<int>(index);
    }
    else
    {
        result = checkedTimeIndex(visitor.dispatch(bound).valueAsDouble(), bound);
        if (result < 0 || result >= numberOfTimeSteps)
        {
            PrintVisitor printVisitor;
            throw Error::InvalidArgumentError(
              fmt::format("Absolute sum bound '{}' is outside the block [0, {}].",
                          printVisitor.dispatch(bound),
                          numberOfTimeSteps - 1));
        }
    }
    return result;
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

int resolveTimeIndex(const Nodes::Node* index,
                     EvalVisitor& visitor,
                     VariabilityVisitor& variabilityVisitor)
{
    if (isTimeDependent(variabilityVisitor.dispatch(index)))
    {
        PrintVisitor printVisitor;
        throw Error::InvalidArgumentError(
          fmt::format("A time index must be fixed in time in '{}'.", printVisitor.dispatch(index)));
    }
    return checkedTimeIndex(visitor.dispatch(index).valueAsDouble(), index, "A time index");
}

} // namespace Antares::Expressions::Visitors
