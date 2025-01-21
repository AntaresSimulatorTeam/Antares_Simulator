#include "antares/solver/modeler/dataSeries/timeSeriesSet.h"

#include <stdexcept>
#include <string>

namespace Antares::Solver::Modeler::DataSeries
{
TimeSeriesSet::TimeSeriesSet(std::string name, unsigned int height):
    IDataSeries::IDataSeries(name),
    height_(height),
    err_prefix_("TS set '" + this->name() + "' : ")
{
}

void TimeSeriesSet::add(std::vector<double> ts)
{
    if (ts.size() != height_)
    {
        std::string error_message = err_prefix_ + "add a TS of size " + std::to_string(ts.size())
                                    + " in a set of height " + std::to_string(height_);
        throw std::invalid_argument(error_message);
    }
    tsSet_.push_back(std::move(ts));
}

double TimeSeriesSet::getData(unsigned rank, unsigned hour)
{
    if (tsSet_.empty())
    {
        std::string error_message = err_prefix_ + "empty, requesting a value makes no sense";
        throw std::invalid_argument(error_message);
    }

    if (rank > tsSet_.size() - 1)
    {
        std::string error_message = err_prefix_ + "rank " + std::to_string(rank)
                                    + " exceeds TS set's width";
        throw std::invalid_argument(error_message);
    }

    if (hour > height_ - 1)
    {
        std::string error_message = err_prefix_ + "hour " + std::to_string(hour)
                                    + " exceeds TS set's height";
        throw std::invalid_argument(error_message);
    }

    return tsSet_[rank][hour];
}
} // namespace Antares::Solver::Modeler::DataSeries
