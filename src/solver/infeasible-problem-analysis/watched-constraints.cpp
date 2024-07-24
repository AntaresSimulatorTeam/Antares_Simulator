#include "antares/solver/infeasible-problem-analysis/watched-constraints.h"

#include <ranges>

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
    return StringBetweenAngleBrackets(splitName.rbegin()[1]);
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

// --- Generic constraint ---
WatchedConstraint::WatchedConstraint(const std::string& name, const double slackValue):
    slack_value_(slackValue)
{
    boost::algorithm::split_regex(splitName_, name, boost::regex("::"));
}

const std::vector<std::string>& WatchedConstraint::splitName() const
{
    return splitName_;
}

double WatchedConstraint::slackValue() const
{
    return slack_value_;
}

std::string HourlyBC::infeasibility()
{
    return "Hourly BC '" + shortName(splitName()) + "' at hour " + timeStep(splitName());
}

std::string HourlyBC::infeasibilityCause()
{
    return "* Hourly binding constraints.";
}

// --- Daily BC constraint ---
std::string DailyBC::infeasibility()
{
    return "Daily BC '" + shortName(splitName()) + "' at day " + timeStep(splitName());
}

std::string DailyBC::infeasibilityCause()
{
    return "* Daily binding constraints,";
}

// --- Weekly BC constraint ---
std::string WeeklyBC::infeasibility()
{
    return "Weekly BC '" + shortName(splitName());
}

std::string WeeklyBC::infeasibilityCause()
{
    return "* Weekly binding constraints.";
}

// --- Fictitious load constraint ---
std::string FictitiousLoad::infeasibility()
{
    return "Last resort shedding status at area '" + areaName(splitName()) + "' at hour "
           + timeStep(splitName());
}

std::string FictitiousLoad::infeasibilityCause()
{
    return "* Last resort shedding status.";
}

// --- Hydro level constraint ---
std::string HydroLevel::infeasibility()
{
    return "Hydro level constraint at area '" + areaName(splitName()) + "' at hour "
           + timeStep(splitName());
}

std::string HydroLevel::infeasibilityCause()
{
    return "* Hydro reservoir impossible to manage with cumulative options "
           "\"hard bounds without heuristic\"";
}

// --- Short term storage constraint ---
std::string STS::infeasibility()
{
    return "Short-term-storage reservoir constraint at area '" + areaName(splitName())
           + "' in STS '" + STSname(splitName()) + "' at hour " + timeStep(splitName());
}

std::string STS::infeasibilityCause()
{
    return "* Short-term storage reservoir level impossible to manage. Please check inflows, "
           "lower & upper curves and initial level (if prescribed),";
}

// --- Hydro production constraint ---
std::string HydroProduction::infeasibility()
{
    return "Hydro weekly production at area '" + areaName(splitName()) + "'";
}

std::string HydroProduction::infeasibilityCause()
{
    return "* impossible to generate exactly the weekly hydro target";
}

// --- Constraints factory ---
ConstraintsFactory::ConstraintsFactory()
{
    regex_to_ctypes_ = {
      {"::hourly::", std::make_unique<HourlyBC, const std::string&, const double>},
      {"::daily::", std::make_unique<DailyBC, const std::string&, const double>},
      {"::weekly::", std::make_unique<WeeklyBC, const std::string&, const double>},
      {"^FictiveLoads::", std::make_unique<FictitiousLoad, const std::string&, const double>},
      {"^AreaHydroLevel::", std::make_unique<HydroLevel, const std::string&, const double>},
      {"^Level::", std::make_unique<STS, const std::string&, const double>},
      {"^HydroPower::", std::make_unique<HydroProduction, const std::string&, const double>}};
}

std::unique_ptr<WatchedConstraint> ConstraintsFactory::create(const std::string& name,
                                                              const double value) const
{
    auto it = std::ranges::find_if(regex_to_ctypes_,
                                   [&name](auto& pair)
                                   { return std::regex_search(name, std::regex(pair.first)); });
    if (it != regex_to_ctypes_.end())
    {
        return it->second(name, value);
    }
    return nullptr;
}

std::regex ConstraintsFactory::constraintsFilter()
{
    auto keyView = std::views::keys(regex_to_ctypes_);
    std::vector<std::string> regex_ids = {keyView.begin(), keyView.end()};
    return std::regex(boost::algorithm::join(regex_ids, "|"));
}

} // namespace Antares::Optimization
