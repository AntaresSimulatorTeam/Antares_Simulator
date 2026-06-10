// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test_api_modelerproblems

#include <filesystem>

#include <boost/test/unit_test.hpp>

#include <antares/expressions/nodes/GreaterThanOrEqualNode.h>
#include <antares/optimisation/linear-problem-api/linearProblem.h>
#include <antares/solver/modeler/ILoader.h>
#include <antares/solver/modeler/ModelerData.h>

#include "inmemory-modeler.h"
#include "modelerProblemsImpl.h"

using namespace Antares::Expressions;
using namespace Antares::Solver;
using namespace Antares::Optimisation;
using PTV = ParameterTypeAndValue;
using VV = VariabilityType;

namespace fs = std::filesystem;

class ConstantDataSeries: public LinearProblemApi::ILinearProblemData
{
public:
    explicit ConstantDataSeries(double value):
        value_(value)
    {
    }

    double getData(const std::string& /*dataSetId*/,
                   unsigned /*year*/,
                   unsigned /*hour*/) const override
    {
        return value_;
    }

    [[nodiscard]] std::span<const double> getData(const std::string& /*dataSetId*/,
                                                  unsigned /*timeSeriesNumber*/,
                                                  unsigned /*firstHour*/,
                                                  unsigned /*lastHour*/) const override
    {
        return vector_;
    }

private:
    double value_{0.};
    std::vector<double> vector_{value_};
};

class TestLoader: public ILoader
{
public:
    ModelerParameters loadParameters() override
    {
        return {.solver = "sirius",
                .solverLogs = false,
                .solverParameters = "DUMMY",
                .noOutput = true,
                .exportMps = false,
                .firstTimeStep = 0,
                .lastTimeStep = 0};
    }

    std::optional<ModelerData> loadAll() override
    {
        auto objective = fixture.variable("x", 0);
        auto var_node = fixture.variable("x", 0);
        auto zero = fixture.literal(0);
        auto ct_node = fixture.nodeRegistry.create<Nodes::GreaterThanOrEqualNode>(var_node, zero);

        fixture.createModelWithOneFloatVar("model",
                                           {},
                                           "x",
                                           fixture.literal(0),
                                           fixture.literal(10),
                                           {{"ct1", ct_node}},
                                           objective,
                                           false);

        fixture.createComponent("model", "component");

        SystemBuilder builder;
        auto system = builder.withId("system")
                        .withComponents(std::move(fixture.components))
                        .build();

        ModelerData md;
        LibraryBuilder lib_builder;
        auto library = lib_builder.withId("library").withDescription("").build();
        md.libraries = {library};
        md.system = std::make_unique<System>(std::move(system));
        md.dataSeries = std::make_unique<ConstantDataSeries>(0.);

        return md;
    }

    Test::Modeler::LinearProblemBuildingFixture fixture;
};

struct OutputDir
{
    fs::path path;

    OutputDir():
        path(fs::temp_directory_path() / "antares-test-modelerproblems")
    {
        fs::create_directories(path);
    }

    ~OutputDir()
    {
        fs::remove_all(path);
    }
};

BOOST_AUTO_TEST_CASE(ModelerProblems_construction_with_inmemory_loader_succeeds)
{
    OutputDir out;
    TestLoader loader;

    BOOST_CHECK_NO_THROW(
      Implementation::ModelerProblems(loader, out.path, Antares::Writer::TableFormat::CSV));
}

BOOST_AUTO_TEST_CASE(ModelerProblems_logSize_does_not_crash)
{
    OutputDir out;
    TestLoader loader;

    Implementation::ModelerProblems problems(loader, out.path, Antares::Writer::TableFormat::CSV);
    BOOST_CHECK_NO_THROW(problems.logSize());
}

BOOST_AUTO_TEST_CASE(ModelerProblems_export_creates_output_files)
{
    OutputDir out;
    TestLoader loader;

    Implementation::ModelerProblems problems(loader, out.path, Antares::Writer::TableFormat::CSV);

    BOOST_CHECK(fs::exists(out.path / "1-1.mps"));
    BOOST_CHECK(fs::exists(out.path / "structure.txt"));
}
