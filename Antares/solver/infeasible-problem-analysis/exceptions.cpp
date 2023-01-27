#include "exceptions.h"

/* This file contains exceptions that can be thrown by the
   infeasible problem analyser.
*/

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
