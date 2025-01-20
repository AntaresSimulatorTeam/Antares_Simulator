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
    double getData(unsigned rank, unsigned hour) override;

private:
    unsigned height_ = 0;
    std::vector<std::vector<double>> tsSet_;
    std::string err_prefix_;
};

} // namespace Antares::Solver::Modeler::DataSeries
