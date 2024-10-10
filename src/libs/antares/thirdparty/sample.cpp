#include <antares/thirdparty/sample.h>
#include <antares/thirdparty/thirdparty.h>

namespace Antares::ThirdParty
{
std::string SampleModule::moduleName() const
{
    return "SampleModule";
}

// LOADING & VALIDATION
// Load parameters/series/etc.
bool SampleModule::loadFromFolder(const std::string& inputFolder)
{
    return true;
}

// TODO `const` may prevent some operations from taking place
// Load all required data from Study, check pre-conditions
bool SampleModule::initializeFromStudy(const Antares::Data::Study& study)
{
    return true;
}

// OPTIMIZATION
// 1st call = 1st weeks
void SampleModule::addVariables(operations_research::MPSolver* solver)
{
}

void SampleModule::addConstraints(operations_research::MPSolver* solver)
{
}

// subsequent weeks
void SampleModule::updateVariables(operations_research::MPSolver* solver, OptimizationContext ctx)
{
}

void SampleModule::updateConstraints(operations_research::MPSolver* solver, OptimizationContext ctx)
{
}

// all weeks
void SampleModule::setObjective(operations_research::MPSolver* solver, OptimizationContext ctx)
{
}

// OUTPUTS
unsigned int SampleModule::numberOfColumns(unsigned int areaIndex)
{
    return 1;
}

double SampleModule::areaOutputForHour(unsigned int areaIndex, unsigned int hourInTheWeek)
{
    return 42.;
}

std::vector<VariableProperties> SampleModule::getProperties()
{
    return std::vector<VariableProperties>{VariableProperties("sample", "unit")};
}
} // namespace Antares::ThirdParty
