// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/io/inputs/InputError.h>
#include <antares/io/inputs/yml-model/decoders.h>
#include <antares/io/inputs/yml-model/parser.h>
#include <antares/io/inputs/yml-optim-config/parser.h>
#include <antares/io/inputs/yml-system/parser.h>
#include <antares/io/inputs/yml-utils/YmlTreeDisplayer.h>
#include <antares/io/inputs/yml-utils/checkFields.h>

using namespace Antares::IO::Inputs;
using Antares::UnitTests::CaptureAntaresLogs;

BOOST_AUTO_TEST_SUITE(FieldValidation)

BOOST_AUTO_TEST_CASE(library_collects_errors_in_nested_and_sibling_nodes)
{
    const auto content = R"(
library:
  port-types:
    - id: p
      fields:
        - unexpected: value
      area-connection:
        price: p
  models:
    - id: m
      ports:
        - id: missing_type
      constraints:
        - id: missing_expression
)";
    CaptureAntaresLogs logs;
    BOOST_CHECK_EXCEPTION(
      YmlModel::Parser{}.parse(content),
      InputError,
      [](const InputError& e)
      {
          const std::string message = e.what();
          BOOST_CHECK(message.find("Missing field: id") != std::string::npos);
          BOOST_CHECK(message.find("Missing field: injection-to-balance") != std::string::npos);
          BOOST_CHECK(message.find("Missing field: spillage-bound") != std::string::npos);
          BOOST_CHECK(message.find("Missing field: unsupplied-energy-bound") != std::string::npos);
          BOOST_CHECK(message.find("Missing field: type") != std::string::npos);
          BOOST_CHECK(message.find("Missing field: expression") != std::string::npos);
          BOOST_CHECK(message.find("|__ [0]") != std::string::npos);
          BOOST_CHECK(message.find("|__ missing_type") != std::string::npos);
          return true;
      });
    BOOST_REQUIRE_EQUAL(logs.getWarnings().size(), 1);
    BOOST_CHECK(logs.getWarnings().begin()->find("Unrecognised field: unexpected")
                != std::string::npos);
}

BOOST_AUTO_TEST_CASE(system_field_errors_are_reported_before_value_conversions)
{
    const auto content = R"(
system:
  components:
    - id: malformed_value
      model: lib.model
      parameters:
        - id: p
          value: [1, 2]
    - id: missing_model
      parameters:
        - id: missing_value
    - model: lib.model
  connections:
    - component1: c
)";
    BOOST_CHECK_EXCEPTION(
      YmlSystem::Parser{}.parse(content, ""),
      InputError,
      [](const InputError& e)
      {
          const std::string message = e.what();
          BOOST_CHECK(message.find("Missing field: model") != std::string::npos);
          BOOST_CHECK(message.find("Missing field: value") != std::string::npos);
          BOOST_CHECK(message.find("Missing field: id") != std::string::npos);
          BOOST_CHECK(message.find("Missing field: component2") != std::string::npos);
          BOOST_CHECK(message.find("Missing field: port1") != std::string::npos);
          BOOST_CHECK(message.find("Missing field: port2") != std::string::npos);
          return true;
      });
}

BOOST_AUTO_TEST_CASE(optim_config_collects_errors_before_resolution_mode_conversion)
{
    const auto content = R"(
resolution-mode: [invalid]
models:
  - model-decomposition:
      variables:
        - id: missing_location
      constraints:
        - location: master
    out-of-bounds-processing:
      constraints:
        - mode: drop
)";
    BOOST_CHECK_EXCEPTION(
      YmlOptimConfig::Parser{}.parse(content),
      InputError,
      [](const InputError& e)
      {
          const std::string message = e.what();
          BOOST_CHECK(message.find("Missing field: id") != std::string::npos);
          BOOST_CHECK(message.find("Missing field: location") != std::string::npos);
          BOOST_CHECK(message.find("|__ model-decomposition") != std::string::npos);
          BOOST_CHECK(message.find("|__ out-of-bounds-processing") != std::string::npos);
          return true;
      });
}

BOOST_AUTO_TEST_CASE(optim_config_collects_missing_model_decomposition_with_sibling_errors)
{
    const auto content = R"(
models:
  - id: first
  - model-decomposition:
      variables: []
      constraints: []
      objective-contributions: []
)";
    BOOST_CHECK_EXCEPTION(YmlOptimConfig::Parser{}.parse(content),
                          InputError,
                          [](const InputError& e)
                          {
                              const std::string message = e.what();
                              BOOST_CHECK(message.find("Missing field: model-decomposition")
                                          != std::string::npos);
                              BOOST_CHECK(message.find("Missing field: id") != std::string::npos);
                              return true;
                          });
}

BOOST_AUTO_TEST_CASE(duplicated_sections_are_all_validated)
{
    const auto content = R"(
system:
  components:
    - id: first
  components:
    - model: lib.model
)";
    BOOST_CHECK_EXCEPTION(YmlSystem::Parser{}.parse(content, ""),
                          InputError,
                          [](const InputError& e)
                          {
                              const std::string message = e.what();
                              BOOST_CHECK(message.find("Duplicated field: components")
                                          != std::string::npos);
                              BOOST_CHECK(message.find("Missing field: model")
                                          != std::string::npos);
                              BOOST_CHECK(message.find("Missing field: id") != std::string::npos);
                              return true;
                          });
}

BOOST_AUTO_TEST_CASE(unknown_fields_warn_and_preserve_decoded_values)
{
    const auto content = R"(
library:
  id: lib
  models:
    - id: m
      variables:
        - id: p
          lower-bound: 3
          lower_bound: 7
      extra-output: ignored
unexpected-root-key: ignored
)";
    CaptureAntaresLogs logs;
    const auto library = YmlModel::Parser{}.parse(content);
    BOOST_REQUIRE_EQUAL(library.models.size(), 1);
    BOOST_CHECK_EQUAL(library.models[0].id, "m");
    BOOST_REQUIRE_EQUAL(library.models[0].variables.size(), 1);
    BOOST_CHECK_EQUAL(library.models[0].variables[0].lower_bound.input_expr, "3");
    BOOST_CHECK(library.models[0].extra_outputs.empty());
    BOOST_REQUIRE_EQUAL(logs.getWarnings().size(), 3);
    std::string warnings;
    for (const auto& warning: logs.getWarnings())
    {
        warnings += warning;
    }
    BOOST_CHECK(warnings.find("Unrecognised field: lower_bound") != std::string::npos);
    BOOST_CHECK(warnings.find("Unrecognised field: extra-output") != std::string::npos);
    BOOST_CHECK(warnings.find("Unrecognised field: unexpected-root-key") != std::string::npos);
    BOOST_CHECK(warnings.find("|__X lower_bound at line 9") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(misplaced_scenario_group_warns)
{
    const auto content = R"(
system:
  components:
    - id: c
      model: lib.model
      scenario-group: correct
      parameters:
        - id: p
          value: data
          scenario-group: misplaced
)";
    CaptureAntaresLogs logs;
    const auto system = YmlSystem::Parser{}.parse(content, "");
    BOOST_REQUIRE_EQUAL(system.components.size(), 1);
    BOOST_CHECK_EQUAL(system.components[0].scenarioGroup, "correct");
    BOOST_REQUIRE_EQUAL(logs.getWarnings().size(), 1);
    BOOST_CHECK(logs.getWarnings().begin()->find("Unrecognised field: scenario-group")
                != std::string::npos);
}

BOOST_AUTO_TEST_CASE(omitted_out_of_bounds_mode_defaults_to_cyclic)
{
    const auto content = R"(
models:
  - id: lib.model
    model-decomposition: {}
    out-of-bounds-processing:
      constraints:
        - id: balance
)";
    CaptureAntaresLogs logs;
    const auto config = YmlOptimConfig::Parser{}.parse(content);
    BOOST_REQUIRE_EQUAL(config.models.size(), 1);
    BOOST_REQUIRE_EQUAL(config.models[0].constraints_out_of_bounds_processing.size(), 1);
    BOOST_CHECK_EQUAL(config.models[0].constraints_out_of_bounds_processing[0].mode, "cyclic");
    BOOST_CHECK(logs.getWarnings().empty());
}

BOOST_AUTO_TEST_CASE(validation_state_does_not_leak_between_parses)
{
    YmlSystem::Parser parser;
    BOOST_CHECK_THROW(parser.parse("system: {components: [{id: c}]}", ""), InputError);
    const auto system = parser.parse("system: {id: valid}", "");
    BOOST_CHECK_EQUAL(system.id, "valid");
    BOOST_CHECK(system.components.empty());
}

BOOST_AUTO_TEST_CASE(complex_keys_do_not_hide_other_field_errors)
{
    const auto node = YAML::Load(R"(
? [invalid, key]
: ignored
children:
  - {}
)");
    BOOST_CHECK_EXCEPTION(
      (YmlUtils::checkFields(node, {{"id", true}, {"children", false, {{"value", true}}}})),
      InputError,
      [](const InputError& e)
      {
          const std::string message = e.what();
          BOOST_CHECK(message.find("expected a scalar field name") != std::string::npos);
          BOOST_CHECK(message.find("Missing field: id") != std::string::npos);
          BOOST_CHECK(message.find("Missing field: value") != std::string::npos);
          return true;
      });
}

BOOST_AUTO_TEST_CASE(non_scalar_ids_do_not_hide_sibling_field_errors)
{
    const auto content = R"(
library:
  id: [invalid]
  models:
    - id: [invalid]
      variables:
        - lower-bound: 0
    - id: m
      ports:
        - id: p
)";
    BOOST_CHECK_EXCEPTION(YmlModel::Parser{}.parse(content),
                          InputError,
                          [](const InputError& e)
                          {
                              const std::string message = e.what();
                              BOOST_CHECK(message.find("Missing field: id") != std::string::npos);
                              BOOST_CHECK(message.find("Missing field: type") != std::string::npos);
                              return true;
                          });
}

BOOST_AUTO_TEST_CASE(validation_preserves_document_tags_on_success_and_failure)
{
    auto node = YAML::Load(R"(
children:
  - id: child
    unexpected: ignored
)");
    node.SetTag("original-root");
    node["children"].SetTag("original-sequence");
    node["children"][0].SetTag("original-child");
    const auto before = YAML::Dump(node);
    CaptureAntaresLogs logs;
    BOOST_CHECK_NO_THROW(
      (YmlUtils::checkFields(node, {{"children", false, {{"id", true}}}}, "explicit-path")));
    BOOST_CHECK_EQUAL(YAML::Dump(node), before);
    BOOST_REQUIRE_EQUAL(logs.getWarnings().size(), 1);
    BOOST_CHECK(logs.getWarnings().begin()->find("explicit-path") != std::string::npos);
    BOOST_CHECK(logs.getWarnings().begin()->find("original-root") == std::string::npos);
    BOOST_CHECK_THROW(
      (YmlUtils::checkFields(node, {{"children", false, {{"id", true}, {"value", true}}}})),
      InputError);
    BOOST_CHECK_EQUAL(YAML::Dump(node), before);
}

BOOST_AUTO_TEST_CASE(mixed_findings_keep_warnings_separate_from_errors)
{
    const auto node = YAML::Load("id: first\nid: second\nunexpected: ignored\n");
    CaptureAntaresLogs logs;
    BOOST_CHECK_EXCEPTION((YmlUtils::checkFields(node, {{"id", true}, {"value", true}}, "root")),
                          InputError,
                          [](const InputError& e)
                          {
                              const std::string message = e.what();
                              BOOST_CHECK(message.find("Missing field: value")
                                          != std::string::npos);
                              BOOST_CHECK(message.find("Duplicated field: id")
                                          != std::string::npos);
                              BOOST_CHECK(message.find("Unrecognised field") == std::string::npos);
                              return true;
                          });
    BOOST_REQUIRE_EQUAL(logs.getWarnings().size(), 1);
    const auto& warning = *logs.getWarnings().begin();
    BOOST_CHECK(warning.find("Unrecognised field: unexpected") != std::string::npos);
    BOOST_CHECK(warning.find("Missing field") == std::string::npos);
    BOOST_CHECK(warning.find("Duplicated field") == std::string::npos);
}

BOOST_AUTO_TEST_CASE(null_model_ids_are_rejected_without_tag_context)
{
    auto node = YAML::Load("id: null");
    node.SetTag("unrelated-tag");
    BOOST_CHECK_EXCEPTION(YAML::checkMandatoryIdField(node, "model"),
                          YAML::KeyNotFound,
                          [](const YAML::KeyNotFound& e)
                          {
                              const std::string message = e.what();
                              BOOST_CHECK(message.find("model id is mandatory in library")
                                          != std::string::npos);
                              BOOST_CHECK(message.find("unrelated-tag") == std::string::npos);
                              return true;
                          });
}

BOOST_AUTO_TEST_CASE(nested_unknown_field_warning_matches_complete_tree)
{
    const auto node = YAML::Load("children:\n"
                                 "  - id: generator\n"
                                 "    typo: ignored\n");
    CaptureAntaresLogs logs;
    YmlUtils::checkFields(node, {{"children", false, {{"id", true}}}}, "library");
    BOOST_REQUIRE_EQUAL(logs.getWarnings().size(), 1);
    BOOST_CHECK_EQUAL(*logs.getWarnings().begin(),
                      "Line 2 column 5\n"
                      "YAML field validation:\n"
                      "- Unrecognised field: typo\n"
                      "\n"
                      "library\n"
                      "|__ children\n"
                      "    |__ generator\n"
                      "        |__X typo at line 3\n");
}

BOOST_AUTO_TEST_CASE(missing_and_duplicated_fields_match_complete_tree)
{
    const auto node = YAML::Load("children:\n"
                                 "  - id: generator\n"
                                 "    id: duplicate\n");
    BOOST_CHECK_EXCEPTION((YmlUtils::checkFields(
                            node,
                            {{"children", false, {{"id", true}, {"value", true}, {"model", true}}}},
                            "library")),
                          InputError,
                          [](const InputError& e)
                          {
                              BOOST_CHECK_EQUAL(e.what(),
                                                "Line 2 column 5\n"
                                                "YAML field validation:\n"
                                                "- Missing field: model\n"
                                                "- Missing field: value\n"
                                                "- Duplicated field: id\n"
                                                "\n"
                                                "library\n"
                                                "|__ children\n"
                                                "    |__ generator\n"
                                                "        |__X id at line 2\n"
                                                "        |__? model (missing)\n"
                                                "        |__? value (missing)\n"
                                                "\n");
                              return true;
                          });
}

BOOST_AUTO_TEST_CASE(sibling_errors_match_complete_trees_with_index_fallback)
{
    const auto node = YAML::Load("children:\n"
                                 "  - id: named\n"
                                 "  - id: null\n"
                                 "  - {}\n");
    BOOST_CHECK_EXCEPTION(
      (YmlUtils::checkFields(node, {{"children", false, {{"value", true}, {"id"}}}}, "root")),
      InputError,
      [](const InputError& e)
      {
          BOOST_CHECK_EQUAL(e.what(),
                            "Line 2 column 5\n"
                            "YAML field validation:\n"
                            "- Missing field: value\n"
                            "\n"
                            "root\n"
                            "|__ children\n"
                            "    |__ named\n"
                            "        |__? value (missing)\n"
                            "\n"
                            "Line 3 column 5\n"
                            "YAML field validation:\n"
                            "- Missing field: value\n"
                            "\n"
                            "root\n"
                            "|__ children\n"
                            "    |__ [1]\n"
                            "        |__? value (missing)\n"
                            "\n"
                            "Line 4 column 5\n"
                            "YAML field validation:\n"
                            "- Missing field: value\n"
                            "\n"
                            "root\n"
                            "|__ children\n"
                            "    |__ [2]\n"
                            "        |__? value (missing)\n"
                            "\n");
          return true;
      });
}

BOOST_AUTO_TEST_CASE(root_warning_matches_complete_message_without_path)
{
    const auto node = YAML::Load("z: ignored\na: ignored\n");
    CaptureAntaresLogs logs;
    YmlUtils::checkFields(node, {});
    BOOST_REQUIRE_EQUAL(logs.getWarnings().size(), 1);
    BOOST_CHECK_EQUAL(*logs.getWarnings().begin(),
                      "Line 1 column 1\n"
                      "YAML field validation:\n"
                      "- Unrecognised field: a\n"
                      "- Unrecognised field: z\n"
                      "\n"
                      "|__X a at line 2\n"
                      "|__X z at line 1\n");
}

BOOST_AUTO_TEST_CASE(tree_displayer_handles_unmarked_and_unknown_keys)
{
    YAML::Node node(YAML::NodeType::Map);
    node["unmarked"] = "value";
    const YAML::YmlTreeDisplayer displayer(node, "root/child");
    BOOST_CHECK_EQUAL(displayer.baseTree(), "root\n|__ child\n");
    BOOST_CHECK_EQUAL(displayer.buildMarkedTree({"unmarked", "absent"}, {"required"}),
                      "    |__X unmarked\n"
                      "    |__X absent\n"
                      "    |__? required (missing)\n");
}

BOOST_AUTO_TEST_CASE(system_parser_warning_matches_complete_tree)
{
    CaptureAntaresLogs logs;
    const auto system = YmlSystem::Parser{}.parse("system:\n"
                                                  "  components:\n"
                                                  "    - id: generator\n"
                                                  "      model: lib.model\n"
                                                  "      typo: ignored\n",
                                                  "");
    BOOST_REQUIRE_EQUAL(system.components.size(), 1);
    BOOST_CHECK_EQUAL(system.components[0].model, "lib.model");
    BOOST_REQUIRE_EQUAL(logs.getWarnings().size(), 1);
    BOOST_CHECK_EQUAL(*logs.getWarnings().begin(),
                      "Line 3 column 7\n"
                      "YAML field validation:\n"
                      "- Unrecognised field: typo\n"
                      "\n"
                      "system\n"
                      "|__ components\n"
                      "    |__ generator\n"
                      "        |__X typo at line 5\n");
}

BOOST_AUTO_TEST_CASE(optim_config_parser_error_matches_complete_tree)
{
    BOOST_CHECK_EXCEPTION(YmlOptimConfig::Parser{}.parse("models:\n"
                                                         "  - id: lib.model\n"
                                                         "    model-decomposition:\n"
                                                         "      variables:\n"
                                                         "        - id: power\n"),
                          InputError,
                          [](const InputError& e)
                          {
                              BOOST_CHECK_EQUAL(e.what(),
                                                "Line 5 column 11\n"
                                                "YAML field validation:\n"
                                                "- Missing field: location\n"
                                                "\n"
                                                "optim-config\n"
                                                "|__ models\n"
                                                "    |__ lib.model\n"
                                                "        |__ model-decomposition\n"
                                                "            |__ variables\n"
                                                "                |__ power\n"
                                                "                    |__? location (missing)\n"
                                                "\n");
                              return true;
                          });
}

BOOST_AUTO_TEST_CASE(non_map_sequence_items_do_not_hide_other_field_errors)
{
    const auto node = YAML::Load("children:\n  - scalar\n  - null\n  - {}\n");
    BOOST_CHECK_EXCEPTION(
      (YmlUtils::checkFields(node, {{"children", false, {{"value", true}}}}, "root")),
      InputError,
      [](const InputError& e)
      {
          const std::string message = e.what();
          BOOST_CHECK(message.find("Line 2: expected a YAML mapping in 'root/children/[0]'.")
                      != std::string::npos);
          BOOST_CHECK(message.find("Line 3: expected a YAML mapping in 'root/children/[1]'.")
                      != std::string::npos);
          BOOST_CHECK(message.find("Missing field: value") != std::string::npos);
          BOOST_CHECK(message.find("|__ [2]") != std::string::npos);
          return true;
      });
}

BOOST_AUTO_TEST_SUITE_END()
