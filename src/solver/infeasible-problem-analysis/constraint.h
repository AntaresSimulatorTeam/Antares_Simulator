#pragma once

#include <vector>
#include <string>

namespace Antares
{
namespace Optimization
{
enum class ConstraintType
{
    binding_constraint_hourly,
    binding_constraint_daily,
    binding_constraint_weekly,
    fictitious_load,
    hydro_reservoir_level,
    none
};

class Constraint
{
public:
    // Construct object
    Constraint() = default;
    Constraint(const std::string& input, double slack_value);

    // Raw members
    const std::string& getInput() const;
    double getSlackValue() const;

    // Extract items, check consistency
    std::size_t extractItems();
    std::string prettyPrint() const;
    ConstraintType getType() const;

private:
    std::string mInput;
    std::vector<std::string> mItems;
    double mSlackValue;

    // Get specific items
    std::string getAreaName() const;
    std::string getTimeStepInYear() const;
    std::string getBindingConstraintName() const;
};
} // namespace Optimization
} // namespace Antares
