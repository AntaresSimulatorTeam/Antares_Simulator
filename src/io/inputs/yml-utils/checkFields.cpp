// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-utils/checkFields.h"

#include <algorithm>
#include <fmt/format.h>
#include <iterator>
#include <unordered_set>
#include <vector>

#include <antares/io/inputs/InputError.h>
#include <antares/logs/logs.h>
#include "antares/io/inputs/yml-utils/YmlTreeDisplayer.h"

namespace Antares::IO::Inputs::YmlUtils
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
    std::ranges::sort(diff);
    return diff;
}

std::string build_message(const std::string& header,
                          const YAML::YmlTreeDisplayer& displayer,
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

void collectFieldErrors(YAML::Node node,
                        const std::vector<Field>& fields,
                        const std::string& path,
                        std::vector<std::string>& errors)
{
    if (!node.IsMap())
    {
        return;
    }
    node.SetTag(path);

    std::unordered_set<std::string> actualKeys;
    std::vector<std::string> duplicated;
    for (const auto& entry: node)
    {
        if (!entry.first.IsScalar())
        {
            errors.push_back(fmt::format("Line {}: expected a scalar field name in '{}'.",
                                         entry.first.Mark().line + 1,
                                         path));
            continue;
        }
        auto key = entry.first.as<std::string>();
        if (!actualKeys.insert(key).second)
        {
            duplicated.push_back(std::move(key));
        }
    }

    std::unordered_set<std::string> mandatoryFields;
    std::unordered_set<std::string> allowedFields;
    for (const auto& field: fields)
    {
        allowedFields.insert(field.name);
        if (field.required)
        {
            mandatoryFields.insert(field.name);
        }
    }

    const auto unexpected = diffSet(actualKeys, allowedFields);
    const auto missing = diffSet(mandatoryFields, actualKeys);

    if (!unexpected.empty() || !missing.empty() || !duplicated.empty())
    {
        YAML::YmlTreeDisplayer displayer(node);
        const auto mark = node.Mark();
        if (!unexpected.empty())
        {
            Antares::logs.warning()
              << fmt::format("Line {} column {}\n{}",
                             mark.line + 1,
                             mark.column + 1,
                             build_message("Unrecognised field(s) will be ignored.",
                                           displayer,
                                           unexpected,
                                           {},
                                           {}));
        }
        if (!missing.empty() || !duplicated.empty())
        {
            errors.push_back(fmt::format("Line {} column {}\n{}",
                                         mark.line + 1,
                                         mark.column + 1,
                                         build_message("Duplicated or missing field(s).",
                                                       displayer,
                                                       {},
                                                       missing,
                                                       duplicated)));
        }
    }

    // Visit every occurrence, including duplicated sections, without looking up ambiguous keys.
    for (const auto& entry: node)
    {
        if (!entry.first.IsScalar())
        {
            continue;
        }
        const auto key = entry.first.as<std::string>();
        const auto field = std::ranges::find(fields, key, &Field::name);
        if (field == fields.end() || field->children.empty())
        {
            continue;
        }
        const auto childPath = path.empty() ? key : path + "/" + key;
        auto child = entry.second;
        if (child.IsSequence())
        {
            for (std::size_t i = 0; i < child.size(); ++i)
            {
                auto item = child[i];
                auto name = "[" + std::to_string(i) + "]";
                if (item.IsMap() && item["id"].IsScalar())
                {
                    name = item["id"].as<std::string>();
                }
                collectFieldErrors(item, field->children, childPath + "/" + name, errors);
            }
        }
        else
        {
            collectFieldErrors(child, field->children, childPath, errors);
        }
    }
}

} // namespace

void checkFields(const YAML::Node& node, const std::vector<Field>& fields, const std::string& path)
{
    std::vector<std::string> errors;
    collectFieldErrors(node, fields, path, errors);
    if (!errors.empty())
    {
        std::string message;
        for (const auto& error: errors)
        {
            message += error + '\n';
        }
        throw InputError(message);
    }
}

} // namespace Antares::IO::Inputs::YmlUtils
