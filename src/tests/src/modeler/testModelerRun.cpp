// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <antares/solver/modeler/Modeler.h>
#include <antares/solver/modeler/ILoader.h>
#include <antares/solver/modeler/IWriter.h>
#include <antares/study/system-model/system.h>
#include <antares/study/system-model/model.h>

// If we don't turn clang-format off here, some antlr4 header does not compile :
// it collides with a #include <windows.h> somewhere in Yuni
// clang-format off
#include <unit_test_utils.h>
// clang-format on

using namespace Antares::Solver;
using namespace Antares::ModelerStudy::SystemModel;

class RunMockLoader: public ILoader
{
public:
    ModelerParameters loadParameters() override
    {
        ModelerParameters params;
        params.firstTimeStep = 0;
        params.lastTimeStep = 0; // single timestep for speed
        params.solver = "SCIP";
        params.solverLogs = false;
        params.noOutput = false; // request output so writer.init is exercised
        params.exportMps = false;
        return params;
    }

    ModelerData loadAll() override
    {
        ModelerData data;
        // Build a simple system with one model and one component and one variable
        ModelBuilder modelBuilder;
        std::vector<Variable> vars;
        // Non FLOAT -> mark as MIP to exercise isMip path
        vars.push_back({"v1", {}, {}, ValueType::INT, {}, {}, Location::MASTER_AND_SUBPROBLEMS});
        auto model = modelBuilder.withVariables(std::move(vars)).withId("m1").build();

        ComponentBuilder cb;
        auto comp = cb.withModel(&model).withId("c1").build();
        SystemBuilder sb;
        auto system = sb.withComponents({comp}).withId("s1").build();

        data.system = std::make_unique<System>(system);
        data.libraries = {};
        data.scenarioGroupRepository = ScenarioGroupRepository();
        data.dataSeries = std::make_unique<ModelerStudy::DataSeriesRepository>();
        return data;
    }
};

class RunMockWriter: public IWriter
{
public:
    void init(const std::string& simulationId) override
    {
        initCalled = true;
        lastId = simulationId;
    }

    void writeSimulationTable(const Antares::Optimisation::LinearProblemApi::ILinearProblem&,
                               const Antares::Optimisation::LinearProblemApi::IMipSolution&,
                               const ModelerData&,
                               const Antares::Optimisation::OptimEntityContainer&,
                               const Antares::Optimisation::LinearProblemApi::FillContext&) const override
    {
        writeCalled = true;
    }

    mutable bool initCalled = false;
    mutable bool writeCalled = false;
    std::string lastId;
};

BOOST_AUTO_TEST_SUITE(modeler_run_tests)

BOOST_AUTO_TEST_CASE(modeler_run_executes_and_calls_writer)
{
    RunMockLoader loader;
    RunMockWriter writer;

    Modeler modeler(loader, writer);

    // Should not throw and should run the full pipeline for the simple system
    BOOST_CHECK_NO_THROW(modeler.run());

    // Writer init should have been called because noOutput=false
    BOOST_CHECK(writer.initCalled == true);
}

BOOST_AUTO_TEST_SUITE_END()
