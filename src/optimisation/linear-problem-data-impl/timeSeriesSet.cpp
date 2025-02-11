#include "antares/optimisation/linear-problem-data-impl/timeSeriesSet.h"

#include <stdexcept>
#include <string>

namespace Antares::Optimisation::LinearProblemDataImpl
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
    if (hour > height_ - 1)
    {
        throw HourTooBig(name(), hour);
    }

    return getData(rank)[hour];
}

std::vector<double> TimeSeriesSet::getData(unsigned rank)
{
    if (tsSet_.empty())
    {
        throw Empty(name());
    }

    if (rank > tsSet_.size() - 1)
    {
        throw RankTooBig(name(), rank);
    }

    return tsSet_[rank];
}
} // namespace Antares::Optimisation::LinearProblemDataImpl
