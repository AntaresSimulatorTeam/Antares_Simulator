#pragma once

#include <array>
#include <map>
#include <optional>

#include "antares/solver/hydro/management/management.h"
#include "antares/solver/lps/LpsFromAntares.h"
#include "antares/solver/optimisation/HebdoProblemToLpsTranslator.h"
#include "antares/solver/simulation/random.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/study/study.h"

// TODO split header
namespace Details
{
struct YearlyData
{
    std::map<const Antares::Data::Area*, std::array<double, 53 /* TODO */>> hydroLevels;
    std::optional<Antares::Solver::Simulation::randomNumbers>
      randomForParallelYears; // Allow the use of std::optional<T>::emplace for delayed building
    Antares::HYDRO_VENTILATION_RESULTS ventilationResults;
};

using AllData = std::map<unsigned int /* year */, YearlyData>;
} // namespace Details

namespace Antares::Solver
{
class SingleProblemGetter
{
public:
    void load(const std::filesystem::path& study_path);

    ConstantDataFromAntares getConstantData();
    WeeklyDataFromAntares getWeeklyData(WeeklyProblemId id);

private:
    Details::YearlyData getYearlyData(unsigned year);

    HebdoProblemToLpsTranslator translator_;
    std::unique_ptr<Antares::Data::Study> study_;
    PROBLEME_HEBDO pb_;
    Details::AllData allData_;
};
} // namespace Antares::Solver
