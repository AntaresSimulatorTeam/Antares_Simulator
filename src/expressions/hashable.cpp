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
#include <boost/functional/hash.hpp>

#include <antares/expressions/hashable.h>

namespace Antares::Expressions
{

Hashable::Hashable(const std::string& s1, const std::string& s2):
    s1(s1),
    s2(s2)
{
}

bool Hashable::operator==(const Hashable& other) const
{
    return s1 == other.s1 && s2 == other.s2;
}

std::size_t PortFieldHash::operator()(const Hashable& n) const
{
    std::size_t seed = 0;

    boost::hash_combine(seed, boost::hash_value(n.s1));
    boost::hash_combine(seed, boost::hash_value(n.s2));

    return seed;
}

} // namespace Antares::Expressions
