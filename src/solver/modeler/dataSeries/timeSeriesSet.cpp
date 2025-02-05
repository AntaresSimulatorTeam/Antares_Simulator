#include "antares/solver/modeler/dataSeries/timeSeriesSet.h"

#include <stdexcept>
#include <string>

namespace Antares::Solver::Modeler::DataSeries
{
TimeSeriesSet::TimeSeriesSet(std::string name, unsigned int height):
    IDataSeries::IDataSeries(name),
    height_(height)
{
}

void TimeSeriesSet::add(const std::vector<double>& ts)
{
    if (ts.size() != height_)
    {
        throw AddTSofWrongSize(name(), ts.size(), height_);
    }
    tsSet_.push_back(std::move(ts));
}

double TimeSeriesSet::getData(unsigned rank, unsigned hour)
{
    if (tsSet_.empty())
    {
        throw Empty(name());
    }

    if (rank > tsSet_.size() - 1)
    {
        throw RankTooBig(name(), rank);
    }

    if (hour > height_ - 1)
    {
        throw HourTooBig(name(), hour);
    }

    return tsSet_[rank][hour];
}
} // namespace Antares::Solver::Modeler::DataSeries
