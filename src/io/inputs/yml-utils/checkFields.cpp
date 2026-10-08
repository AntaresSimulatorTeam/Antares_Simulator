// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-utils/checkFields.h"

#include <algorithm>
#include <fmt/format.h>
#include <iterator>
#include <vector>

#include "antares/io/inputs/yml-utils/YmlTreeDisplayer.h"

namespace YAML
{

namespace
{
std::vector<std::string> diffSet(const std::unordered_set<std::string>& setA,
                                 const std::unordered_set<std::string>& setB)
{
    std::vector<std::string> diff;
    std::ranges::copy_if(setA,
                         std::back_inserter(diff),
                         [&setB](const auto& item) { return !setB.contains(item); });
    return diff;
}

std::string build_error_message(const size_t& nbFieldsAllowed,
                                const YmlTreeDisplayer& displayer,
                                const std::vector<std::string>& unexpected,
                                const std::vector<std::string>& missing)
{
    // Build a readable list of errors (one per line), then append the tree
    std::string errors_list;
    for (const auto& f: unexpected)
    {
        errors_list += fmt::format("- Unexpected field: {}\n", f);
    }
    for (const auto& f: missing)
    {
        errors_list += fmt::format("- Missing field: {}\n", f);
    }

    // Final message: brief header, individual errors, then the tree
    const std::string message = fmt::format(
      "Unexpected or missing field(s) (expected {} field(s)).\n{}\n{}{}",
      nbFieldsAllowed,
      errors_list,
      displayer.baseTree(),
      displayer.buildMarkedTree(unexpected, missing));

    return message;
}

} // namespace

void checkFields(const Node& node,
                 const std::unordered_set<std::string>& mandatoryFields,
                 const std::unordered_set<std::string>& optionalFields)
{
    if (!node.IsDefined() || !node.IsMap())
    {
        return;
    }

    // Extract actual key names (cheap, no line-number tracking yet)
    std::unordered_set<std::string> actualKeys;
    for (const auto& entry: node)
    {
        const Node keyNode = entry.first;
        actualKeys.insert(keyNode.IsDefined() ? keyNode.as<std::string>()
                                              : std::string("<unknown>"));
    }

    std::unordered_set<std::string> allowedFields = mandatoryFields;
    allowedFields.insert(optionalFields.begin(), optionalFields.end());

    const auto unexpected = diffSet(actualKeys, allowedFields);
    const auto missing = diffSet(mandatoryFields, actualKeys);

    if (unexpected.empty() && missing.empty())
    {
        return; // valid
    }

    // Invalid map: now build the displayer for error reporting
    YmlTreeDisplayer displayer(node);

    const std::string message = build_error_message(allowedFields.size(),
                                                    displayer,
                                                    unexpected,
                                                    missing);

    throw Exception(node.Mark(), message);
}

} // namespace YAML
