// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/modeler/parameters/scenarioScope.h"

#include <algorithm>
#include <cctype>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <set>
#include <stdexcept>
#include <string>

#include <antares/io/file.h>
#include <antares/logs/logs.h>

namespace Antares::Solver
{

namespace
{

[[noreturn]] void throwInvalidEntry(const std::string& entry)
{
    throw std::invalid_argument(
      fmt::format("Invalid scenario-scope entry '{}': expected an integer, a quoted integer or an "
                  "inclusive 'a-b' range of non-negative integers",
                  entry));
}

/** Expand a single entry into individual indices.
 *
 * Accepts an integer ("5"), a string integer ("5") or an inclusive range ("0-9").
 */
std::set<unsigned> expandEntry(const std::string& entry)
{
    std::set<unsigned> result;

    const auto parseIndex = [&](const std::string& token) -> unsigned
    {
        // Check for a negative sign
        const auto hasSign = token.size() >= 1 && (token[0] == '-' || token[0] == '+');
        const std::string numberPart = hasSign ? token.substr(1) : token;
        if (numberPart.empty()
            || std::any_of(numberPart.begin(),
                           numberPart.end(),
                           [](unsigned char c) { return !std::isdigit(c); }))
        {
            throwInvalidEntry(entry);
        }
        if (hasSign && token[0] == '-')
        {
            throw std::invalid_argument(
              fmt::format("Invalid scenario-scope entry '{}': indices must be >= 0", entry));
        }
        return static_cast<unsigned>(std::stoul(numberPart));
    };

    // An entry is either a range "a-b" or a single index.
    const auto dashPos = entry.find('-');
    if (dashPos != std::string::npos)
    {
        const auto firstPart = entry.substr(0, dashPos);
        const auto secondPart = entry.substr(dashPos + 1);
        if (firstPart.empty() || secondPart.empty())
        {
            throwInvalidEntry(entry);
        }
        const auto first = parseIndex(firstPart);
        const auto last = parseIndex(secondPart);
        if (first > last)
        {
            throw std::invalid_argument(
              fmt::format("Invalid scenario-scope entry '{}': range start must be <= range end",
                          entry));
        }
        for (auto i = first; i <= last; ++i)
        {
            result.insert(i);
        }
    }
    else
    {
        result.insert(parseIndex(entry));
    }

    return result;
}

/** Expand a list of entries into a sorted, deduplicated vector of indices. */
std::vector<unsigned> expandEntries(const std::vector<std::string>& entries)
{
    std::set<unsigned> indices;
    for (const auto& entry: entries)
    {
        const auto expanded = expandEntry(entry);
        indices.insert(expanded.begin(), expanded.end());
    }
    return {indices.begin(), indices.end()};
}

std::vector<std::string> entriesFromJson(const std::filesystem::path& playlistFile)
{
    const auto content = IO::readFile(playlistFile);
    const auto json = nlohmann::json::parse(content);

    if (!json.is_array())
    {
        throw std::invalid_argument(
          fmt::format("Invalid playlist file '{}': expected a JSON array", playlistFile.string()));
    }

    std::vector<std::string> entries;
    entries.reserve(json.size());
    for (const auto& item: json)
    {
        if (item.is_number_integer())
        {
            entries.push_back(std::to_string(item.get<long long>()));
        }
        else if (item.is_string())
        {
            entries.push_back(item.get<std::string>());
        }
        else
        {
            throw std::invalid_argument(
              fmt::format("Invalid playlist file '{}': each element must be an integer or string",
                          playlistFile.string()));
        }
    }
    return entries;
}

} // namespace

std::vector<unsigned> resolveScenarioScopeScenarios(const ScenarioScope& scope,
                                                    const std::filesystem::path& studyPath)
{
    const bool hasInclude = !scope.include.empty();
    const bool hasPlaylist = scope.playlistFile.has_value();
    const bool hasExclude = !scope.exclude.empty();

    if (hasInclude && hasPlaylist)
    {
        throw std::invalid_argument("scenario-scope: 'include' and 'playlist-file' are mutually "
                                    "exclusive");
    }
    if (hasExclude && !hasInclude && !hasPlaylist)
    {
        throw std::invalid_argument("scenario-scope: 'exclude' can only be used with 'include' or "
                                    "'playlist-file'");
    }
    if (!hasInclude && !hasPlaylist)
    {
        // No scenario-scope key at all, or an empty block: run scenario 0 only.
        return {0};
    }

    std::vector<std::string> include = scope.include;
    if (hasPlaylist)
    {
        const auto playlistPath = scope.playlistFile->is_absolute()
                                    ? *scope.playlistFile
                                    : studyPath / *scope.playlistFile;
        include = entriesFromJson(playlistPath);
    }

    auto base = expandEntries(include);
    const auto excludes = expandEntries(scope.exclude);
    for (const auto excluded: excludes)
    {
        const auto it = std::find(base.begin(), base.end(), excluded);
        if (it == base.end())
        {
            logs.warning() << fmt::format(
              "scenario-scope: excluded scenario {} is not in the base set and has no effect",
              excluded);
        }
        else
        {
            base.erase(it);
        }
    }

    if (base.empty())
    {
        logs.warning(
          "scenario-scope: no scenarios left after applying exclusions, defaulting to scenario 0");
        return {0};
    }

    return base;
}

} // namespace Antares::Solver
