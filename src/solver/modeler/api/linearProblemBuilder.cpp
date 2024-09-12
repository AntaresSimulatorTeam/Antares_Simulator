#include <algorithm>
#include <memory>

#include <antares/solver/modeler/api/linearProblemBuilder.h>

namespace Antares::Solver::Modeler::Api
{

LinearProblemBuilder::LinearProblemBuilder(
  const std::vector<std::shared_ptr<LinearProblemFiller>>& fillers):
    fillers_(fillers)
{
}

void LinearProblemBuilder::build()
{
    std::ranges::for_each(fillers_, [](const auto& filler) { filler->addVariables(); });
    std::ranges::for_each(fillers_, [](const auto& filler) { filler->addConstraints(); });
    std::ranges::for_each(fillers_, [](const auto& filler) { filler->addObjective(); });
}

} // namespace Antares::Solver::Modeler::Api
