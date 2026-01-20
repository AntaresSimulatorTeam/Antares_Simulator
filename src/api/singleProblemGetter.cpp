// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/api/singleProblemGetter.h"

#include "singleProblemGetterImpl.h"

namespace Antares::Solver
{
SingleProblemGetter::SingleProblemGetter(const std::filesystem::path& studyPath):
    impl_(std::make_unique<Implementation::SingleProblemGetter>(studyPath))
{
}

/* std::unique_ptr's destructor does not handle incomplete types
   Implementation::SingleProblemGetter is complete here */
SingleProblemGetter::~SingleProblemGetter() = default;

ConstantDataFromAntares SingleProblemGetter::getConstantData()
{
    return impl_->getConstantData();
}

WeeklyDataFromAntares SingleProblemGetter::getWeeklyData(WeeklyProblemId id)
{
    return impl_->getWeeklyData(id);
}

std::vector<WeeklyProblemId> SingleProblemGetter::getProblemIds() const
{
    return impl_->getProblemIds();
}

void SingleProblemGetter::writeNTCTimeSeries(const std::filesystem::path& outputDir)
{
    impl_->writeNTCTimeSeries(outputDir);
}

void SingleProblemGetter::writeStudyDescriptionFiles(const std::filesystem::path& outputDir)
{
    impl_->writeStudyDescriptionFiles(outputDir);
}

} // namespace Antares::Solver
