#pragma once

#include "antares/solver/modeler/api/linearProblemFiller.h"

namespace Antares::Solver::Modeler::Api
{

class TwoVarsTwoConstraints: public LinearProblemFiller
{
    using LinearProblemFiller::LinearProblemFiller;

public:
    void addVariables() override;
    void addConstraints() override;
    void addObjective() override;
};

} // namespace Antares::Solver::Modeler::Api
