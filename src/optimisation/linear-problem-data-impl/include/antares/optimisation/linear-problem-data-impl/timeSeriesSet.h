#include <stdexcept>
#include <string>
#include <vector>

#include "dataSeries.h"

namespace Antares::Optimisation::LinearProblemDataImpl
{
class TimeSeriesSet: public IDataSeries
{
public:
    explicit TimeSeriesSet(std::string name, unsigned height);
    void add(const std::vector<double>& ts);
    double getData(unsigned rank, unsigned hour) override;

private:
    unsigned height_ = 0;
    std::vector<std::vector<double>> tsSet_;

public:
    class AddTSofWrongSize: public std::invalid_argument
    {
    public:
        explicit AddTSofWrongSize(const std::string& name,
                                  const size_t& tsSize,
                                  const unsigned& height);
    };

    class Empty: public std::invalid_argument
    {
    public:
        explicit Empty(const std::string& name);
    };

    class RankTooBig: public std::invalid_argument
    {
    public:
        explicit RankTooBig(const std::string& name, unsigned rank);
    };

    class HourTooBig: public std::invalid_argument
    {
    public:
        explicit HourTooBig(const std::string& name, unsigned hour);
    };
};

} // namespace Antares::Optimisation::LinearProblemDataImpl
