#pragma once

#include <string>

// ---------------------------------------------
// Optimization period as string : base class
// --------------------------------------------
class optPeriodStringGenerator
{
public:
    virtual std::string to_string() const = 0;
    virtual ~optPeriodStringGenerator() = default;
};
