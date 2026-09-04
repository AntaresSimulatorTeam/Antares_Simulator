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
 * Accepts an unsigned integer ("5"), a quoted integer ("5") or an inclusive range of
 * non-negative integers ("0-9"). A leading sign is not part of the grammar: a '-' is
 * reported as "indices must be >= 0" and a '+' is reported as a format error.
 */
std::set<unsigned> expandEntry(const std::string& entry)
{
    std::set<unsigned> result;

    // Parse a token that must be a plain (unsigned) run of digits.
    const auto parseDigits = [&](const std::string& token) -> unsigned
    {
        if (token.empty()
            || std::any_of(token.begin(),
                           token.end(),
                           [](unsigned char c) { return !std::isdigit(c); }))
        {
            throwInvalidEntry(entry);
        }
        return static_cast<unsigned>(std::stoul(token));
    };

    // A leading sign means the entry is a single signed number, not a range. The
    // documented grammar only allows unsigned non-negative integers, so a '-' yields a
    // dedicated message and a '+' is a plain format error.
    if (!entry.empty() && (entry[0] == '-' || entry[0] == '+'))
    {
        if (entry[0] == '-')
        {
            throw std::invalid_argument(
              fmt::format("Invalid scenario-scope entry '{}': indices must be >= 0", entry));
        }
        throwInvalidEntry(entry);
    }

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
        const auto first = parseDigits(firstPart);
        const auto last = parseDigits(secondPart);
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
        result.insert(parseDigits(entry));
    }

    return result;
}

/** Expand a list of entries into a deduplicated, sorted set of indices. */
std::set<unsigned> expandEntries(const std::vector<std::string>& entries)
{
    std::set<unsigned> indices;
    for (const auto& entry: entries)
    {
        const auto expanded = expandEntry(entry);
        indices.insert(expanded.begin(), expanded.end());
    }
    return indices;
}

std::vector<std::string> entriesFromJson(const std::filesystem::path& playlistFile)
{
    const auto content = IO::readFile(playlistFile);
    nlohmann::json json;
    try
    {
        json = nlohmann::json::parse(content);
    }
    catch (const nlohmann::json::parse_error& e)
    {
        // e.what() already carries the line/column position; add the file path so the
        // broken playlist can be located (the missing-file case is reported by IO::readFile).
        throw std::invalid_argument(
          fmt::format("Invalid playlist file '{}': {}", playlistFile.string(), e.what()));
    }

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
        // std::set::erase returns the number of elements removed (0 or 1).
        if (base.erase(excluded) == 0)
        {
            logs.warning() << fmt::format(
              "scenario-scope: excluded scenario {} is not in the base set and has no effect",
              excluded);
        }
    }

    if (base.empty())
    {
        logs.warning(
          "scenario-scope: no scenarios left after applying exclusions, defaulting to scenario 0");
        return {0};
    }

    return std::vector<unsigned>(base.begin(), base.end());
}

} // namespace Antares::Solver
