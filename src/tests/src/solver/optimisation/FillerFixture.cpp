// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "FillerFixture.h"

#include "antares/io/inputs/model-converter/modelConverter.h"
#include "antares/io/inputs/yml-model/parser.h"
#include "antares/io/inputs/yml-system/converter.h"
#include "antares/io/inputs/yml-system/parser.h"
#include "antares/optimisation/linear-problem-data-impl/timeSeriesSet.h"
#include "antares/solver/optim-model-filler/Dimensions.h"
#include "antares/solver/optimisation/ComponentToAreaConnectionFiller.h"
#include "antares/solver/optimisation/ThermalCapacityFiller.h"
using namespace std::string_literals;

using namespace Optimization;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Optimisation;
using namespace LinearProblemApi;
using namespace LinearProblemDataImpl;

FillerFixture::FillerFixture():
    linearProblem(true, "scip"),
    linearProblemData((DataSeriesRepository()))
{
}

void FillerFixture::init(const std::string& systemYaml, const std::string& libraryYaml)
{
    problemeHebdo = std::make_unique<PROBLEME_HEBDO>();
    problemeHebdo->ProblemeAResoudre = std::make_unique<PROBLEME_ANTARES_A_RESOUDRE>();
    setUpModelerSystem(systemYaml, libraryYaml);

    auto scenarioPtr = std::make_unique<Scenario>("SG");
    scenarioPtr->setTimeSerieNumber(0, 1);
    scenarioGroupRepository.addScenario("SG", std::move(scenarioPtr));
}

void FillerFixture::setData(const std::string& name, const std::vector<double>& some_param_value)
{
    auto tss = std::make_unique<TimeSeriesSet>(name, some_param_value.size());
    tss->add(some_param_value);
    linearProblemData.addDataSeries(std::move(tss));
}

void FillerFixture::setUpModelerSystem(const std::string& systemYaml,
                                       const std::string& libraryYaml)
{
    IO::Inputs::YmlModel::Parser parserModel;
    libraries.push_back(IO::Inputs::ModelConverter::convert(parserModel.parse(libraryYaml)));
    IO::Inputs::YmlSystem::Parser parserSystem;
    auto ymlSystem = parserSystem.parse(systemYaml);
    auto system = IO::Inputs::SystemConverter::convert(ymlSystem, libraries);
    modelerData = std::make_unique<Solver::ModelerData>();
    modelerData->system = std::make_unique<System>(std::move(system));
    problemeHebdo->modelerData = modelerData.get();
}

// this should be called before setUpModelerVariables
void FillerFixture::addLegacyVariables(const std::vector<std::string>& variableNames)
{
    for (const auto& variable: variableNames)
    {
        linearProblem.addVariable(-999, 999, false, variable);
    }
}

void FillerFixture::addModelerVariables(unsigned int ts_start,
                                        unsigned int ts_end,
                                        OptimEntityContainer& optimEntityContainer)
{
    const Dimensions dim({}, IntegerInterval(ts_start, ts_end));
    for (const auto& component: modelerData->system->Components())
    {
        for (const auto& variable: component.getModel()->Variables())
        {
            optimEntityContainer.addStartColumn();
            if (variable.isTimeDependent())
            {
                for (auto t = ts_start; t <= ts_end; ++t)
                {
                    auto name = buildVariableName(component.Id(), variable.Id(), {}, t);
                    linearProblem.addVariable(-999, 999, false, name);
                }
            }
            else
            {
                auto name = buildVariableName(component.Id(),
                                              variable.Id(),
                                              std::nullopt,
                                              std::nullopt);
                linearProblem.addVariable(-999, 999, false, name);
            }
        }
    }
}

void FillerFixture::addEmptyConstraintsToLinearProblem(std::vector<std::string>& names, double rhs)
{
    for (const auto& name: names)
    {
        linearProblem.addConstraint(rhs, rhs, name);
    }
}

void FillerFixture::addEmptyConstraints(std::vector<std::string>& constraintNames,
                                        bool useNamedProblems,
                                        double rhs)
{
    problemeHebdo->ProblemeAResoudre->NomDesContraintes = constraintNames;
    problemeHebdo->ProblemeAResoudre->NombreDeContraintes = constraintNames.size();
    if (useNamedProblems)
    {
        addEmptyConstraintsToLinearProblem(constraintNames, rhs);
    }
    else
    {
        std::vector<std::string> lpConstraintNames;
        for (unsigned int i = 0; i < constraintNames.size(); ++i)
        {
            lpConstraintNames.push_back("c" + std::to_string(i));
        }
        addEmptyConstraintsToLinearProblem(lpConstraintNames, rhs);
    }
}

void FillerFixture::addLegacyLp(std::vector<std::string>& constraintNames,
                                bool useNamedProblems,
                                double rhs)
{
    problemeHebdo->NamedProblems = useNamedProblems;
    addEmptyConstraints(constraintNames, useNamedProblems, rhs);
}

void FillerFixture::fillProblemWithThermalCapacityConnectionFiller(
  const FillContext& fillCtx,
  OptimEntityContainer& optimEntityContainer) const
{
    problemeHebdo->NombreDePasDeTempsPourUneOptimisation = fillCtx.getLocalNumberOfTimeSteps();

    ThermalCapacityFiller filler(problemeHebdo.get(),
                                 optimEntityContainer,
                                 &linearProblemData,
                                 scenarioGroupRepository);
    filler.addVariables(fillCtx);
    filler.addConstraints(fillCtx);
    filler.addObjectives(fillCtx);
}
