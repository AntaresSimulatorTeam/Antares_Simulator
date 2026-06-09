
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/solver/hydro/management/management.h"
#include "antares/solver/lps/LpsFromAntares.h"
#include "antares/solver/modeler/Modeler.h"
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
    explicit SingleProblemGetter(
      std::pair<std::unique_ptr<Data::Study>, Solver::IResultWriter::Ptr>&& loadedPair);
    ConstantDataFromAntares getConstantData() const;
    WeeklyDataFromAntares getWeeklyData(WeeklyProblemId id);
    std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblem> getWeeklyProblem(
      WeeklyProblemId id);
    std::vector<WeeklyProblemId> getProblemIds() const;

    void writeNTCTimeSeries(const std::filesystem::path& outputDir);
    void writeStudyDescriptionFiles(const std::filesystem::path& outputDir);
    int nbYears() const;
    int nbWeeks() const;
    bool areWeeksIndependent() const;
    Solver::ProblemEntity getMasterProblem() const;
    void writeMasterAndStructure() const;
    void printProblems();
    std::set<int> playedYears() const;

private:
    void initConstantData();

    const YearlyData& getYearlyData(unsigned year);
    YearlyData computeHydroLevels(unsigned year, const std::vector<double>& initialLevel);
    void initializeRandomNumbers();
    void fillProblem(Optimisation::LinearProblemApi::ILinearProblem& problem,
                     const WeeklyProblemId& id);
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

    std::vector<std::string> variablesName_;
    std::vector<std::string> constraintsName_;
    std::vector<NameMemo> variablesMemo_;
    std::vector<NameMemo> constraintsMemo_;
    std::set<int> randomPrepared_;
    std::shared_ptr<IResultWriter> resultWriter_;
};
} // namespace Antares::Solver::Implementation
