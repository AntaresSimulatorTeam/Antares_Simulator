#pragma once

#include <string>

// ---------------------------------------------
// Optimization period as string : base class
// --------------------------------------------
class OptPeriodStringGenerator
{
public:
    virtual std::string to_string() const = 0;
    virtual ~OptPeriodStringGenerator() = default;
};
