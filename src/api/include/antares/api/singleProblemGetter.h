#pragma once

#include <memory>

#include "antares/solver/lps/LpsFromAntares.h"
#include "antares/study/study.h"

namespace Antares::Solver::Implementation
{
class SingleProblemGetter;
}

namespace Antares::Solver
{
class SingleProblemGetter
{
public:
    explicit SingleProblemGetter(std::unique_ptr<Antares::Data::Study>&& study);
    ~SingleProblemGetter();
    ConstantDataFromAntares getConstantData();
    WeeklyDataFromAntares getWeeklyData(WeeklyProblemId id);

private:
    std::unique_ptr<class Implementation::SingleProblemGetter> impl;
};
} // namespace Antares::Solver
