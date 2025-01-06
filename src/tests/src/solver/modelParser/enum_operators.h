/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once
#include <ostream>
#include <string>

#include "antares/solver/modelParser/Library.h"
#include "antares/study/system-model/valueType.h"

namespace Antares::Study::SystemModel
{
inline std::ostream& operator<<(std::ostream& os, const ValueType& value_type)
{
    using namespace std::string_literals;
    switch (value_type)
    {
    case ValueType::FLOAT:
        os << "float"s;
        break;
    case ValueType::INTEGER:
        os << "integer"s;
        break;
    case ValueType::BOOL:
        os << "boolean"s;
        break;
    default:
        os << "UNKNOWN"s;
        break;
    }
    return os;
}
} // namespace Antares::Study::SystemModel

namespace Antares::Solver::ModelParser
{
inline std::ostream& operator<<(std::ostream& os, const ValueType& value_type)
{
    using namespace std::string_literals;
    switch (value_type)
    {
    case ValueType::CONTINUOUS:
        os << "CONTINUOUS"s;
        break;
    case ValueType::INTEGER:
        os << "INTEGER"s;
        break;
    case ValueType::BOOL:
        os << "BOOL"s;
        break;
    default:
        os << "UNKNOWN"s;
        break;
    }
    return os;
}
} // namespace Antares::Solver::ModelParser
