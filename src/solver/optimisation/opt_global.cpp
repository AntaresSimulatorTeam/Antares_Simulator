#include "opt_global.h"

// TODO one object per thread (numSpace)
// TODO eliminate global variables
Antares::optim::api::LinearProblemData gLinearProblemData({}, 0, {}, {});
Antares::optim::api::MipSolution gMipSolution(operations_research::MPSolver::NOT_SOLVED, {});
std::vector<Antares::optim::api::LinearProblemFiller*> gAdditionalFillers;
