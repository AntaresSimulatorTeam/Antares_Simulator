
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
    WatchedConstraint(const std::string& name);
    virtual std::string infeasibility() = 0;
    virtual std::string infeasibilityCause() = 0;

protected:
    const std::vector<std::string>& splitName() const;

protected:
    std::string fullName_;
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
    std::unique_ptr<WatchedConstraint> create(const std::string& pair) const;
    std::regex constraintsFilter();

private:
    std::map<std::string, std::function<std::unique_ptr<WatchedConstraint>(const std::string&)>>
      regex_to_ctypes_;
};

} // namespace Antares::Optimization
