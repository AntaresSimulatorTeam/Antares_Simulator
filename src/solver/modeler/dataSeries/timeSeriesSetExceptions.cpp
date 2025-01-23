
#include "antares/solver/modeler/dataSeries/timeSeriesSetExceptions.h"

#include <string>

namespace Antares::Solver::Modeler::DataSeries
{

TSset_AddTSofWrongSize::TSset_AddTSofWrongSize(const std::string& name,
                                               const size_t& tsSize,
                                               const unsigned& height):
    std::invalid_argument("TS set '" + name + "' : add a TS of size " + std::to_string(tsSize)
                          + " in a set of height " + std::to_string(height))
{
}

TSset_Empty::TSset_Empty(const std::string& name):
    std::invalid_argument("TS set '" + name + "' : empty, requesting a value makes no sense")
{
}

TSset_RankTooBig::TSset_RankTooBig(const std::string& name, unsigned rank):
    std::invalid_argument("TS set '" + name + "' : rank " + std::to_string(rank)
                          + " exceeds TS set's width")
{
}

TSset_HourTooBig::TSset_HourTooBig(const std::string& name, unsigned int hour):
    std::invalid_argument("TS set '" + name + "' : hour " + std::to_string(hour)
                          + " exceeds TS set's height")
{
}

} // namespace Antares::Solver::Modeler::DataSeries
