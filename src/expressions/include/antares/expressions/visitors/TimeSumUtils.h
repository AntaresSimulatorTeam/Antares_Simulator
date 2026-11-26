// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/expressions/nodes/ExpressionsNodes.h"

namespace Antares::Expressions::Visitors
{
class EvalVisitor; // forward declaration to avoid include cycles
class VariabilityVisitor;

// Normalize a possibly negative or out-of-range time index into [0, size).
int normalizeTimeIndex(int timeIndex, int size);

// Resolve a time-shift offset, rejecting non-integer values and values outside the int range.
int resolveTimeShift(const Nodes::Node* offset, EvalVisitor& visitor);

// Visit the indices covered by the window [from, to] (no-op if from > to). Indices wrap cyclically
// on [0, size): a window longer than the block covers some indices several times.
// callback(index, count) is called once per index covered, with the number of times it is covered.
// The cost is O(size), whatever the length of the window. For a window that is not longer than the
// block, indices are visited in window order, each with a count of 1.
template<class Callback>
inline void forEachTimeSumIndex(int from, int to, int size, Callback callback)
{
    if (from > to || size <= 0)
    {
        return;
    }
    // 64-bit arithmetic: to - from + 1 can exceed INT_MAX
    const long long length = static_cast<long long>(to) - from + 1;
    const long long fullCycles = length / size;
    const long long remainder = length % size;
    const int first = normalizeTimeIndex(from, size);
    for (int offset = 0; offset < size; ++offset)
    {
        const long long count = fullCycles + (offset < remainder ? 1 : 0);
        if (count > 0)
        {
            callback((first + offset) % size, count);
        }
    }
}

// Resolve a bound used in a time-sum when no variability check is required.
int resolveTimeSumBound(const Nodes::Node* bound, EvalVisitor& visitor, unsigned localTimeStep);

// Check that a time-sum bound is fixed in time and can be evaluated before optimization.
void checkTimeSumBoundVariability(const Nodes::Node* bound, VariabilityVisitor& variabilityVisitor);

// Resolve the index of a TimeIndexNode (x[i]): the index must be fixed in time and resolve to an
// integer. Range checking is left to the caller, which knows the size of the indexed expression.
int resolveTimeIndex(const Nodes::Node* index,
                     EvalVisitor& visitor,
                     VariabilityVisitor& variabilityVisitor);

} // namespace Antares::Expressions::Visitors
