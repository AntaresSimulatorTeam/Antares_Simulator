// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <memory>

#include "antares/solver/lps/LpsFromAntares.h"
#include "antares/study/study.h"

namespace Antares::Solver::Implementation
{
class SingleProblemGetter;
}

namespace Antares::Solver
{

class SingleProblemGetter final
{
public:
    explicit SingleProblemGetter(const std::filesystem::path& studyPath);
    ~SingleProblemGetter();
    // NOTE week indices start at 1
    // year indices start at 0
    std::vector<WeeklyProblemId> getProblemIds() const;
    ConstantDataFromAntares getConstantData();
    // NOTE week indices start at 1
    // year indices start at 0
    WeeklyDataFromAntares getWeeklyData(WeeklyProblemId id);
    std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblem> getWeeklyProblem(
      WeeklyProblemId id);

    // TODO[FOM] This should not be necessary
    void writeNTCTimeSeries(const std::filesystem::path& outputDir);
    void writeStudyDescriptionFiles(const std::filesystem::path& outputDir);
    int nbYears() const;
    int nbWeeks() const;
    std::set<int> playedYears() const;

private:
    std::unique_ptr<Implementation::SingleProblemGetter> impl_;
};
} // namespace Antares::Solver
