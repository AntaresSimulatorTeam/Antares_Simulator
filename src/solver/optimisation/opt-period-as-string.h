#pragma once

// ------------------------------------
// Optimization period : base class
// ------------------------------------
class optPeriodAsString
{
public:
    virtual std::string to_string() const = 0;
    virtual ~optPeriodAsString() = default;
};
