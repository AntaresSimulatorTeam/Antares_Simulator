#include <memory>
#include <antares/solver/modeler/api/linearProblemBuilder.h>

namespace Antares::Solver::Modeler::Api
{

LinearProblemBuilder::LinearProblemBuilder(const std::vector<std::shared_ptr<LinearProblemFiller>>& fillers):
        fillers_(fillers)
{
}

void LinearProblemBuilder::build()
{
    for (const auto filler : fillers_)
    {
        filler->addVariables();
        filler->addConstraints();
        filler->addObjective();
    }
}

} // namespace Antares::Solver::Modeler::Api
