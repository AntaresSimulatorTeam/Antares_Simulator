#pragma once

#include <antares/mersenne-twister/mersenne-twister.h>
#include <antares/solver/simulation/solver_utils.h>
#include <antares/study/study.h>

namespace Antares::Solver::Simulation
{
void allocateMemoryForRandomNumbers(const Antares::Data::Study& study,
                                    randomNumbers& randomForParallelYears);

void computeRandomNumbers(Antares::Data::Study& study,
                          randomNumbers& randomForYears,
                          unsigned years,
                          std::map<unsigned int, bool>& isYearPerformed,
                          MersenneTwister& randomHydro);
} // namespace Antares::Solver::Simulation
