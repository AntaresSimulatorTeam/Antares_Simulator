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
struct Findings
{
    std::vector<std::string> unexpected = {};
    std::vector<std::string> missing = {};
    std::vector<std::string> duplicated = {};
    std::vector<std::string> nullIds = {};
};

struct ValidationResult
{
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
};

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

std::string buildMessage(const YAML::Node& node, const std::string& path, const Findings& findings)
{
    const YAML::YmlTreeDisplayer displayer(node, path);
    // Duplicated keys are highlighted like unexpected ones
    auto markedUnexpected = findings.unexpected;
    markedUnexpected.insert(markedUnexpected.end(),
                            findings.duplicated.begin(),
                            findings.duplicated.end());
    markedUnexpected.insert(markedUnexpected.end(),
                            findings.nullIds.begin(),
                            findings.nullIds.end());

    std::string fieldsList;
    for (const auto& f: findings.unexpected)
    {
        fieldsList += fmt::format("- Unrecognised field: {}\n", f);
    }
    for (const auto& f: findings.missing)
    {
        fieldsList += fmt::format("- Missing field: {}\n", f);
    }

    for (const auto& f: findings.duplicated)
    {
        fieldsList += fmt::format("- Duplicated field: {}\n", f);
    }
    for (const auto& f: findings.nullIds)
    {
        fieldsList += fmt::format("- Invalid null value for required field: {}\n", f);
    }

    const auto mark = node.Mark();
    return fmt::format("Line {} column {}\nYAML field validation:\n{}\n{}{}",
                       mark.line + 1,
                       mark.column + 1,
                       fieldsList,
                       displayer.baseTree(),
                       displayer.buildMarkedTree(markedUnexpected, findings.missing));
}

void validateMap(const YAML::Node& node,
                 const std::vector<Field>& fields,
                 const std::string& path,
                 ValidationResult& result)
{
    std::unordered_set<std::string> actualKeys;
    std::vector<std::string> duplicated;
    for (const auto& entry: node)
    {
        if (!entry.first.IsScalar())
        {
            result.errors.push_back(fmt::format("Line {}: expected a scalar field name in '{}'.",
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
    std::vector<std::string> nullIds;
    for (const auto& field: fields)
    {
        allowedFields.insert(field.name);
        if (field.required)
        {
            mandatoryFields.insert(field.name);
            if (field.name == "id" && node[field.name].IsNull())
            {
                nullIds.push_back(field.name);
            }
        }
    }

    const auto unexpected = diffSet(actualKeys, allowedFields);
    const auto missing = diffSet(mandatoryFields, actualKeys);

    if (!unexpected.empty())
    {
        result.warnings.push_back(buildMessage(node, path, {.unexpected = unexpected}));
    }
    if (!missing.empty() || !duplicated.empty() || !nullIds.empty())
    {
        result.errors.push_back(
          buildMessage(node,
                       path,
                       {.missing = missing, .duplicated = duplicated, .nullIds = nullIds}));
    }
}

std::string sequenceItemName(const YAML::Node& node, std::size_t index)
{
    if (node.IsMap())
    {
        const auto id = node["id"];
        if (id.IsDefined() && id.IsScalar())
        {
            return id.as<std::string>();
        }
    }
    return "[" + std::to_string(index) + "]";
}

void collectFieldErrors(const YAML::Node& node,
                        const std::vector<Field>& fields,
                        const std::string& path,
                        ValidationResult& result)
{
    if (!node.IsMap())
    {
        return;
    }
    validateMap(node, fields, path, result);

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
        const auto& child = entry.second;
        if (child.IsSequence())
        {
            for (std::size_t i = 0; i < child.size(); ++i)
            {
                auto item = child[i];
                const auto itemPath = childPath + "/" + sequenceItemName(item, i);
                if (!item.IsMap())
                {
                    result.errors.push_back(fmt::format("Line {}: expected a YAML mapping in '{}'.",
                                                        item.Mark().line + 1,
                                                        itemPath));
                    continue;
                }
                collectFieldErrors(item, field->children, itemPath, result);
            }
        }
        else
        {
            collectFieldErrors(child, field->children, childPath, result);
        }
    }
}

ValidationResult validateFields(const YAML::Node& node,
                                const std::vector<Field>& fields,
                                const std::string& path)
{
    ValidationResult result;
    collectFieldErrors(node, fields, path, result);
    return result;
}

} // namespace

void checkFields(const YAML::Node& node, const std::vector<Field>& fields, const std::string& path)
{
    const auto result = validateFields(node, fields, path);
    for (const auto& warning: result.warnings)
    {
        Antares::logs.warning() << warning;
    }
    if (!result.errors.empty())
    {
        std::string message;
        for (const auto& error: result.errors)
        {
            message += error + '\n';
        }
        throw InputError(message);
    }
}

} // namespace Antares::IO::Inputs::YmlUtils
