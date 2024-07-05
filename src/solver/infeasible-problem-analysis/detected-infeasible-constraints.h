
#pragma once

#include <string>
#include <vector>
#include <memory>

namespace Antares::Optimization
{
class DetectedConstraint
{
public:
    void setConstraintName(std::string constraintName);
    std::string regexId();
    virtual std::shared_ptr<DetectedConstraint> clone() const = 0;
    virtual std::string infeasisibity() = 0;
    virtual std::string infeasisibityCause() = 0;

protected:
    std::string name_;
    std::string regexId_;
    std::vector<std::string> splitName_;
};

class HourlyBC : public DetectedConstraint
{
public:
    HourlyBC();
    std::shared_ptr<DetectedConstraint> clone() const override;
    std::string infeasisibity() override;
    std::string infeasisibityCause() override;
};

class DailyBC : public DetectedConstraint
{
public:
    DailyBC();
    std::shared_ptr<DetectedConstraint> clone() const override;
    std::string infeasisibity() override;
    std::string infeasisibityCause() override;
};

class WeeklyBC : public DetectedConstraint
{
public:
    WeeklyBC();
     std::shared_ptr<DetectedConstraint> clone() const override;
    std::string infeasisibity() override;
    std::string infeasisibityCause() override;
};

class FictitiousLoad : public DetectedConstraint
{
public:
    FictitiousLoad();
    std::shared_ptr<DetectedConstraint> clone() const override;
    std::string infeasisibity() override;
    std::string infeasisibityCause() override;
};

class HydroLevel : public DetectedConstraint
{
public:
    HydroLevel();
    std::shared_ptr<DetectedConstraint> clone() const override;
    std::string infeasisibity() override;
    std::string infeasisibityCause() override;
};

class STS : public DetectedConstraint
{
public:
    STS();
    std::shared_ptr<DetectedConstraint> clone() const override;
    std::string infeasisibity() override;
    std::string infeasisibityCause() override;
};

class HydroProduction : public DetectedConstraint
{
public:
    std::shared_ptr<DetectedConstraint> clone() const override;
    HydroProduction();
    std::string infeasisibity() override;
    std::string infeasisibityCause() override;
};
}

