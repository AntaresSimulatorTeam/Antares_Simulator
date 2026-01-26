// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

namespace Antares::ModelerStudy::SystemModel
{
/** Using enum class to avoid primitive obsession. Mainly prevent headhaches when reading
 * Parameter("Param", ValueType::FLOAT, false, true)
 * Avoid mixing wich value is which boolean parameter
 */

enum class TimeDependent : bool
{
    NO = false,
    YES = true
};

enum class ScenarioDependent : bool
{
    NO = false,
    YES = true
};

template<class T>
inline T fromBool(bool in);

template<>
inline TimeDependent fromBool(bool in)
{
    return in ? TimeDependent::YES : TimeDependent::NO;
}

template<>
inline ScenarioDependent fromBool(bool in)
{
    return in ? ScenarioDependent::YES : ScenarioDependent::NO;
}

} // namespace Antares::ModelerStudy::SystemModel
