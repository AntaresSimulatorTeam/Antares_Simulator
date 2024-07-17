
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
    explicit WatchedConstraint(const std::string& name, const double slackValue);
    virtual ~WatchedConstraint() = default;
    virtual std::string infeasibility() = 0;
    virtual std::string infeasibilityCause() = 0;
    double slackValue() const;

protected:
    const std::vector<std::string>& splitName() const;

private:
    std::vector<std::string> splitName_;
    double slack_value_;
};

class HourlyBC: public WatchedConstraint
{
    using WatchedConstraint::WatchedConstraint;

public:
    ~HourlyBC() override = default;
    std::string infeasibility() override;
    std::string infeasibilityCause() override;
};

class DailyBC: public WatchedConstraint
{
    using WatchedConstraint::WatchedConstraint;

public:
    ~DailyBC() override = default;
    std::string infeasibility() override;
    std::string infeasibilityCause() override;
};

class WeeklyBC: public WatchedConstraint
{
    using WatchedConstraint::WatchedConstraint;

public:
    ~WeeklyBC() override = default;
    std::string infeasibility() override;
    std::string infeasibilityCause() override;
};

class FictitiousLoad: public WatchedConstraint
{
    using WatchedConstraint::WatchedConstraint;

public:
    ~FictitiousLoad() override = default;
    std::string infeasibility() override;
    std::string infeasibilityCause() override;
};

class HydroLevel: public WatchedConstraint
{
    using WatchedConstraint::WatchedConstraint;

public:
    ~HydroLevel() override = default;
    std::string infeasibility() override;
    std::string infeasibilityCause() override;
};

class STS: public WatchedConstraint
{
    using WatchedConstraint::WatchedConstraint;

public:
    ~STS() override = default;
    std::string infeasibility() override;
    std::string infeasibilityCause() override;
};

class HydroProduction: public WatchedConstraint
{
    using WatchedConstraint::WatchedConstraint;

public:
    ~HydroProduction() override = default;
    std::string infeasibility() override;
    std::string infeasibilityCause() override;
};

class ConstraintsFactory
{
public:
    explicit ConstraintsFactory();
    std::unique_ptr<WatchedConstraint> create(const std::string&, const double) const;
    std::regex constraintsFilter();

private:
    std::map<std::string,
             std::function<std::unique_ptr<WatchedConstraint>(const std::string&, const double)>>
      regex_to_ctypes_;
};

} // namespace Antares::Optimization
