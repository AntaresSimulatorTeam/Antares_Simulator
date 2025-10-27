#include "antares/api/singleProblemGetter.h"

#include "singleProblemGetterImpl.h"

namespace Antares::Solver
{
SingleProblemGetter::SingleProblemGetter(std::unique_ptr<Antares::Data::Study>&& study):
    impl(std::make_unique<Implementation::SingleProblemGetter>(std::move(study)))
{
}

SingleProblemGetter::~SingleProblemGetter() = default;

ConstantDataFromAntares SingleProblemGetter::getConstantData()
{
    return impl->getConstantData();
}

WeeklyDataFromAntares SingleProblemGetter::getWeeklyData(WeeklyProblemId id)
{
    return impl->getWeeklyData(id);
}

} // namespace Antares::Solver
