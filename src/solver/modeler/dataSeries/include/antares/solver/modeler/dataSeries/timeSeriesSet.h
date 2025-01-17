#include <string>
#include <vector>

#include "dataSeries.h"

namespace Antares::Solver::Modeler::DataSeries
{
class TimeSeriesSet: public IDataSeries
{
public:
    explicit TimeSeriesSet(std::string name, unsigned height);
    void add(std::vector<double> ts);
    double getData(unsigned int rank, unsigned int hour) override;

private:
    std::string name_;
    unsigned height_ = 0;
    std::vector<std::vector<double>> tsSet_;
};

} // namespace Antares::Solver::Modeler::DataSeries
