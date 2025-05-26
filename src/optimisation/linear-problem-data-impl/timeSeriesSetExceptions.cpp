
#include <string>

#include "antares/optimisation/linear-problem-data-impl/timeSeriesSet.h"

namespace Antares::Optimisation::LinearProblemDataImpl
{

TimeSeriesSet::AddTSofWrongSize::AddTSofWrongSize(const std::string& name,
                                                  const size_t& tsSize,
                                                  const unsigned& height):
    std::invalid_argument("TS set '" + name + "' : add a TS of size " + std::to_string(tsSize)
                          + " in a set of height " + std::to_string(height))
{
}

TimeSeriesSet::Empty::Empty(const std::string& name):
    std::invalid_argument("TS set '" + name + "' : empty, requesting a value makes no sense")
{
}

TimeSeriesSet::RankTooBig::RankTooBig(const std::string& name, unsigned rank):
    std::invalid_argument("TS set '" + name + "' : rank " + std::to_string(rank)
                          + " exceeds TS set's width")
{
}

TimeSeriesSet::HourTooBig::HourTooBig(const std::string& name, unsigned int hour):
    std::invalid_argument("TS set '" + name + "' : hour " + std::to_string(hour)
                          + " exceeds TS set's height")
{
}

} // namespace Antares::Optimisation::LinearProblemDataImpl
