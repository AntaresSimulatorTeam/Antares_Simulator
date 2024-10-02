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

    using LP = Antares::Solver::Modeler::Api::ILinearProblem;
    using LPD = Antares::Solver::Modeler::Api::LinearProblemData;

    void addVariables(LP& pb, LPD& data) override;
    void addConstraints(LP& pb, LPD& data) override;
    void addObjective(LP& pb, LPD& data) override;

    // OUTPUTS
    unsigned int numberOfColumns(unsigned int areaIndex) override;
    double areaOutputForHour(unsigned int areaIndex, unsigned int hourInTheWeek) override;
    std::vector<VariableProperties> getProperties() override;

private:
    Antares::Solver::Modeler::Api::IMipVariable* pVar;
    Antares::Solver::Modeler::Api::IMipConstraint* pConstr;
};
} // namespace Antares::ThirdParty
