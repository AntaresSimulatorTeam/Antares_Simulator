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
    explicit SlackVariablesEmpty(const std::string& message);
};
class ProblemResolutionFailed : public std::runtime_error
{
public:
    explicit ProblemResolutionFailed(const std::string& message);
};
} // namespace Optimization
} // namespace Antares
