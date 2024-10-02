#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

#include <antares/solver/modeler/api/linearProblemFiller.h>

namespace Antares::Data
{
class Study;
}

namespace Antares::ThirdParty
{
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

class Optimization
{
public:
    struct Context
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

    Context context;

private:
};

class Module: public Antares::Solver::Modeler::Api::LinearProblemFiller
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

    virtual unsigned int numberOfColumns(unsigned int areaIndex) = 0;
    virtual double areaOutputForHour(unsigned int areaIndex, unsigned int hourInTheWeek) = 0;
    virtual std::vector<VariableProperties> getProperties() = 0;
};

extern Module* GLOBAL_mod;
} // namespace Antares::ThirdParty
