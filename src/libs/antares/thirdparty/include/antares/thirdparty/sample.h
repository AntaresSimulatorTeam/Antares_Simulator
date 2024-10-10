#pragma once
#include <antares/thirdparty/thirdparty.h>

namespace Antares::ThirdParty
{

class SampleModule: public Module
{
    // The name of the 3rd party module
    std::string moduleName() const override;
    bool loadFromFolder(const std::string& inputFolder) override;
    bool initializeFromStudy(const Antares::Data::Study& study) override;

    void addVariables(operations_research::MPSolver* solver) override;
    void addConstraints(operations_research::MPSolver* solver) override;

    void updateVariables(operations_research::MPSolver* solver, OptimizationContext ctx) override;
    void updateConstraints(operations_research::MPSolver* solver, OptimizationContext ctx) override;
    void setObjective(operations_research::MPSolver* solver, OptimizationContext ctx) override;
    unsigned int numberOfColumns(unsigned int areaIndex) override;
    double areaOutputForHour(unsigned int areaIndex, unsigned int hourInTheWeek) override;
    std::vector<VariableProperties> getProperties() override;
};
} // namespace Antares::ThirdParty
