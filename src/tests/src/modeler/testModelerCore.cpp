// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <antares/solver/modeler/Modeler.h>
#include <antares/solver/modeler/ILoader.h>
#include <antares/solver/modeler/IWriter.h>
#include <antares/solver/modeler/ModelerData.h>
#include <antares/solver/modeler/parameters/modelerParameters.h>
#include "antares/optimisation/linear-problem-api/FillContext.h"
#include "antares/optimisation/linear-problem-api/mipSolution.h"
#include "antares/study/system-model/model.h"
#include "antares/study/system-model/system.h"

// If we don't turn clang-format off here, some antlr4 header does not compile :
// it collides with a #include <windows.h> somewhere in Yuni
// clang-format off
#include <unit_test_utils.h>
// clang-format on

using namespace Antares::Solver;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Optimisation::LinearProblemApi;

class MockLoader: public ILoader
{
public:
    ModelerParameters loadParameters() override
    {
        ModelerParameters params;
        params.firstTimeStep = 0;
        params.lastTimeStep = 4;
        params.solver = "SCIP";
        params.solverLogs = false;
        params.noOutput = true;
        params.exportMps = false;
        return params;
    }

    ModelerData loadAll() override
    {
        ModelerData data;
        // Create simple empty system for testing.
        data.system = std::make_unique<System>(SystemBuilder().withId("test_system").build());
        data.libraries = {};
        return data;
    }
};

class MockWriter: public IWriter
{
public:
    void init(const std::string& simulationId) override
    {
        initCalled = true;
        lastSimulationId = simulationId;
    }

    void writeSimulationTable(const ILinearProblem& linearProblem,
                               const IMipSolution& solution,
                               const ModelerData& modelerData,
                               const Optimisation::OptimEntityContainer& variableContainer,
                               const FillContext& fillContext) const override
    {
        writeCalled = true;
    }

    mutable bool initCalled = false;
    mutable bool writeCalled = false;
    std::string lastSimulationId;
};

BOOST_AUTO_TEST_SUITE(modeler_core_tests)

BOOST_AUTO_TEST_CASE(modeler_construction_loads_parameters_and_data)
{
    MockLoader loader;
    MockWriter writer;

    // Constructor should load parameters and data.
    Modeler modeler(loader, writer);

    // Check that problems are not yet built.
    BOOST_CHECK(modeler.masterProblem() == nullptr);
    BOOST_CHECK(modeler.subproblems().empty());
}

BOOST_AUTO_TEST_CASE(modeler_throws_on_load_error)
{
    class FailingLoader: public ILoader
    {
    public:
        ModelerParameters loadParameters() override
        {
            throw LoadFiles::ErrorLoadingYaml();
        }
        ModelerData loadAll() override
        {
            return {};
        }
    };

    FailingLoader loader;
    MockWriter writer;

    BOOST_CHECK_THROW(Modeler(loader, writer), Modeler::ModelerError);
}

BOOST_AUTO_TEST_CASE(modeler_error_message)
{
    try
    {
        throw Modeler::ModelerError("Test error message");
    }
    catch (const std::exception& e)
    {
        BOOST_CHECK_EQUAL(std::string(e.what()), "Test error message");
    }
}

BOOST_AUTO_TEST_SUITE_END()
