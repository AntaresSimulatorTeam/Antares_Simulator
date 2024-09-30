#pragma once

#include <string>
#include <vector>

namespace operations_research
{
class MPSolver;
}

namespace Antares::Data
{
class Study;
}

namespace Antares::ThirdParty
{
struct OptimizationContext
{
    enum OptimizationNumber
    {
        first,
        second
    };

    OptimizationNumber n;
    unsigned int weekNumber;
    unsigned int yearNumber;
};

struct VariableProperties

{
    VariableProperties(std::string caption, std::string unit):
        caption(caption),
        unit(unit)
    {
    }

    std::string caption;
    std::string unit;
};

class Module
{
public:
    // The name of the 3rd party module
    virtual std::string moduleName() const = 0;

    // LOADING & VALIDATION
    // Load parameters/series/etc.
    virtual bool loadFromFolder(const std::string& inputFolder) = 0;

    // TODO `const` may prevent some operations from taking place
    // Load all required data from Study, check pre-conditions
    virtual bool initializeFromStudy(const Antares::Data::Study& study) = 0;

    // OPTIMIZATION
    // 1st call = 1st weeks
    virtual void addVariables(operations_research::MPSolver* solver) = 0;
    virtual void addConstraints(operations_research::MPSolver* solver) = 0;

    // subsequent weeks
    virtual void updateVariables(operations_research::MPSolver* solver, OptimizationContext ctx)
      = 0;
    virtual void updateConstraints(operations_research::MPSolver* solver, OptimizationContext ctx)
      = 0;

    // all weeks
    virtual void setObjective(operations_research::MPSolver* solver, OptimizationContext ctx) = 0;

    virtual unsigned int numberOfColumns(unsigned int areaIndex) = 0;
    virtual double areaOutputForHour(unsigned int areaIndex, unsigned int hourInTheWeek) = 0;
    virtual std::vector<VariableProperties> getProperties() = 0;
};
} // namespace Antares::ThirdParty
