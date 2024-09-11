#pragma once

#include "antares/solver/modeler/api/linearProblemFiller.h"

namespace Antares::Solver::Modeler::Api
{

class OneVarFiller : public LinearProblemFiller
{
    using LinearProblemFiller::LinearProblemFiller;
public:
    void addVariables() override;
    void addConstraints() override;
    void addObjective() override;
};

}