/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
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

#include <antares/solver/expressions/nodes/Node.h>

namespace Antares::Solver::Expressions
{
// constraint T to arithmetic types?
template<class T>
class Leaf: public Node
{
public:
    virtual ~Leaf() = default;

    Leaf(const T& value):
        value_(value)
    {
    }

    // private:
    T value_ = 0;
};

class LiteralNode: public Leaf<double>
{
public:
    using Leaf<double>::Leaf;
};

class VariableNode: public Leaf<std::string>
{
public:
    using Leaf<std::string>::Leaf;
};

class ParameterNode: public Leaf<std::string>
{
public:
    using Leaf<std::string>::Leaf;
};

} // namespace Antares::Solver::Expressions
