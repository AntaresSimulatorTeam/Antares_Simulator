// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <algorithm>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace YmlTest
{

// Removes the common leading whitespace of a raw string, so that test YAML can be written with
// natural C++ indentation. Leading/trailing blank lines are dropped.
inline std::string dedent(std::string_view text)
{
    std::vector<std::string> lines;
    std::istringstream in{std::string(text)};
    for (std::string line; std::getline(in, line);)
    {
        lines.push_back(line);
    }
    auto isBlank = [](const std::string& l) { return l.find_first_not_of(" \t") == l.npos; };
    while (!lines.empty() && isBlank(lines.front()))
    {
        lines.erase(lines.begin());
    }
    while (!lines.empty() && isBlank(lines.back()))
    {
        lines.pop_back();
    }

    size_t common = std::string::npos;
    for (const auto& l: lines)
    {
        if (!isBlank(l))
        {
            common = std::min(common, l.find_first_not_of(' '));
        }
    }

    std::string result;
    for (const auto& l: lines)
    {
        result += (isBlank(l) ? "" : l.substr(common)) + "\n";
    }
    return result;
}

inline std::string indent(const std::string& text, size_t spaces)
{
    std::string result;
    std::istringstream in(text);
    for (std::string line; std::getline(in, line);)
    {
        result += (line.empty() ? "" : std::string(spaces, ' ') + line) + "\n";
    }
    return result;
}

// "key: value" for inline values ("[]", scalars), "key:" followed by an indented block otherwise.
inline std::string section(const std::string& key, const std::string& value, size_t spaces)
{
    const auto body = dedent(value);
    const bool isInline = body.find('\n') == body.size() - 1 && !body.starts_with("- ");
    if (isInline)
    {
        return std::string(spaces, ' ') + key + ": " + body.substr(0, body.size() - 1) + "\n";
    }
    return std::string(spaces, ' ') + key + ":\n" + indent(body, spaces + 2);
}

// Sections that are not overridden are empty lists. An empty `extraOutputs` is omitted.
struct ModelSpec
{
    std::string id = "model_id";
    std::string description = "model_description";
    std::string parameters = "[]";
    std::string variables = "[]";
    std::string ports = "[]";
    std::string portFieldDefinitions = "[]";
    std::string constraints = "[]";
    std::string extraOutputs;
    std::string objectives = R"(
        - id: objective
          expression: objective)";
};

// A model as a YAML list item, to be used as (part of) `LibrarySpec::models`.
inline std::string model(const ModelSpec& m)
{
    std::string body = "id: " + m.id + "\n" + "description: " + m.description + "\n"
                       + section("parameters", m.parameters, 0)
                       + section("variables", m.variables, 0) + section("ports", m.ports, 0)
                       + section("port-field-definitions", m.portFieldDefinitions, 0)
                       + section("constraints", m.constraints, 0);
    if (!m.extraOutputs.empty())
    {
        body += section("extra-outputs", m.extraOutputs, 0);
    }
    body += section("objective-contributions", m.objectives, 0);
    return "- " + indent(body, 2).substr(2);
}

struct LibrarySpec
{
    std::string id = "lib_id";
    std::string description = "lib_description";
    std::string portTypes = "[]";
    std::string models = "[]";
};

inline std::string library(const LibrarySpec& l)
{
    std::string body = "id: " + l.id + "\n" + "description: " + l.description + "\n"
                       + section("port-types", l.portTypes, 0) + section("models", l.models, 0);
    return "library:\n" + indent(body, 4);
}

// Library holding a single model.
inline std::string libraryWithModel(const ModelSpec& m)
{
    return library({.models = model(m)});
}

} // namespace YmlTest
