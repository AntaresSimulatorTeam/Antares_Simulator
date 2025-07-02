
#pragma once

#include <functional>
#include <map>
#include <memory>
#include <regex>
#include <string>
#include <vector>

std::string timeStep(const std::vector<std::string>& splitName);
std::string shortName(const std::vector<std::string>& splitName);
std::string areaName(const std::vector<std::string>& splitName);
std::string STSname(const std::vector<std::string>& splitName);
std::string STSAdditionalConstraintName(const std::vector<std::string>& splitName);

namespace Antares::Optimization
{
class WatchedConstraint
{
public:
    explicit WatchedConstraint(const std::string& name, double slackValue);
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

template<const char* ConstraintType>
class STSAdditionalConstraint: public WatchedConstraint
{
    using WatchedConstraint::WatchedConstraint;

public:
    ~STSAdditionalConstraint() override = default;

    std::string infeasibility() override
    {
        return "Short-term-storage additional constraint (" + std::string(ConstraintType) + ") "
               + STSAdditionalConstraintName(splitName()) + " at area '" + areaName(splitName())
               + "' in STS '" + STSname(splitName()) + "' at  " + timeStep(splitName());
    }

    std::string infeasibilityCause() override
    {
        return "* Short-term " + std::string(ConstraintType) + " additional constraint";
    }
};

constexpr char WITHDRAWAL[] = "withdrawal";
constexpr char INJECTION[] = "injection";
constexpr char NETTING[] = "netting";

using STSWithdrawalSum = STSAdditionalConstraint<WITHDRAWAL>;
using STSInjectionSum = STSAdditionalConstraint<INJECTION>;
using STSNettingSum = STSAdditionalConstraint<NETTING>;

class ConstraintsFactory
{
public:
    explicit ConstraintsFactory() = default;
    std::unique_ptr<WatchedConstraint> create(const std::string&, double) const;
    std::regex constraintsFilter();

private:
    static const std::map<
      std::string,
      std::pair<std::regex,
                std::function<std::unique_ptr<WatchedConstraint>(const std::string&, double)>>>
      regex_to_ctypes_;
};

} // namespace Antares::Optimization
