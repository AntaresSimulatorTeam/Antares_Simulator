#pragma once

#include <stdexcept>
#include <string>

namespace Antares
{
namespace Optimization
{
class SlackVariablesEmpty : public std::runtime_error
{
public:
    SlackVariablesEmpty(const std::string&);
};
class ProblemResolutionFailed : public std::runtime_error
{
public:
    ProblemResolutionFailed(const std::string&);
};
} // namespace Optimization
} // namespace Antares
