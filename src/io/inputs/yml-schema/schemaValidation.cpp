// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-schema/schemaValidation.h"

#include <algorithm>
#include <array>
#include <charconv>
#include <cstdint>
#include <cstdlib>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonschema/jsonschema.hpp>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <unordered_set>
#include <utility>
#include <vector>

#include <antares/io/inputs/InputError.h>

#include "embeddedSchemas.h"

namespace Antares::IO::Inputs::YmlSchema
{
namespace
{
namespace jsonschema = jsoncons::jsonschema;
using Json = jsoncons::json;

struct Finding
{
    int line = 0;
    int column = 0;
    std::string text;
};

/// Findings are reported with 1-based coordinates. yaml-cpp gives no position (-1) to some nodes,
/// e.g. the root of an empty document: report those at 1:1 rather than at line 0.
Finding makeFinding(const YAML::Mark& mark, std::string text)
{
    return {std::max(mark.line, 0) + 1, std::max(mark.column, 0) + 1, std::move(text)};
}

std::string joinPath(const std::string& path, const std::string& item)
{
    return path.empty() ? item : path + "/" + item;
}

/// Name of a sequence item in diagnostics: its `id` when it has one, its index otherwise.
std::string sequenceItemName(const YAML::Node& item, std::size_t index)
{
    if (item.IsMap())
    {
        const auto id = item["id"];
        if (id.IsDefined() && id.IsScalar())
        {
            return id.as<std::string>();
        }
    }
    return "[" + std::to_string(index) + "]";
}

bool onlyContains(std::string_view text, std::string_view allowed)
{
    return !text.empty() && text.find_first_not_of(allowed) == std::string_view::npos;
}

/// A plain (unquoted) scalar, typed as in the YAML 1.2 core schema.
Json plainScalar(const std::string& text)
{
    if (text.empty() || text == "~" || text == "null" || text == "Null" || text == "NULL")
    {
        return Json::null();
    }
    if (text == "true" || text == "True" || text == "TRUE")
    {
        return Json(true);
    }
    if (text == "false" || text == "False" || text == "FALSE")
    {
        return Json(false);
    }
    if (onlyContains(text, "+-0123456789"))
    {
        std::int64_t integer = 0;
        const char* first = text.data() + (text.front() == '+' ? 1 : 0);
        const auto [end, error] = std::from_chars(first, text.data() + text.size(), integer);
        if (error == std::errc() && end == text.data() + text.size())
        {
            return Json(integer);
        }
    }
    if (onlyContains(text, "+-.0123456789eE"))
    {
        char* end = nullptr;
        const double number = std::strtod(text.c_str(), &end);
        if (end == text.c_str() + text.size())
        {
            return Json(number);
        }
    }
    return Json(text);
}

/// Converts a YAML tree to JSON. yaml-cpp does not type scalars, and duplicated keys would
/// silently disappear in a JSON object, so both are handled here.
///
/// Anchors and aliases are expanded, so a small document can describe an enormous (or, with an
/// alias to one of its own ancestors, infinite) tree. The conversion gives up beyond fixed limits
/// instead of exhausting the stack or the memory; see `aborted()`.
class YamlToJson
{
public:
    /// Real inputs are a handful of levels deep and a few thousand nodes
    static constexpr std::size_t maxDepth = 64;
    static constexpr std::size_t maxNodes = 1'000'000;

    explicit YamlToJson(std::vector<Finding>& findings):
        findings_(findings)
    {
    }

    /// True when a limit was exceeded: the result is then incomplete and must not be validated
    bool aborted() const
    {
        return aborted_;
    }

    Json convert(const YAML::Node& node, const std::string& path)
    {
        if (aborted_)
        {
            return Json::null();
        }
        if (depth_ >= maxDepth || ++nodes_ > maxNodes)
        {
            aborted_ = true;
            findings_.push_back(makeFinding(node.Mark(),
                                            fmt::format("'{}': document too deeply nested or too "
                                                        "large (check anchors and aliases)",
                                                        path)));
            return Json::null();
        }
        ++depth_;
        auto json = convertNode(node, path);
        --depth_;
        return json;
    }

private:
    Json convertNode(const YAML::Node& node, const std::string& path)
    {
        switch (node.Type())
        {
        case YAML::NodeType::Null:
            return Json::null();
        case YAML::NodeType::Scalar:
            return isString(node) ? Json(node.Scalar()) : plainScalar(node.Scalar());
        case YAML::NodeType::Sequence:
            return convertSequence(node, path);
        case YAML::NodeType::Map:
            return convertMap(node, path);
        default:
            return Json::null();
        }
    }

    /// Quoted and block scalars have the non-specific tag "!" in yaml-cpp, `!!str` its own tag:
    /// all are strings whatever they look like
    static bool isString(const YAML::Node& node)
    {
        return node.Tag() == "!" || node.Tag() == "tag:yaml.org,2002:str";
    }

    Json convertSequence(const YAML::Node& node, const std::string& path)
    {
        Json array(jsoncons::json_array_arg);
        std::size_t index = 0;
        for (const auto& item: node)
        {
            array.push_back(convert(item, joinPath(path, sequenceItemName(item, index))));
            ++index;
        }
        return array;
    }

    Json convertMap(const YAML::Node& node, const std::string& path)
    {
        Json object(jsoncons::json_object_arg);
        std::unordered_set<std::string> seen;
        for (const auto& entry: node)
        {
            if (!entry.first.IsScalar())
            {
                // A null key (`~: 1`, `: 1`) has no position of its own: use its value's
                report(entry.first.Mark().is_null() ? entry.second : entry.first,
                       path,
                       entry.first.IsNull()
                         ? "field name must not be empty or null"
                         : "expected a scalar field name, found a mapping or a sequence");
                continue;
            }
            const auto key = entry.first.Scalar();
            if (!seen.insert(key).second)
            {
                report(entry.first, path, fmt::format("duplicated field '{}'", key));
                continue; // the first occurrence wins, as it is the one that is validated
            }
            object.insert_or_assign(key, convert(entry.second, joinPath(path, key)));
        }
        return object;
    }

    void report(const YAML::Node& where, const std::string& path, std::string message)
    {
        findings_.push_back(
          makeFinding(where.Mark(), fmt::format("'{}': {}", path, std::move(message))));
    }

    std::vector<Finding>& findings_;
    std::size_t depth_ = 0;
    std::size_t nodes_ = 0;
    bool aborted_ = false;
};

/// Embedded schema by file name, as a JSON value.
Json loadEmbeddedSchema(std::string_view fileName)
{
    for (const auto& schema: embeddedSchemas())
    {
        if (schema.fileName == fileName)
        {
            std::vector<Finding> findings;
            auto json = YamlToJson(findings).convert(YAML::Load(std::string(schema.content)), "");
            if (!findings.empty())
            {
                throw std::logic_error(
                  fmt::format("Embedded schema '{}' is invalid: {}", fileName, findings[0].text));
            }
            return json;
        }
    }
    throw std::logic_error(fmt::format("No embedded schema named '{}'", fileName));
}

std::string_view schemaFileName(SchemaKind kind)
{
    switch (kind)
    {
    case SchemaKind::ModelLibrary:
        return "model-library.schema.yml";
    case SchemaKind::System:
        return "system.schema.yml";
    }
    throw std::logic_error("Unknown schema kind");
}

/// `$ref`s between schemas are relative to the `$id` of the schema that contains them: resolve
/// them by file name against the embedded copies, never over the network.
Json resolveEmbedded(const jsoncons::uri& uri)
{
    auto text = uri.string();
    text = text.substr(0, text.find('#')); // drop the fragment (`#/$defs/Name`), keep the document
    const auto lastSlash = text.rfind('/');
    const auto name = lastSlash == std::string::npos ? text : text.substr(lastSlash + 1);
    return loadEmbeddedSchema(name);
}

jsonschema::json_schema<Json> compile(SchemaKind kind)
{
    return jsonschema::make_json_schema(loadEmbeddedSchema(schemaFileName(kind)), resolveEmbedded);
}

const jsonschema::json_schema<Json>& compiledSchema(SchemaKind kind)
{
    // Compiled once, on first use. Validation is const, so the compiled schemas can be shared
    // between threads.
    static const std::array<jsonschema::json_schema<Json>, 2> schemas = {
      compile(SchemaKind::ModelLibrary),
      compile(SchemaKind::System)};
    switch (kind)
    {
    case SchemaKind::ModelLibrary:
        return schemas[0];
    case SchemaKind::System:
        return schemas[1];
    }
    throw std::logic_error("Unknown schema kind");
}

/// Finds the YAML node a JSON pointer designates, to report its line and column and a path that
/// reads like the document. When the pointer leads nowhere (e.g. a missing required key), the
/// deepest node that exists is returned.
struct Location
{
    YAML::Node node;
    std::string path;
    YAML::Mark mark;    ///< Position to report: yaml-cpp gives no position to null values
    YAML::Mark keyMark; ///< Position of the key holding the node, when it is a mapping value
};

Location locate(const YAML::Node& root, const jsoncons::jsonpointer::json_pointer& pointer)
{
    Location current{root, "", root.Mark(), root.Mark()};
    for (const auto& token: pointer)
    {
        YAML::Node next;
        YAML::Mark mark = current.mark;
        YAML::Mark keyMark = current.mark;
        std::string name = token;
        if (current.node.IsMap())
        {
            for (const auto& entry: current.node)
            {
                if (entry.first.IsScalar() && entry.first.Scalar() == token)
                {
                    next.reset(entry.second);
                    keyMark = entry.first.Mark();
                    // A null value has no position of its own (or one past the end of its line):
                    // report the key
                    mark = entry.second.IsNull() || entry.second.Mark().is_null()
                             ? keyMark
                             : entry.second.Mark();
                    break;
                }
            }
        }
        else if (current.node.IsSequence())
        {
            std::size_t wanted = 0;
            const auto [end, error] = std::from_chars(token.data(),
                                                      token.data() + token.size(),
                                                      wanted);
            if (error == std::errc())
            {
                // Iterate rather than `node[index]`: yaml-cpp's indexed access does not find
                // elements that follow a null or scalar element
                std::size_t index = 0;
                for (const auto& item: current.node)
                {
                    if (index == wanted)
                    {
                        next.reset(item);
                        mark = item.IsNull() || item.Mark().is_null() ? current.mark : item.Mark();
                        keyMark = mark;
                        name = sequenceItemName(item, index);
                        break;
                    }
                    ++index;
                }
            }
        }
        if (!next.IsDefined())
        {
            break;
        }
        // Never assign a YAML::Node: unlike a pointer, `operator=` overwrites the node it refers to
        // inside the document. `reset` rebinds the handle only.
        current.node.reset(next);
        current.path = joinPath(current.path, name);
        current.mark = mark;
        current.keyMark = keyMark;
    }
    return current;
}

std::string describe(const jsonschema::validation_message& message)
{
    auto text = message.message();
    if (!message.details().empty())
    {
        std::vector<std::string> details;
        for (const auto& detail: message.details())
        {
            details.push_back(detail.message());
        }
        text += fmt::format(" ({})", fmt::join(details, "; "));
    }
    return text;
}

} // namespace

namespace
{
[[noreturn]] void fail(std::vector<Finding> findings, const std::string& context)
{
    std::ranges::stable_sort(findings,
                             [](const Finding& a, const Finding& b)
                             { return std::pair(a.line, a.column) < std::pair(b.line, b.column); });

    std::string report = context.empty()
                           ? "YAML schema validation failed:"
                           : fmt::format("{}: YAML schema validation failed:", context);
    for (const auto& finding: findings)
    {
        report += fmt::format("\n- Line {} column {}: {}",
                              finding.line,
                              finding.column,
                              finding.text);
    }
    throw InputError(report);
}
} // namespace

void validate(const YAML::Node& root, SchemaKind kind, const std::string& context)
{
    std::vector<Finding> findings;
    YamlToJson converter(findings);
    const auto instance = converter.convert(root, "");
    if (converter.aborted())
    {
        // The tree is incomplete: validating it would only add misleading errors
        fail(std::move(findings), context);
    }

    compiledSchema(kind).validate(
      instance,
      [&](const jsonschema::validation_message& message)
      {
          const auto where = locate(root, message.instance_location());
          // An unknown key is best pointed at by the key itself, any
          // other error by the offending value
          const auto mark = message.keyword() == "additionalProperties" ? where.keyMark
                                                                        : where.mark;
          findings.push_back(
            makeFinding(mark, fmt::format("'{}': {}", where.path, describe(message))));
          return jsonschema::walk_result::advance;
      });

    if (!findings.empty())
    {
        fail(std::move(findings), context);
    }
}

} // namespace Antares::IO::Inputs::YmlSchema
