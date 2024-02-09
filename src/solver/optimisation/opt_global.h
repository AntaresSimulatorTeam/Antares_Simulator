#pragma once

#include <antares/optim/api/LinearProblemData.h>
#include <antares/optim/api/LinearProblemFiller.h>

extern Antares::optim::api::LinearProblemData gLinearProblemData;
extern std::vector<Antares::optim::api::LinearProblemFiller*> gAdditionalFillers;
