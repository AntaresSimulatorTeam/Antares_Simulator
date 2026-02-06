
#pragma once

#include "antares/solver/hydro/management/management.h"
#include "antares/solver/lps/LpsFromAntares.h"
#include "antares/solver/optim-model-filler/BendersDecomposition.h"
#include "antares/solver/optimisation/HebdoProblemToLpsTranslator.h"
#include "antares/solver/simulation/random.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares::Solver::Implementation
{

using HydroLevels = std::map<const Antares::Data::Area*, std::array<double, 54 /* TODO */>>;

struct YearlyData
{
    HydroLevels hydroLevels;
    Antares::HYDRO_VENTILATION_RESULTS ventilationResults;
};

using AllData = std::map<unsigned int /* year */, YearlyData>;

struct NameMemo
{
    std::size_t left_end;    // index where the number starts
    std::size_t right_begin; // index of '>' (first char of right part)
    int baseTime;
    std::size_t index; // index in names vector
    unsigned base;     // 168 -> hour, 7 -> day, 1 -> week
};

class SingleProblemGetter final
{
public:
    explicit SingleProblemGetter(const std::filesystem::path& studyPath);
    explicit SingleProblemGetter(std::unique_ptr<Antares::Data::Study>&& study);
    ConstantDataFromAntares getConstantData();
    WeeklyDataFromAntares getWeeklyData(WeeklyProblemId id);
    std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblem> getWeeklyProblem(
      WeeklyProblemId id);
    std::vector<WeeklyProblemId> getProblemIds() const;

    void writeNTCTimeSeries(const std::filesystem::path& outputDir);
    void writeStudyDescriptionFiles(const std::filesystem::path& outputDir);
    int nbYears() const;
    int nbWeeks() const;
    bool areWeeksIndependent() const;
    std::set<int> playedYears() const;
    void setBendersDecomposition(Optimisation::BendersDecomposition* bd);
    ModelerData* modelerData();

private:
    const YearlyData& getYearlyData(unsigned year);
    YearlyData computeHydroLevels(unsigned year, const std::vector<double>& initialLevel);
    void initializeRandomNumbers();
    void fillProblem(Optimisation::LinearProblemApi::ILinearProblem& problem) const;
    void setWeeklyData(WeeklyProblemId& id);
    Antares::Data::Area::ScratchMap scratchmap_;
    HebdoProblemToLpsTranslator translator_;
    std::unique_ptr<Antares::Data::Study> study_;
    PROBLEME_HEBDO pb_;
    AllData allData_;
    std::optional<Antares::Solver::Simulation::randomNumbers>
      randomForParallelYears_; // Allow the use of std::optional<T>::emplace for delayed
                               // building
    std::set<int> playedYears_;
    int nbWeeks_;
    Optimisation::BendersDecomposition* bendersDecomposition_ = nullptr;

    std::vector<std::string> variablesName_;
    std::vector<std::string> constraintsName_;
    std::vector<NameMemo> variablesMemo_;
    std::vector<NameMemo> constraintsMemo_;
    std::set<int> randomPrepared_;
};
} // namespace Antares::Solver::Implementation
