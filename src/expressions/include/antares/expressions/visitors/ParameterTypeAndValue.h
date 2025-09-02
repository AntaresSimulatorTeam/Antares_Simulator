#pragma once

#include <string>

namespace Antares::Expressions::Visitors
{
enum class ParameterType : unsigned int
{
    CONSTANT = 0,
    TIMESERIE = 1
};

// this struct contains more or less the same infos as the one in system.h
struct ParameterTypeAndValue
{
    std::string id;
    ParameterType type;
    std::string value;
};
} // namespace Antares::Expressions::Visitors
