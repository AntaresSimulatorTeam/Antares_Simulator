#include <algorithm>
#include <memory>

#include <antares/solver/modeler/api/linearProblemBuilder.h>

namespace Antares::Solver::Modeler::Api
{

LinearProblemBuilder::LinearProblemBuilder(const std::vector<LinearProblemFiller*>& fillers):
    fillers_(fillers)
{
}

void LinearProblemBuilder::build(ILinearProblem& pb, LinearProblemData& data)
{
    std::ranges::for_each(fillers_, [&](const auto& filler) { filler->addVariables(pb, data); });
    std::ranges::for_each(fillers_, [&](const auto& filler) { filler->addConstraints(pb, data); });
    std::ranges::for_each(fillers_, [&](const auto& filler) { filler->addObjective(pb, data); });
}

} // namespace Antares::Solver::Modeler::Api
