// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-utils/checkFields.h"

#include <algorithm>
#include <fmt/format.h>
#include <iterator>
#include <vector>

#include <antares/io/inputs/InputError.h>
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

std::string build_message(const std::string& header,
                          const YmlTreeDisplayer& displayer,
                          const std::vector<std::string>& unexpected,
                          const std::vector<std::string>& missing,
                          const std::vector<std::string>& duplicated)
{
    // Duplicated keys are highlighted like unexpected ones
    auto markedUnexpected = unexpected;
    markedUnexpected.insert(markedUnexpected.end(), duplicated.begin(), duplicated.end());

    std::string fieldsList;
    for (const auto& f: unexpected)
    {
        fieldsList += fmt::format("- Unrecognised field: {}\n", f);
    }
    for (const auto& f: missing)
    {
        fieldsList += fmt::format("- Missing field: {}\n", f);
    }

    for (const auto& f: duplicated)
    {
        fieldsList += fmt::format("- Duplicated field: {}\n", f);
    }

    return fmt::format("{}\n{}\n{}{}",
                       header,
                       fieldsList,
                       displayer.baseTree(),
                       displayer.buildMarkedTree(markedUnexpected, missing));
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
    std::vector<std::string> duplicated;
    for (const auto& entry: node)
    {
        const Node keyNode = entry.first;
        auto key = keyNode.IsDefined() ? keyNode.as<std::string>() : std::string("<unknown>");
        if (!actualKeys.insert(key).second)
        {
            duplicated.push_back(std::move(key));
        }
    }

    std::unordered_set<std::string> allowedFields = mandatoryFields;
    allowedFields.insert(optionalFields.begin(), optionalFields.end());

    const auto unexpected = diffSet(actualKeys, allowedFields);
    const auto missing = diffSet(mandatoryFields, actualKeys);

    if (unexpected.empty() && missing.empty() && duplicated.empty())
    {
        return; // valid
    }

    // Invalid map: now build the displayer for reporting
    YmlTreeDisplayer displayer(node);

    const auto mark = node.Mark();
    throw Antares::IO::Inputs::InputError(fmt::format(
      "Line {} column {}\n{}",
      mark.line + 1,
      mark.column + 1,
      build_message(fmt::format("Unexpected, duplicated or missing field(s) (expected {} "
                                "field(s)).",
                                allowedFields.size()),
                    displayer,
                    unexpected,
                    missing,
                    duplicated)));
}

} // namespace YAML
