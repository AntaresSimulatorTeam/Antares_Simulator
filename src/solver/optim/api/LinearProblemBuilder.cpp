//
// Created by mitripet on 29/01/24.
//

#include "include/antares/optim/api/LinearProblemBuilder.h"

using namespace Antares::optim::api;

void LinearProblemBuilder::addFiller(LinearProblemFiller& filler)
{
    fillers_.push_back(&filler);
}

void LinearProblemBuilder::build(const LinearProblemData& data) {
    if (built) {
        // TODO
        throw;
    }
    for (auto filler : fillers_)
    {
        filler->addVariables(*linearProblem_, data);
    }
    for (auto filler : fillers_)
    {
        filler->addConstraints(*linearProblem_, data);
    }
    for (auto filler : fillers_)
    {
        filler->addObjective(*linearProblem_, data);
    }
    built = true;
}

void LinearProblemBuilder::update(const LinearProblemData& data) const {
    // TODO : throw if timestamps have changed ?
    if (!built) {
        // TODO
        throw;
    }
    for (auto filler : fillers_)
    {
        filler->update(*linearProblem_, data);
    }
}

MipSolution LinearProblemBuilder::solve()
{
    // TODO : move to new interface LinearProblemSolver ??
    if (!built) {
        // TODO
        throw;
    }
    return linearProblem_->solve();
}