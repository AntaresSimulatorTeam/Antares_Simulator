
#include "antares/solver/infeasible-problem-analysis/watched-constraints.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/regex.hpp>

class StringIsNotWellFormated: public std::runtime_error
{
public:
    StringIsNotWellFormated(const std::string& error_message):
        std::runtime_error(error_message)
    {
    }
};

std::string StringBetweenAngleBrackets(const std::string& constraintName)
{
    std::vector<std::string> split_name;
    boost::split(split_name, constraintName, boost::is_any_of("<>"));

    std::string err_msg = "Error: ";
    if (split_name.size() < 3)
    {
        err_msg += "constraint name '" + constraintName + "' misses '<' and/or '>' bracket";
        throw StringIsNotWellFormated(err_msg);
    }
    if (split_name[1].empty())
    {
        err_msg += "constraint name '" + constraintName + "' must be of format '*<str>*'";
        throw StringIsNotWellFormated(err_msg);
    }
    return split_name[1];
}

std::string timeStep(std::vector<std::string> splitName)
{
    return StringBetweenAngleBrackets(splitName.at(splitName.size() - 2));
}

std::string shortName(std::vector<std::string> splitName)
{
    return splitName.at(0);
}

std::string areaName(std::vector<std::string> splitName)
{
    return StringBetweenAngleBrackets(splitName.at(1));
}

std::string STSname(std::vector<std::string> splitName)
{
    return StringBetweenAngleBrackets(splitName.at(2));
}

namespace Antares::Optimization
{

// =======================================
// Generic constraint logger
// =======================================
void WatchedConstraint::setConstraintName(std::string name)
{
    name_ = name;
    boost::algorithm::split_regex(splitName_, name_, boost::regex("::"));
}

std::string WatchedConstraint::regexId() const
{
    return regexId_;
}

void WatchedConstraint::setRegexId(std::string regexId)
{
    regexId_ = regexId;
}

const std::vector<std::string>& WatchedConstraint::splitName() const
{
    return splitName_;
}

// =======================================
// Hourly BC logger
// =======================================
HourlyBC::HourlyBC()
{
    setRegexId("::hourly::");
}

std::shared_ptr<WatchedConstraint> HourlyBC::clone() const
{
    return std::make_shared<HourlyBC>(*this);
}

std::string HourlyBC::infeasisibity()
{
    return "Hourly BC '" + shortName(splitName()) + "' at hour " + timeStep(splitName());
}

std::string HourlyBC::infeasisibityCause()
{
    return "* Hourly binding constraints.";
}

// =======================================
// Daily BC logger
// =======================================
DailyBC::DailyBC()
{
    setRegexId("::daily::");
}

std::shared_ptr<WatchedConstraint> DailyBC::clone() const
{
    return std::make_shared<DailyBC>(*this);
}

std::string DailyBC::infeasisibity()
{
    return "Daily BC '" + shortName(splitName()) + "' at day " + timeStep(splitName());
}

std::string DailyBC::infeasisibityCause()
{
    return "* Daily binding constraints,";
}

// =======================================
// Weekly BC constraint
// =======================================
WeeklyBC::WeeklyBC()
{
    setRegexId("::weekly::");
}

std::shared_ptr<WatchedConstraint> WeeklyBC::clone() const
{
    return std::make_shared<WeeklyBC>(*this);
}

std::string WeeklyBC::infeasisibity()
{
    return "Weekly BC '" + shortName(splitName());
}

std::string WeeklyBC::infeasisibityCause()
{
    return "* Weekly binding constraints.";
}

// =======================================
// Fictitious load constraint
// =======================================
FictitiousLoad::FictitiousLoad()
{
    setRegexId("^FictiveLoads::");
}

std::shared_ptr<WatchedConstraint> FictitiousLoad::clone() const
{
    return std::make_shared<FictitiousLoad>(*this);
}

std::string FictitiousLoad::infeasisibity()
{
    return "Last resort shedding status at area '" + areaName(splitName()) + "' at hour "
           + timeStep(splitName());
}

std::string FictitiousLoad::infeasisibityCause()
{
    return "* Last resort shedding status.";
}

// =======================================
// Hydro level constraint
// =======================================
HydroLevel::HydroLevel()
{
    setRegexId("^AreaHydroLevel::");
}

std::shared_ptr<WatchedConstraint> HydroLevel::clone() const
{
    return std::make_shared<HydroLevel>(*this);
}

std::string HydroLevel::infeasisibity()
{
    return "Hydro level constraint at area '" + areaName(splitName()) + "' at hour "
           + timeStep(splitName());
}

std::string HydroLevel::infeasisibityCause()
{
    return "* Hydro reservoir impossible to manage with cumulative options "
           "\"hard bounds without heuristic\"";
}

// =======================================
// Short term storage constraint
// =======================================
STS::STS()
{
    setRegexId("^Level::");
}

std::shared_ptr<WatchedConstraint> STS::clone() const
{
    return std::make_shared<STS>(*this);
}

std::string STS::infeasisibity()
{
    return "Short-term-storage reservoir constraint at area '" + areaName(splitName()) + "' in STS '"
           + STSname(splitName()) + "' at hour " + timeStep(splitName());
}

std::string STS::infeasisibityCause()
{
    return "* Short-term storage reservoir level impossible to manage. Please check inflows, "
           "lower & upper curves and initial level (if prescribed),";
}

// =======================================
// Hydro production constraint
// =======================================
HydroProduction::HydroProduction()
{
    setRegexId("^HydroPower::");
}

std::shared_ptr<WatchedConstraint> HydroProduction::clone() const
{
    return std::make_shared<HydroProduction>(*this);
}

std::string HydroProduction::infeasisibity()
{
    return "Hydro weekly production at area '" + areaName(splitName()) + "'";
}

std::string HydroProduction::infeasisibityCause()
{
    return "* impossible to generate exactly the weekly hydro target";
}
} // namespace Antares::Optimization
