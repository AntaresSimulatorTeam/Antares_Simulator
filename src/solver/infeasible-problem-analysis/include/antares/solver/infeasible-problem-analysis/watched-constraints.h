
#pragma once

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
    std::string regexId() const;
    virtual std::string infeasibility() = 0;
    virtual std::string infeasibilityCause() = 0;

protected:
    const std::vector<std::string>& splitName() const;

protected:
    std::string constraintName_;
    std::string regexId_;
    std::vector<std::string> splitName_;
};

class HourlyBC: public WatchedConstraint
{
public:
    HourlyBC(std::string constraintName);
    std::string infeasibility() override;
    std::string infeasibilityCause() override;
};

class DailyBC: public WatchedConstraint
{
public:
    DailyBC(std::string constraintName);
    std::string infeasibility() override;
    std::string infeasibilityCause() override;
};

class WeeklyBC: public WatchedConstraint
{
public:
    WeeklyBC(std::string constraintName);
    std::string infeasibility() override;
    std::string infeasibilityCause() override;
};

class FictitiousLoad: public WatchedConstraint
{
public:
    FictitiousLoad(std::string constraintName);
    std::string infeasibility() override;
    std::string infeasibilityCause() override;
};

class HydroLevel: public WatchedConstraint
{
public:
    HydroLevel(std::string constraintName);
    std::string infeasibility() override;
    std::string infeasibilityCause() override;
};

class STS: public WatchedConstraint
{
public:
    STS(std::string constraintName);
    std::string infeasibility() override;
    std::string infeasibilityCause() override;
};

class HydroProduction: public WatchedConstraint
{
public:
    HydroProduction(std::string constraintName);
    std::string infeasibility() override;
    std::string infeasibilityCause() override;
};

class ConstraintsFactory
{
public:
    std::shared_ptr<WatchedConstraint> create(std::string regexId) const;
    std::regex constraintsFilter();

private:
    const std::vector<std::string> regex_ids_ = {"::hourly::",
                                                 "::daily::",
                                                 "::weekly::",
                                                 "^FictiveLoads::",
                                                 "^AreaHydroLevel::",
                                                 "^Level::",
                                                 "^HydroPower::"};
};

} // namespace Antares::Optimization
