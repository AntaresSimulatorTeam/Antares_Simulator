
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
WatchedConstraint::WatchedConstraint(std::string name):
    constraintName_(name)
{
    boost::algorithm::split_regex(splitName_, constraintName_, boost::regex("::"));
}

const std::vector<std::string>& WatchedConstraint::splitName() const
{
    return splitName_;
}

// =======================================
// Hourly BC logger
// =======================================
HourlyBC::HourlyBC(std::string constraintName):
    WatchedConstraint(constraintName)
{
}

std::string HourlyBC::infeasibility()
{
    return "Hourly BC '" + shortName(splitName()) + "' at hour " + timeStep(splitName());
}

std::string HourlyBC::infeasibilityCause()
{
    return "* Hourly binding constraints.";
}

// =======================================
// Daily BC logger
// =======================================
DailyBC::DailyBC(std::string constraintName):
    WatchedConstraint(constraintName)
{
}

std::string DailyBC::infeasibility()
{
    return "Daily BC '" + shortName(splitName()) + "' at day " + timeStep(splitName());
}

std::string DailyBC::infeasibilityCause()
{
    return "* Daily binding constraints,";
}

// =======================================
// Weekly BC constraint
// =======================================
WeeklyBC::WeeklyBC(std::string constraintName):
    WatchedConstraint(constraintName)
{
}

std::string WeeklyBC::infeasibility()
{
    return "Weekly BC '" + shortName(splitName());
}

std::string WeeklyBC::infeasibilityCause()
{
    return "* Weekly binding constraints.";
}

// =======================================
// Fictitious load constraint
// =======================================
FictitiousLoad::FictitiousLoad(std::string constraintName):
    WatchedConstraint(constraintName)
{
}

std::string FictitiousLoad::infeasibility()
{
    return "Last resort shedding status at area '" + areaName(splitName()) + "' at hour "
           + timeStep(splitName());
}

std::string FictitiousLoad::infeasibilityCause()
{
    return "* Last resort shedding status.";
}

// =======================================
// Hydro level constraint
// =======================================
HydroLevel::HydroLevel(std::string constraintName):
    WatchedConstraint(constraintName)
{
}

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

// =======================================
// Short term storage constraint
// =======================================
STS::STS(std::string constraintName):
    WatchedConstraint(constraintName)
{
}

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

// =======================================
// Hydro production constraint
// =======================================
HydroProduction::HydroProduction(std::string constraintName):
    WatchedConstraint(constraintName)
{
}

std::string HydroProduction::infeasibility()
{
    return "Hydro weekly production at area '" + areaName(splitName()) + "'";
}

std::string HydroProduction::infeasibilityCause()
{
    return "* impossible to generate exactly the weekly hydro target";
}

// =======================================
// Constraints factory
// =======================================
std::shared_ptr<WatchedConstraint> createHourlyBC(std::string varName)
{
    return std::make_shared<HourlyBC>(varName);
}

std::shared_ptr<WatchedConstraint> createDailyBC(std::string varName)
{
    return std::make_shared<DailyBC>(varName);
}

std::shared_ptr<WatchedConstraint> createWeeklyBC(std::string varName)
{
    return std::make_shared<WeeklyBC>(varName);
}

std::shared_ptr<WatchedConstraint> createFictitiousLoad(std::string varName)
{
    return std::make_shared<FictitiousLoad>(varName);
}

std::shared_ptr<WatchedConstraint> createHydroLevel(std::string varName)
{
    return std::make_shared<HydroLevel>(varName);
}

std::shared_ptr<WatchedConstraint> createSTS(std::string varName)
{
    return std::make_shared<STS>(varName);
}

std::shared_ptr<WatchedConstraint> createHydroProduction(std::string varName)
{
    return std::make_shared<HydroProduction>(varName);
}

ConstraintsFactory::ConstraintsFactory()
{
    regex_to_constraints_["::hourly::"] = createHourlyBC;
    regex_to_constraints_["::daily::"] = createDailyBC;
    regex_to_constraints_["::weekly::"] = createWeeklyBC;
    regex_to_constraints_["^FictiveLoads::"] = createFictitiousLoad;
    regex_to_constraints_["^AreaHydroLevel::"] = createHydroLevel;
    regex_to_constraints_["^Level::"] = createSTS;
    regex_to_constraints_["^HydroPower::"] = createHydroProduction;

    auto keyView = std::views::keys(regex_to_constraints_);
    regex_ids_ = {keyView.begin(), keyView.end()};
}

std::shared_ptr<WatchedConstraint> ConstraintsFactory::create(std::string varName) const
{
    for (auto& [pattern, createFunction]: regex_to_constraints_)
    {
        if (std::regex_search(varName, std::regex(pattern)))
        {
            return createFunction(varName);
        }
    }
    return nullptr; // Cannot happen
}

std::regex ConstraintsFactory::constraintsFilter()
{
    return std::regex(boost::algorithm::join(regex_ids_, "|"));
}

} // namespace Antares::Optimization
