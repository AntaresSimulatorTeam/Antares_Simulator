#include <stdexcept>

namespace Antares::Solver::Modeler::DataSeries
{
class TSset_AddTSofWrongSize: public std::invalid_argument
{
public:
    explicit TSset_AddTSofWrongSize(const std::string& name,
                                    const size_t& tsSize,
                                    const unsigned& height);
};

class TSset_Empty: public std::invalid_argument
{
public:
    explicit TSset_Empty(const std::string& name);
};

class TSset_RankTooBig: public std::invalid_argument
{
public:
    explicit TSset_RankTooBig(const std::string& name, unsigned rank);
};

class TSset_HourTooBig: public std::invalid_argument
{
public:
    explicit TSset_HourTooBig(const std::string& name, unsigned hour);
};

} // namespace Antares::Solver::Modeler::DataSeries
