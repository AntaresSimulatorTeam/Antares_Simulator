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
using HydroLevels = std::map<const Antares::Data::Area*, std::array<double, 53 /* TODO */>>;

struct YearlyData
{
    HydroLevels hydroLevels;
    Antares::HYDRO_VENTILATION_RESULTS ventilationResults;
};

using AllData = std::map<unsigned int /* year */, YearlyData>;
} // namespace Details

namespace Antares::Solver
{
class SingleProblemGetter
{
public:
    explicit SingleProblemGetter(std::unique_ptr<Antares::Data::Study>&& study);

    ConstantDataFromAntares getConstantData();
    WeeklyDataFromAntares getWeeklyData(WeeklyProblemId id);

private:
    const Details::YearlyData& getYearlyData(unsigned year);
    Details::YearlyData computeHydroLevels(unsigned year, const std::vector<double>& initialLevel);
    void initializeRandomNumbers();
    Antares::Data::Area::ScratchMap scratchmap_;
    HebdoProblemToLpsTranslator translator_;
    std::unique_ptr<Antares::Data::Study> study_;
    PROBLEME_HEBDO pb_;
    Details::AllData allData_;
    std::optional<Antares::Solver::Simulation::randomNumbers>
      randomForParallelYears_; // Allow the use of std::optional<T>::emplace for delayed building
};
} // namespace Antares::Solver
