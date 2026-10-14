// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-utils/checkFields.h"

#include <algorithm>
#include <fmt/format.h>
#include <iterator>
#include <vector>

#include <antares/io/inputs/InputError.h>
#include <antares/logs/logs.h>

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
                          const std::vector<std::string>& missing)
{
    std::string fieldsList;
    for (const auto& f: unexpected)
    {
        fieldsList += fmt::format("- Unrecognised field: {}\n", f);
    }
    for (const auto& f: missing)
    {
        fieldsList += fmt::format("- Missing field: {}\n", f);
    }

    return fmt::format("{}\n{}\n{}{}",
                       header,
                       fieldsList,
                       displayer.baseTree(),
                       displayer.buildMarkedTree(unexpected, missing));
}

} // namespace

void checkFields(const Node& node,
                 const std::unordered_set<std::string>& mandatoryFields,
                 const std::unordered_set<std::string>& optionalFields,
                 UnexpectedFields unexpectedPolicy)
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

    // Invalid map: now build the displayer for reporting
    YmlTreeDisplayer displayer(node);

    // Missing mandatory fields are always fatal; unrecognised ones depend on the policy
    if (!missing.empty() || unexpectedPolicy == UnexpectedFields::Throw)
    {
            const auto mark = node.Mark();
        throw Antares::IO::Inputs::InputError(
          fmt::format("Line {} column {}\n{}",
                      mark.line + 1,
                      mark.column + 1,
                      build_message(fmt::format("Unexpected or missing field(s) (expected {} "
                                                  "field(s)).",
                                                  allowedFields.size()),
                                    displayer,
                                    unexpected,
                                    missing)));
    }

    Antares::logs.warning()
      << build_message("Unrecognised field(s) will be ignored.", displayer, unexpected, missing);
}

} // namespace YAML
