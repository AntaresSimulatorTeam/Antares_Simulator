#include "antares/solver/modeler/dataSeries/timeSeriesSet.h"

#include <stdexcept>
#include <string>

namespace Antares::Solver::Modeler::DataSeries
{
TimeSeriesSet::TimeSeriesSet(std::string name, unsigned int height):
    name_(name),
    height_(height)
{
}

void TimeSeriesSet::add(std::vector<double> ts)
{
    tsSet_.push_back(std::move(ts));
}

double TimeSeriesSet::getData(unsigned int rank, unsigned int hour)
{
    if (rank > tsSet_.size())
    {
        std::string error_message = "Rank '" + std::to_string(rank) + "' exceeds size of TS '"
                                    + name_ + "'.";
        throw std::invalid_argument(error_message);
    }

    if (hour + 1 > height_)
    {
        std::string error_message = "Hour '" + std::to_string(hour) + "' exceeds height of TS '"
                                    + name_ + "'.";
        throw std::invalid_argument(error_message);
    }

    return tsSet_[rank][hour];
}
} // namespace Antares::Solver::Modeler::DataSeries
