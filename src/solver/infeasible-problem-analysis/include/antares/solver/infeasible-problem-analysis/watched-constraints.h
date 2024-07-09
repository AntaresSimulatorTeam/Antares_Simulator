
#pragma once

#include <functional>
#include <map>
#include <memory>
#include <regex>
#include <string>
#include <vector>

namespace Antares::Optimization
{
class WatchedConstraint
{
public:
    WatchedConstraint(std::string constraintName);
    virtual std::string infeasibility() = 0;
    virtual std::string infeasibilityCause() = 0;

protected:
    const std::vector<std::string>& splitName() const;

protected:
    std::string constraintName_;
    std::vector<std::string> splitName_;
};

class HourlyBC: public WatchedConstraint
{
    using WatchedConstraint::WatchedConstraint;
public:
    std::string infeasibility() override;
    std::string infeasibilityCause() override;
};

class DailyBC: public WatchedConstraint
{
    using WatchedConstraint::WatchedConstraint;
public:
    std::string infeasibility() override;
    std::string infeasibilityCause() override;
};

class WeeklyBC: public WatchedConstraint
{
    using WatchedConstraint::WatchedConstraint;
public:
    std::string infeasibility() override;
    std::string infeasibilityCause() override;
};

class FictitiousLoad: public WatchedConstraint
{
    using WatchedConstraint::WatchedConstraint;
public:
    std::string infeasibility() override;
    std::string infeasibilityCause() override;
};

class HydroLevel: public WatchedConstraint
{
    using WatchedConstraint::WatchedConstraint;
public:
    std::string infeasibility() override;
    std::string infeasibilityCause() override;
};

class STS: public WatchedConstraint
{
    using WatchedConstraint::WatchedConstraint;
public:
    std::string infeasibility() override;
    std::string infeasibilityCause() override;
};

class HydroProduction: public WatchedConstraint
{
    using WatchedConstraint::WatchedConstraint;
public:
    std::string infeasibility() override;
    std::string infeasibilityCause() override;
};

class ConstraintsFactory
{
public:
    ConstraintsFactory();
    std::shared_ptr<WatchedConstraint> create(std::string regexId) const;
    std::regex constraintsFilter();

private:
    std::map<std::string, std::function<std::shared_ptr<WatchedConstraint>(std::string)>>
      regex_to_ctypes_;
    std::vector<std::string> regex_ids_;
};

} // namespace Antares::Optimization
