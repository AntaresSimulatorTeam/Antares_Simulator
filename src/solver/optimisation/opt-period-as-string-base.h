#pragma once

#include <string>

// ---------------------------------------------
// Optimization period as string : base class
// --------------------------------------------
class optPeriodAsString
{
public:
    virtual std::string to_string() const = 0;
    virtual ~optPeriodAsString() = default;
};
