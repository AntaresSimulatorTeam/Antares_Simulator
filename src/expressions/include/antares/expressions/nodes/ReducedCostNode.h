/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once

#include <string>

#include <antares/expressions/nodes/Leaf.h>
#include "antares/modeler-optimisation-container/TimeIndex.h"

namespace Antares::Expressions::Nodes
{
/**
 * @brief Represents a reduced cost node in a syntax tree, storing a variable name.
 */

class ReducedCostNode final: public Leaf<std::string>
{
public:
    explicit ReducedCostNode(const std::string& value,
                             const unsigned index,
                             const Optimisation::TimeIndex time_index = Optimisation::TimeIndex::
                               VARYING_IN_TIME_AND_SCENARIO):
        Leaf<std::string>(value),
        index_(index),
        time_index_(time_index)
    {
    }

    std::string name() const override
    {
        return "ReducedCostNode";
    }

    unsigned index() const
    {
        return index_;
    }

    Optimisation::TimeIndex timeIndex() const
    {
        return time_index_;
    }

private:
    unsigned index_;
    const Optimisation::TimeIndex time_index_;
};
} // namespace Antares::Expressions::Nodes
