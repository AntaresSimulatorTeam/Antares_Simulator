
#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Antares::Optimization
{
class WatchedConstraint
{
public:
    void setConstraintName(std::string constraintName);
    std::string regexId();
    virtual std::shared_ptr<WatchedConstraint> clone() const = 0;
    virtual std::string infeasisibity() = 0;
    virtual std::string infeasisibityCause() = 0;

protected:
    std::string name_;
    std::string regexId_;
    std::vector<std::string> splitName_;
};

class HourlyBC: public WatchedConstraint
{
public:
    HourlyBC();
    std::shared_ptr<WatchedConstraint> clone() const override;
    std::string infeasisibity() override;
    std::string infeasisibityCause() override;
};

class DailyBC: public WatchedConstraint
{
public:
    DailyBC();
    std::shared_ptr<WatchedConstraint> clone() const override;
    std::string infeasisibity() override;
    std::string infeasisibityCause() override;
};

class WeeklyBC: public WatchedConstraint
{
public:
    WeeklyBC();
    std::shared_ptr<WatchedConstraint> clone() const override;
    std::string infeasisibity() override;
    std::string infeasisibityCause() override;
};

class FictitiousLoad: public WatchedConstraint
{
public:
    FictitiousLoad();
    std::shared_ptr<WatchedConstraint> clone() const override;
    std::string infeasisibity() override;
    std::string infeasisibityCause() override;
};

class HydroLevel: public WatchedConstraint
{
public:
    HydroLevel();
    std::shared_ptr<WatchedConstraint> clone() const override;
    std::string infeasisibity() override;
    std::string infeasisibityCause() override;
};

class STS: public WatchedConstraint
{
public:
    STS();
    std::shared_ptr<WatchedConstraint> clone() const override;
    std::string infeasisibity() override;
    std::string infeasisibityCause() override;
};

class HydroProduction: public WatchedConstraint
{
public:
    std::shared_ptr<WatchedConstraint> clone() const override;
    HydroProduction();
    std::string infeasisibity() override;
    std::string infeasisibityCause() override;
};
} // namespace Antares::Optimization
