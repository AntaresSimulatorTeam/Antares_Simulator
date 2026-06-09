// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

// clang-format off
#include <unit_test_utils.h>
// clang-format on

#include <boost/test/unit_test.hpp>

#include "antares/solver/optimisation/MipDetection.h"

#include "inmemory-modeler.h"

using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Solver;
using namespace Test::Modeler;

BOOST_AUTO_TEST_SUITE(test_mip_detection)

BOOST_AUTO_TEST_CASE(null_modeler_data_returns_false)
{
    BOOST_CHECK_EQUAL(
      Antares::Optimization::hasIntegerVariables(nullptr,
                                                 Antares::Solver::Config::Location::SUBPROBLEMS),
      false);
}

BOOST_AUTO_TEST_CASE(modeler_data_with_null_system_returns_false)
{
    ModelerData modelerData;
    modelerData.system = nullptr;
    BOOST_CHECK_EQUAL(
      Antares::Optimization::hasIntegerVariables(&modelerData,
                                                 Antares::Solver::Config::Location::SUBPROBLEMS),
      false);
}

struct MipDetectionFixture: public LinearProblemBuildingFixture
{
    std::unique_ptr<ModelerData> createModelerDataWithVariableType(ValueType type)
    {
        auto modelerData = std::make_unique<ModelerData>();

        createModel("test_model",
                    {},
                    {{"test_var", type, literal(0.0), literal(1.0), false, false}},
                    {});
        createComponent("test_model", "test_component");

        SystemBuilder systemBuilder;
        systemBuilder.withId("test_system");
        systemBuilder.withComponents(std::move(components));
        modelerData->system = std::make_unique<System>(systemBuilder.build());

        return modelerData;
    }
};

BOOST_FIXTURE_TEST_CASE(modeler_data_with_float_variable_returns_false, MipDetectionFixture)
{
    auto modelerData = createModelerDataWithVariableType(ValueType::FLOAT);
    BOOST_CHECK_EQUAL(
      Antares::Optimization::hasIntegerVariables(modelerData.get(),
                                                 Antares::Solver::Config::Location::SUBPROBLEMS),
      false);
}

BOOST_FIXTURE_TEST_CASE(modeler_data_with_integer_variable_returns_true, MipDetectionFixture)
{
    auto modelerData = createModelerDataWithVariableType(ValueType::INTEGER);
    BOOST_CHECK_EQUAL(
      Antares::Optimization::hasIntegerVariables(modelerData.get(),
                                                 Antares::Solver::Config::Location::SUBPROBLEMS),
      true);
}

BOOST_FIXTURE_TEST_CASE(modeler_data_with_bool_variable_returns_true, MipDetectionFixture)
{
    auto modelerData = createModelerDataWithVariableType(ValueType::BOOL);
    BOOST_CHECK_EQUAL(
      Antares::Optimization::hasIntegerVariables(modelerData.get(),
                                                 Antares::Solver::Config::Location::SUBPROBLEMS),
      true);
}

BOOST_FIXTURE_TEST_CASE(modeler_data_with_mixed_variables_float_and_integer_returns_true,
                        MipDetectionFixture)
{
    ModelerData modelerData;

    createModel("test_model",
                {},
                {{"var_float", ValueType::FLOAT, literal(0.0), literal(1.0), false, false},
                 {"var_int", ValueType::INTEGER, literal(0.0), literal(10.0), false, false}},
                {});
    createComponent("test_model", "test_component");

    SystemBuilder systemBuilder;
    systemBuilder.withId("test_system");
    systemBuilder.withComponents(std::move(components));
    modelerData.system = std::make_unique<System>(systemBuilder.build());

    BOOST_CHECK_EQUAL(
      Antares::Optimization::hasIntegerVariables(&modelerData,
                                                 Antares::Solver::Config::Location::SUBPROBLEMS),
      true);
}

BOOST_FIXTURE_TEST_CASE(modeler_data_with_mixed_variables_float_and_bool_returns_true,
                        MipDetectionFixture)
{
    ModelerData modelerData;

    createModel("test_model",
                {},
                {{"var_float", ValueType::FLOAT, literal(0.0), literal(1.0), false, false},
                 {"var_bool", ValueType::BOOL, literal(0.0), literal(1.0), false, false}},
                {});
    createComponent("test_model", "test_component");

    SystemBuilder systemBuilder;
    systemBuilder.withId("test_system");
    systemBuilder.withComponents(std::move(components));
    modelerData.system = std::make_unique<System>(systemBuilder.build());

    BOOST_CHECK_EQUAL(
      Antares::Optimization::hasIntegerVariables(&modelerData,
                                                 Antares::Solver::Config::Location::SUBPROBLEMS),
      true);
}

BOOST_FIXTURE_TEST_CASE(modeler_data_with_only_float_variables_returns_false, MipDetectionFixture)
{
    ModelerData modelerData;

    createModel("test_model",
                {},
                {{"var1", ValueType::FLOAT, literal(0.0), literal(1.0), false, false},
                 {"var2", ValueType::FLOAT, literal(0.0), literal(2.0), false, false},
                 {"var3", ValueType::FLOAT, literal(0.0), literal(3.0), false, false}},
                {});
    createComponent("test_model", "test_component");

    SystemBuilder systemBuilder;
    systemBuilder.withId("test_system");
    systemBuilder.withComponents(std::move(components));
    modelerData.system = std::make_unique<System>(systemBuilder.build());

    BOOST_CHECK_EQUAL(
      Antares::Optimization::hasIntegerVariables(&modelerData,
                                                 Antares::Solver::Config::Location::SUBPROBLEMS),
      false);
}

BOOST_AUTO_TEST_SUITE_END()
