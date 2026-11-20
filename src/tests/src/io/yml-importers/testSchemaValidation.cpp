// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <algorithm>
#include <string>
#include <unit_test_utils.h>
#include <utility>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <antares/io/inputs/InputError.h>
#include <antares/io/inputs/yml-model/parser.h>
#include <antares/io/inputs/yml-schema/schemaValidation.h>
#include <antares/io/inputs/yml-system/parser.h>

using namespace Antares::IO::Inputs;
using YmlSchema::SchemaKind;

namespace
{
std::string validationMessage(const std::string& content, SchemaKind kind)
{
    try
    {
        YmlSchema::validate(YAML::Load(content), kind);
    }
    catch (const InputError& e)
    {
        return e.what();
    }
    return "";
}
} // namespace

BOOST_AUTO_TEST_SUITE(SchemaValidation)

BOOST_AUTO_TEST_CASE(valid_documents_pass)
{
    BOOST_CHECK_NO_THROW(
      YmlSchema::validate(YAML::Load("library: {id: lib}"), SchemaKind::ModelLibrary));
    BOOST_CHECK_NO_THROW(YmlSchema::validate(YAML::Load("system: {id: s}"), SchemaKind::System));
}

BOOST_AUTO_TEST_CASE(context_prefixes_the_message)
{
    try
    {
        YmlSchema::validate(YAML::Load("system: {typo: 1}"),
                            SchemaKind::System,
                            "study/system.yml");
        BOOST_FAIL("expected an InputError");
    }
    catch (const InputError& e)
    {
        BOOST_CHECK(std::string(e.what()).starts_with("study/system.yml: YAML schema validation"));
    }
}

BOOST_AUTO_TEST_CASE(plain_scalars_are_typed_as_in_yaml_1_2)
{
    // A plain number is not a string, a quoted one is
    BOOST_CHECK(validationMessage("library: {id: 123}", SchemaKind::ModelLibrary)
                  .find("'library/id': Expected string, found integer")
                != std::string::npos);
    BOOST_CHECK_EQUAL(validationMessage("library: {id: \"123\"}", SchemaKind::ModelLibrary), "");
    // Booleans: `true` is one, `"true"` and `yes` are strings
    const auto system = [](const std::string& flag)
    {
        return "system:\n  components:\n    - id: c\n      model: l.m\n      parameters:\n"
               "        - id: p\n          value: 1\n          time-dependent: "
               + flag;
    };
    BOOST_CHECK_EQUAL(validationMessage(system("true"), SchemaKind::System), "");
    BOOST_CHECK(validationMessage(system("\"true\""), SchemaKind::System)
                  .find("Expected boolean, found string")
                != std::string::npos);
    BOOST_CHECK(
      validationMessage(system("yes"), SchemaKind::System).find("Expected boolean, found string")
      != std::string::npos);
}

BOOST_AUTO_TEST_CASE(duplicated_keys_are_reported_with_their_position)
{
    const auto message = validationMessage("library:\n  id: a\n  id: b\n",
                                           SchemaKind::ModelLibrary);
    BOOST_CHECK(message.find("Line 3 column 3: 'library': duplicated field 'id'")
                != std::string::npos);
}

// yaml-cpp's Node::operator= overwrites the referenced node inside the document instead of
// rebinding a handle. Resolving error locations must leave the document untouched and give the same
// answer every time, even after a scalar or null element in a list.
BOOST_AUTO_TEST_CASE(validation_does_not_modify_the_document)
{
    const auto node = YAML::Load(R"(
library:
  id: lib
  port-types:
    - scalar
    - id: p
      area-connection: scalar
  models:
    - null
    - id: m
      variables:
        - id: v
          variable-type: invalid
)");
    const auto before = YAML::Dump(node);
    std::string first;
    for (int i = 0; i < 2; ++i)
    {
        try
        {
            YmlSchema::validate(node, SchemaKind::ModelLibrary);
            BOOST_FAIL("expected an InputError");
        }
        catch (const InputError& e)
        {
            if (i == 0)
            {
                first = e.what();
            }
            BOOST_CHECK_EQUAL(e.what(), first);
        }
        BOOST_CHECK_EQUAL(YAML::Dump(node), before);
    }
    // Entries after a scalar or null one are located by id with the right position
    BOOST_CHECK(first.find("Line 7 column 24: 'library/port-types/p/area-connection'")
                != std::string::npos);
    BOOST_CHECK(first.find("Line 13 column 26: 'library/models/m/variables/v/variable-type'")
                != std::string::npos);
}

BOOST_AUTO_TEST_CASE(errors_are_sorted_by_position)
{
    const auto message = validationMessage(R"(
library:
  id: lib
  models:
    - id: m
      ports:
        - id: p
  port-types:
    - id: t
      typo: 1
)",
                                           SchemaKind::ModelLibrary);
    const auto typo = message.find("'typo'");
    const auto missingType = message.find("Required property 'type'");
    BOOST_REQUIRE(typo != std::string::npos);
    BOOST_REQUIRE(missingType != std::string::npos);
    BOOST_CHECK_LT(missingType, typo); // `ports` comes before `port-types` in the document
}

BOOST_AUTO_TEST_CASE(unknown_key_points_at_the_key_and_bad_value_at_the_value)
{
    const auto message = validationMessage("library:\n  id: lib\n  typo: 1\n",
                                           SchemaKind::ModelLibrary);
    BOOST_CHECK(message.find("Line 3 column 3: 'library/typo'") != std::string::npos);
    const auto typeMessage = validationMessage("library:\n  id: [a]\n", SchemaKind::ModelLibrary);
    BOOST_CHECK(typeMessage.find("Line 2 column 7: 'library/id'") != std::string::npos);
}

// Documented deviations of the local schema copy from the GEMS draft (see schemas/README.md)
BOOST_AUTO_TEST_CASE(simulator_deviations_are_accepted)
{
    BOOST_CHECK_NO_THROW(YmlModel::Parser{}.parse(R"(
library:
  id: lib
  models:
    - id: m
      variables:
        - id: v
          variable-type: boolean
          location: master
      constraints:
        - id: c
          expression: v <= 1
          location: master
      objective-contributions:
        - id: o
          expression: v
          location: master
)"));
    // The `<library>.<model>` format is checked when the system is converted, not by the schema
    BOOST_CHECK_NO_THROW(
      YmlSystem::Parser{}.parse("system:\n  components:\n    - id: c\n      model: no_dot\n", ""));
}

BOOST_AUTO_TEST_CASE(parameter_scenario_group_is_not_accepted)
{
    const auto message = validationMessage(R"(
system:
  components:
    - id: c
      model: l.m
      parameters:
        - id: p
          value: 1
          scenario-group: g
)",
                                           SchemaKind::System);
    BOOST_CHECK(message.find("Additional property 'scenario-group' not allowed by schema.")
                != std::string::npos);
}

BOOST_AUTO_TEST_CASE(scalars_are_typed_as_in_yaml_1_2)
{
    // value written after `id: ` -> what the schema sees. A string is accepted, anything else is
    // reported with its JSON type.
    const std::vector<std::pair<std::string, std::string>> cases = {
      {"~", "found null"},
      {"null", "found null"},
      {"Null", "found null"},
      {"NULL", "found null"},
      {"", "found null"},
      {"true", "found boolean"},
      {"True", "found boolean"},
      {"FALSE", "found boolean"},
      {"123", "found integer"},
      {"-3", "found integer"},
      {"+5", "found integer"},
      {"007", "found integer"},
      {"1.5", "found number"},
      {".5", "found number"},
      {"1e5", "found number"},
      {"99999999999999999999", "found number"}, // overflows int64: read as a double
      {"yes", ""},
      {"no", ""},
      {"on", ""},
      {"1-2", ""},
      {"e", ""},
      {"+", ""},
      {".inf", ""},
      {"0x10", ""},
      {"\"123\"", ""},
      {"'true'", ""},
      {"!!str 123", ""},
      {"!!str true", ""},
    };
    for (const auto& [value, expected]: cases)
    {
        BOOST_TEST_CONTEXT("id: " << value)
        {
            const auto message = validationMessage("library:\n  id: " + value + "\n",
                                                   SchemaKind::ModelLibrary);
            if (expected.empty())
            {
                BOOST_CHECK_EQUAL(message, "");
            }
            else
            {
                BOOST_CHECK(message.find("'library/id': Expected string, " + expected)
                            != std::string::npos);
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(non_scalar_and_null_keys_are_reported)
{
    const auto complexKey = validationMessage("library:\n  id: l\n  ? [a, b]\n  : 1\n",
                                              SchemaKind::ModelLibrary);
    BOOST_CHECK(complexKey.find("'library': expected a scalar field name") != std::string::npos);
    const auto nullKey = validationMessage("library:\n  id: l\n  ~: 1\n", SchemaKind::ModelLibrary);
    BOOST_CHECK(nullKey.find("Line 3 column 3: 'library': field name must not be empty or null")
                != std::string::npos);
}

BOOST_AUTO_TEST_CASE(positions_are_never_below_line_1_column_1)
{
    const auto message = validationMessage("", SchemaKind::ModelLibrary);
    BOOST_CHECK(message.find("Line 1 column 1: '': Expected object, found null")
                != std::string::npos);
}

BOOST_AUTO_TEST_CASE(missing_required_key_is_reported_on_its_parent)
{
    const auto message = validationMessage("library:\n  models: []\n", SchemaKind::ModelLibrary);
    BOOST_CHECK(message.find("Line 2 column 3: 'library': Required property 'id' not found.")
                != std::string::npos);
}

BOOST_AUTO_TEST_CASE(null_value_is_reported_on_its_key)
{
    // yaml-cpp gives no position to a null value: the key's is used
    const auto message = validationMessage(R"(library:
  id: l
  models:
    - id: m
      variables:
        - id: v
          variable-type:
)",
                                           SchemaKind::ModelLibrary);
    BOOST_CHECK(message.find("Line 7 column 11: 'library/models/m/variables/v/variable-type'")
                != std::string::npos);
}

// Aliases are expanded: a document must not be able to exhaust the stack or the memory
BOOST_AUTO_TEST_CASE(alias_to_an_ancestor_is_rejected)
{
    const auto message = validationMessage("library: &a\n  id: l\n  x: *a\n",
                                           SchemaKind::ModelLibrary);
    BOOST_CHECK(message.find("document too deeply nested or too large") != std::string::npos);
    const auto sequence = validationMessage("library:\n  id: l\n  x: &a [*a]\n",
                                            SchemaKind::ModelLibrary);
    BOOST_CHECK(sequence.find("document too deeply nested or too large") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(exponential_alias_expansion_is_rejected)
{
    // "Billion laughs": every level doubles the previous one
    std::string document = "library:\n  id: l\n  x:\n    - &a0 [z, z]\n";
    for (int level = 1; level < 40; ++level)
    {
        document += "    - &a" + std::to_string(level) + " [*a" + std::to_string(level - 1) + ", *a"
                    + std::to_string(level - 1) + "]\n";
    }
    const auto message = validationMessage(document, SchemaKind::ModelLibrary);
    BOOST_CHECK(message.find("document too deeply nested or too large") != std::string::npos);
    // Only that: the incomplete tree is not validated, which would add misleading errors
    BOOST_CHECK_EQUAL(std::ranges::count(message, '\n'), 1);
}

// A null collection is read as absent (deviation L12), in a system as in a library
BOOST_AUTO_TEST_CASE(null_system_sections_are_accepted)
{
    const auto system = YmlSystem::Parser{}.parse(R"(
system:
  id: s
  components:
    - id: c
      model: l.m
      parameters:
      properties:
  connections:
  area-connections:
  thermal-capacity-connections:
)",
                                                  "");
    BOOST_CHECK_EQUAL(system.components.size(), 1);
    BOOST_CHECK(system.components[0].parameters.empty());
    BOOST_CHECK(system.components[0].properties.empty());
    BOOST_CHECK(system.connections.empty());
    BOOST_CHECK(system.areaConnections.empty());
    BOOST_CHECK(system.thermalCapacityConnections.empty());
    BOOST_CHECK_EQUAL(validationMessage("system:\n  components:\n", SchemaKind::System), "");
}

BOOST_AUTO_TEST_CASE(other_deviations_are_accepted)
{
    // Empty id, null capacity-field, all-null area-connection, null library collections
    BOOST_CHECK_EQUAL(validationMessage(R"(
library:
  id: ""
  port-types:
    - id: p
      area-connection:
        injection-to-balance:
        spillage-bound:
        unsupplied-energy-bound:
    - id: t
      thermal-capacity-connection:
        capacity-field:
  models:
)",
                                        SchemaKind::ModelLibrary),
                      "");
    // Numeric and boolean property values, and a parameter value that is not a number
    BOOST_CHECK_EQUAL(validationMessage(R"(
system:
  components:
    - id: c
      model: l.m
      properties:
        - id: a
          value: 1
        - id: b
          value: true
      parameters:
        - id: p
          value: not_a_number
          time-dependent: false
)",
                                        SchemaKind::System),
                      "");
}

BOOST_AUTO_TEST_CASE(deviation_values_are_decoded)
{
    const auto library = YmlModel::Parser{}.parse(R"(
library:
  id: lib
  models:
    - id: m
      variables:
        - id: v
          variable-type: boolean
          location: master
      constraints:
        - id: c
          expression: v <= 1
          location: master
      objective-contributions:
        - id: o
          expression: v
          location: master
)");
    const auto& model = library.models[0];
    BOOST_CHECK(model.variables[0].variable_type == YmlModel::ValueType::BOOL);
    BOOST_CHECK_EQUAL(model.variables[0].location, "master");
    BOOST_CHECK_EQUAL(model.constraints[0].location, "master");
    BOOST_CHECK_EQUAL(model.objectives[0].location, "master");
}

BOOST_AUTO_TEST_SUITE_END()
