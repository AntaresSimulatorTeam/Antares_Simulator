/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include <algorithm>
#include <charconv>
#include <fstream>
#include <ranges>
#include <vector>

#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>

#include <antares/io/inputs/data-series-csv-importer/DataSeriesRepoImporter.h>
#include <antares/optimisation/linear-problem-data-impl/timeSeriesSet.h>

namespace fs = std::filesystem;

namespace Antares::IO::Inputs::DataSeriesCsvImporter
{
using namespace Optimisation::LinearProblemDataImpl;

inline const char* ParseOneDouble(const char* ptr,
                                  const char* end,
                                  double& value,
                                  const std::string& errorMessagePrefix = "")
{
    auto [p, ec] = std::from_chars(ptr, end, value);
    if (ec == std::errc::invalid_argument)
    {
        throw std::invalid_argument(errorMessagePrefix + ": \"" + *p + "\" is not a number");
    }
    return p;
}

inline void SkipWhiteSpaceAndSeparator(const char*& ptr, const char* last, char sep)
{
    // Skip leading whitespace and separators
    while (ptr < last && (*ptr == sep || *ptr == ' ' || *ptr == '\t'))
    {
        ++ptr;
    }
}

static std::vector<double> ParseFirstRow(const char* first,
                                         const char* last,
                                         char sep = ' ',
                                         const std::string& errorMessagePrefix = "")
{
    std::vector<double> row;

    const char* ptr = first;

    while (ptr < last)
    {
        SkipWhiteSpaceAndSeparator(ptr, last, sep);

        // If we've reached the end, break
        if (ptr >= last)
        {
            break;
        }

        double val = 0.;
        const char* next = ParseOneDouble(ptr, last, val, errorMessagePrefix);
        // Check if we parsed anything
        if (next == ptr)
        {
            // Skip invalid characters and try again
            ++ptr;
            continue;
        }

        row.push_back(val);
        ptr = next;
    }

    return row;
}

static void ParseRow(const char* first,
                     const char* last,
                     std::vector<std::vector<double>>& columns,
                     unsigned rowIndex,
                     char sep = ' ',
                     const std::string& errorMessagePrefix = "")
{
    const char* ptr = first;
    unsigned colIndex = 0;
    while (ptr < last)
    {
        SkipWhiteSpaceAndSeparator(ptr, last, sep);

        // If we've reached the end, break
        if (ptr >= last)
        {
            break;
        }

        double val = 0.;
        const char* next = ParseOneDouble(ptr, last, val, errorMessagePrefix);
        // Check if we parsed anything
        if (next == ptr)
        {
            // Skip invalid characters and try again
            ++ptr;
            continue;
        }
        if (colIndex < columns.size())
        {
            columns[colIndex][rowIndex] = val;
            ptr = next;
            ++colIndex;
        }
        else
        {
            throw std::invalid_argument(errorMessagePrefix
                                        + ": rows have inconsistent number of columns");
        }
    }
}

static std::vector<std::vector<double>> readCSV(const std::filesystem::path& filename, char sep)
{
    // MappedFile file(fileName.c_str());
    // Check file size first
    std::error_code ec;
    auto sz = std::filesystem::file_size(filename, ec);
    if (ec || sz == 0)
    {
        return {}; // empty or inaccessible
    }
    const auto& fileName = filename.string();
    boost::iostreams::mapped_file_source file(fileName);

    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + fileName);
    }

    std::vector<std::vector<double>> columns;
    const char* start = file.data();
    const char* end = start + file.size();
    unsigned lineCount = std::count(start, end, '\n') + 1;
    unsigned lineNumber = 0;

    while (start < end)
    {
        const char* endLine = static_cast<const char*>(memchr(start, '\n', end - start));
        if (!endLine)
        {
            endLine = end;
        }
        // Handle Windows line endings
        size_t lineLen = endLine - start;
        if (lineLen > 0 && start[lineLen - 1] == '\r')
        {
            lineLen--;
        }


        // initialize columns on first row
        if (columns.empty())
        {
            auto row = ParseFirstRow(start, start + lineLen, sep, fileName);

            // skip empty line
            if (row.empty())
            {
                continue;
            }
            columns.resize(row.size());

            for (auto colIndex = 0; colIndex < columns.size(); ++colIndex)
            {
                auto& column = columns[colIndex];
                column.resize(lineCount);
                column[0] = row[colIndex];
            }
        }
        else
        {
            ParseRow(start, start + lineLen, columns, lineNumber, sep, fileName);
        }

        start = endLine + 1;
        ++lineNumber;
    }

    return columns;
}

bool hasRightExtension(const std::filesystem::directory_entry& e)
{
    auto ext = e.path().extension();
    return (ext == ".csv") || (ext == ".tsv");
}

DataSeriesRepository DataSeriesRepoImporter::importFromDirectory(const std::filesystem::path& path,
                                                                 char csvSeparator)
{
    if (!is_directory(path))
    {
        throw std::invalid_argument("Not a directory: " + path.string());
    }
    using std::views::filter;
    auto pathFilter = filter(static_cast<bool (*)(const fs::path&)>(&fs::is_regular_file));

    DataSeriesRepository repo{};
    for (auto paths = std::filesystem::directory_iterator{path};
         const auto& entry: paths | pathFilter)
    {
        if (!hasRightExtension(entry))
        {
            continue;
        }
        auto timeSeriesSet = std::make_unique<TimeSeriesSet>(entry.path().stem().string(),
                                                             readCSV(entry, csvSeparator));
        repo.addDataSeries(std::move(timeSeriesSet));
    }
    return repo;
}

} // namespace Antares::IO::Inputs::DataSeriesCsvImporter
