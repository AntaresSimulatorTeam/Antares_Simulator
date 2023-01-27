#include <cassert>

#include "constraint.h"

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

std::string Constraint::getAreaName() const
{
    if ((getType() == ConstraintType::binding_constraint_hourly)
        || (getType() == ConstraintType::binding_constraint_daily)
        || (getType() == ConstraintType::binding_constraint_weekly))
    {
        return "<none>";
    }
    return mItems.at(2);
}

std::string Constraint::getTimeStepInYear() const
{
    switch (getType())
    {
    case ConstraintType::binding_constraint_hourly:
    case ConstraintType::binding_constraint_daily:
        return mItems.at(2);
    case ConstraintType::fictitious_load:
    case ConstraintType::hydro_reservoir_level:
        return mItems.at(1);
    default:
        return "-1";
    }
}

static ConstraintType bindingConstraintPeriodicity(const std::string& in)
{
    if (in == "hourly")
    {
        return ConstraintType::binding_constraint_hourly;
    }
    if (in == "daily")
    {
        return ConstraintType::binding_constraint_daily;
    }
    if (in == "weekly")
    {
        return ConstraintType::binding_constraint_weekly;
    }
    return ConstraintType::none;
}

ConstraintType Constraint::getType() const
{
    assert(mItems.size() > 1);
    if (mItems.at(0) == "bc")
    {
        return bindingConstraintPeriodicity(mItems.at(1));
    }
    if (mItems.at(0) == "fict_load")
    {
        return ConstraintType::fictitious_load;
    }
    if (mItems.at(0) == "hydro_level")
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
        return mItems.at(3);
    case ConstraintType::binding_constraint_weekly:
        return mItems.at(2);
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
