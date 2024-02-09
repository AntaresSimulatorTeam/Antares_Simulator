#include "opt_global.h"

// TODO one object per thread (numSpace)
Antares::optim::api::LinearProblemData gLinearProblemData({}, 0, {}, {});
std::vector<Antares::optim::api::LinearProblemFiller*> gAdditionalFillers;
