

#pragma once

#include "antares/solver/hydro/management/management.h"
#include "antares/solver/lps/LpsFromAntares.h"
#include "antares/solver/optimisation/HebdoProblemToLpsTranslator.h"
#include "antares/solver/simulation/random.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares::Solver::Implementation
{

using HydroLevels = std::map<const Antares::Data::Area*, std::array<double, 53 /* TODO */>>;

struct YearlyData
{
    HydroLevels hydroLevels;
    Antares::HYDRO_VENTILATION_RESULTS ventilationResults;
};

using AllData = std::map<unsigned int /* year */, YearlyData>;

class SingleProblemGetter
{
public:
    explicit SingleProblemGetter(std::unique_ptr<Antares::Data::Study>&& study);
    ConstantDataFromAntares getConstantData();
    WeeklyDataFromAntares getWeeklyData(WeeklyProblemId id);

private:
    const YearlyData& getYearlyData(unsigned year);
    YearlyData computeHydroLevels(unsigned year, const std::vector<double>& initialLevel);
    void initializeRandomNumbers();
    Antares::Data::Area::ScratchMap scratchmap_;
    HebdoProblemToLpsTranslator translator_;
    std::unique_ptr<Antares::Data::Study> study_;
    PROBLEME_HEBDO pb_;
    AllData allData_;
    std::optional<Antares::Solver::Simulation::randomNumbers>
      randomForParallelYears_; // Allow the use of std::optional<T>::emplace for delayed
                               // building
};
} // namespace Antares::Solver::Implementation
