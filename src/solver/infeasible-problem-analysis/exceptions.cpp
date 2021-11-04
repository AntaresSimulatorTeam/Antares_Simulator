#include "exceptions.h"

namespace Antares
{
namespace Optimization
{
SlackVariablesEmpty::SlackVariablesEmpty(const std::string& s) : std::runtime_error(s)
{
}
ProblemResolutionFailed::ProblemResolutionFailed(const std::string& s) : std::runtime_error(s)
{
}
} // namespace Optimization
} // namespace Antares
