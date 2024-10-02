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

// OUTPUTS
unsigned int SampleModule::numberOfColumns(unsigned int areaIndex)
{
    return 1;
}

double SampleModule::areaOutputForHour(unsigned int areaIndex, unsigned int hourInTheWeek)
{
    return 42.; // TODO pVar->solution_value();
}

std::vector<VariableProperties> SampleModule::getProperties()
{
    return std::vector<VariableProperties>{VariableProperties("sample", "unit")};
}

void SampleModule::addVariables(LP& pb, LPD& data)
{
    pVar = pb.addNumVariable(0, 10, "sample_variable");
}

void SampleModule::addConstraints(LP& pb, LPD& data)
{
    pConstr = pb.addConstraint(0, 1, "sample_constraint");
}

void SampleModule::addObjective(LP& pb, LPD& data)
{
    pb.setObjectiveCoefficient(pVar, 1);
}

} // namespace Antares::ThirdParty
