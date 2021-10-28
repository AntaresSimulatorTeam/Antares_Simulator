#include <cassert>

#include "constraint.h"

namespace Antares
{
namespace Optimization
{
Constraint::Constraint(const std::string& input, double slack_value) :
 mInput(input), mSlackValue(slack_value)
{
}

std::size_t Constraint::extractItems()
{
    auto b = mInput.begin();
    auto e = mInput.end();
    size_t new_pos;
    for (size_t pos = 0; pos < mInput.size();)
    {
        new_pos = mInput.find("::", pos);
        if (new_pos == std::string::npos)
        {
            mItems.emplace_back(b + pos, e);
            break;
        }
        if (new_pos > pos)
            mItems.emplace_back(b + pos, b + new_pos);
        pos = new_pos + 2;
    }
    return mItems.size();
}

const std::string& Constraint::getInput() const
{
    return mInput;
}

double Constraint::getSlackValue() const
{
    return mSlackValue;
}

std::string Constraint::getAreaName() const
{
    if (getType() == ConstraintType::binding_constraint_hourly
        || getType() == ConstraintType::binding_constraint_daily
        || getType() == ConstraintType::binding_constraint_weekly)
        return "<none>";
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

ConstraintType Constraint::getType() const
{
    assert(mItems.size() > 1);
    if (mItems.at(0) == "bc")
    {
        if (mItems.at(1) == "hourly")
            return ConstraintType::binding_constraint_hourly;
        if (mItems.at(1) == "daily")
            return ConstraintType::binding_constraint_daily;
        if (mItems.at(1) == "weekly")
            return ConstraintType::binding_constraint_weekly;
    }
    if (mItems.at(0) == "fict_load")
        return ConstraintType::fictitious_load;
    if (mItems.at(0) == "hydro_level")
        return ConstraintType::hydro_reservoir_level;

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
        return "Hourly binding constraint '" + getBindingConstraintName() + "' at day "
               + getTimeStepInYear();
    case ConstraintType::binding_constraint_daily:
        return "Daily binding constraint '" + getBindingConstraintName() + "' at hour "
               + getTimeStepInYear();
    case ConstraintType::binding_constraint_weekly:
        return "Weekly binding constraint '" + getBindingConstraintName();

    case ConstraintType::fictitious_load:
      return "Last resort shedding status at area '" + getAreaName() + "' at hour " + getTimeStepInYear();
    case ConstraintType::hydro_reservoir_level:
        return "Hydro reservoir constraint at area '" + getAreaName() + "' at hour " + getTimeStepInYear();
    default:
        return "<unknown>";
    }
}
} // namespace Optimization
} // namespace Antares
