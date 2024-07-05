
#pragma once

#include <string>
#include <vector>

namespace Antares::Optimization
{
class DetectedConstraint
{
public:
    void setConstraintName(std::string constraintName);
    std::string regexId();
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
    std::string infeasisibity() override;
    std::string infeasisibityCause() override;
};

class DailyBC : public DetectedConstraint
{
public:
    DailyBC();
    std::string infeasisibity() override;
    std::string infeasisibityCause() override;
};

class WeeklyBC : public DetectedConstraint
{
public:
    WeeklyBC();
    std::string infeasisibity() override;
    std::string infeasisibityCause() override;
};

class FictitiousLoad : public DetectedConstraint
{
public:
    FictitiousLoad();
    std::string infeasisibity() override;
    std::string infeasisibityCause() override;
};

class HydroLevel : public DetectedConstraint
{
public:
    HydroLevel();
    std::string infeasisibity() override;
    std::string infeasisibityCause() override;
};

class STS : public DetectedConstraint
{
public:
    STS();
    std::string infeasisibity() override;
    std::string infeasisibityCause() override;
};

class HydroProduction : public DetectedConstraint
{
public:
    HydroProduction();
    std::string infeasisibity() override;
    std::string infeasisibityCause() override;
};
}

