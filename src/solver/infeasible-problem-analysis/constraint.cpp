#include <cassert>

#include "constraint.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace Antares
{
namespace Optimization
{
Constraint::Constraint(const std::string& input, const double slackValue) :
 mInput(input), mSlackValue(slackValue)
{
}

std::size_t Constraint::extractItems()
{
    const auto beg = mInput.begin();
    const auto end = mInput.end();
    std::size_t newPos = 0;
    const std::size_t sepSize = 2;
    const std::size_t inputSize = mInput.size();
    for (std::size_t pos = 0; pos < inputSize; pos = newPos + sepSize)
    {
        newPos = mInput.find("::", pos);
        if (newPos == std::string::npos)
        {
            mItems.emplace_back(beg + pos, end);
            break;
        }
        if (newPos > pos)
        {
            mItems.emplace_back(beg + pos, beg + newPos);
        }
    }
    return mItems.size();
}

double Constraint::getSlackValue() const
{
    return mSlackValue;
}

class StringIsNotWellFormated : public std::runtime_error
{
public:
    StringIsNotWellFormated(const std::string& error_message) : std::runtime_error(error_message)
    {
    }
};

std::string StringBetweenAngleBrackets(const std::string& str)
{
    const auto& str_begin_iterator = str.begin();
    const auto& str_end_iterator = str.end();
    auto left = std::find(str.begin(), str.end(), '<');
    if (left == str_end_iterator)
    {
        std::ostringstream stream;
        stream << std::string("Error the string: ") << std::quoted(str)
               << " does not contains the left angle bracket " << std::quoted("<");
        throw StringIsNotWellFormated(stream.str());
    }

    auto right = std::find(str_begin_iterator, str_end_iterator, '>');
    if (right == str_end_iterator)
    {
        std::ostringstream stream;
        stream << std::string("Error the string: ") << std::quoted(str)
               << " does not contains the right angle bracket " << std::quoted(">");
        throw StringIsNotWellFormated(stream.str());
    }

    if (std::distance(left, right) <= 1)
    {
        std::ostringstream stream;
        stream << std::string("Error the string: ") << std::quoted(str) << " must be of format  "
               << std::quoted("*<str>*");
        throw StringIsNotWellFormated(stream.str());
    }
    return std::string(left + 1, right);
}

std::string Constraint::getAreaName() const
{
    if ((getType() == ConstraintType::binding_constraint_hourly)
        || (getType() == ConstraintType::binding_constraint_daily)
        || (getType() == ConstraintType::binding_constraint_weekly))
    {
        return "<none>";
    }
    return StringBetweenAngleBrackets(mItems.at(1));
}

std::string Constraint::getTimeStepInYear() const
{
    switch (getType())
    {
    case ConstraintType::binding_constraint_hourly:
    case ConstraintType::binding_constraint_daily:
    case ConstraintType::fictitious_load:
    case ConstraintType::hydro_reservoir_level:
        return StringBetweenAngleBrackets (mItems.at(mItems.size()-2));
    default:
        return "-1";
    }
}

ConstraintType Constraint::getType() const
{
    assert(mItems.size() > 1);
    if (mItems.at(1) == "hourly")
    {
        return ConstraintType::binding_constraint_hourly;
    }
    if (mItems.at(1) == "daily")
    {
        return ConstraintType::binding_constraint_daily;
    }
    if (mItems.at(1) == "weekly")
    {
        return ConstraintType::binding_constraint_weekly;
    }
    if (mItems.at(0) == "FictiveLoads")
    {
        return ConstraintType::fictitious_load;
    }
    if (mItems.at(0) == "AreaHydroLevel")
    {
        return ConstraintType::hydro_reservoir_level;
    }
    return ConstraintType::none;
}

std::string Constraint::getBindingConstraintName() const
{
    switch (getType())
    {
    case ConstraintType::binding_constraint_hourly:
    case ConstraintType::binding_constraint_daily:
    case ConstraintType::binding_constraint_weekly:
        return mItems.at(0);
    default:
        return "<unknown>";
    }
}

std::string Constraint::prettyPrint() const
{
    switch (getType())
    {
    case ConstraintType::binding_constraint_hourly:
        return "Hourly binding constraint '" + getBindingConstraintName() + "' at hour "
               + getTimeStepInYear();
    case ConstraintType::binding_constraint_daily:
        return "Daily binding constraint '" + getBindingConstraintName() + "' at day "
               + getTimeStepInYear();
    case ConstraintType::binding_constraint_weekly:
        return "Weekly binding constraint '" + getBindingConstraintName();

    case ConstraintType::fictitious_load:
        return "Last resort shedding status at area '" + getAreaName() + "' at hour "
               + getTimeStepInYear();
    case ConstraintType::hydro_reservoir_level:
        return "Hydro reservoir constraint at area '" + getAreaName() + "' at hour "
               + getTimeStepInYear();
    default:
        return "<unknown>";
    }
}
} // namespace Optimization
} // namespace Antares
