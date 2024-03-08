#pragma once

#include <antares/optim/api/LinearProblemData.h>
#include <antares/optim/api/LinearProblemFiller.h>

extern Antares::optim::api::LinearProblemData gLinearProblemData;
extern Antares::optim::api::MipSolution gMipSolution;
extern std::vector<std::shared_ptr<Antares::optim::api::LinearProblemFiller>> gAdditionalFillers;
