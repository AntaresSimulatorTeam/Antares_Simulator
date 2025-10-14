#pragma once

#include "antares/solver/lps/LpsFromAntares.h"
#include "antares/solver/optimisation/HebdoProblemToLpsTranslator.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/study/study.h"

namespace Antares::Solver
{
class SingleProblemGetter
{
public:
    void load(const std::filesystem::path& study_path);

    ConstantDataFromAntares getConstantData();
    WeeklyDataFromAntares getWeeklyData(WeeklyProblemId id);

private:
    HebdoProblemToLpsTranslator translator_;
    std::unique_ptr<Antares::Data::Study> study_;
    PROBLEME_HEBDO pb_;
};
} // namespace Antares::Solver
