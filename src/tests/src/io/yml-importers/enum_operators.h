// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <ostream>
#include <string>

#include "antares/io/inputs/yml-model/Library.h"
#include "antares/study/system-model/valueType.h"

namespace Antares::ModelerStudy::SystemModel
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
} // namespace Antares::ModelerStudy::SystemModel

namespace Antares::IO::Inputs::YmlModel
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
} // namespace Antares::IO::Inputs::YmlModel
