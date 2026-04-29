// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <algorithm>
#include <charconv>
#include <ranges>
#include <vector>

#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>

#include <antares/io/inputs/data-series-csv-importer/DataSeriesRepoImporter.h>
#include <antares/optimisation/linear-problem-data-impl/timeSeriesSet.h>
#include "antares/io/inputs/InputError.h"

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
        throw InputError(errorMessagePrefix + ": \"" + *p + "\" is not a number");
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

static bool ParseRow(const char* first,
                     const char* last,
                     std::vector<std::vector<double>>& columns,
                     unsigned rowIndex,
                     unsigned rowCount,
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
        // initialize columns on first row
        if (rowIndex == 0)
        {
            std::vector<double> col(rowCount);
            col[rowIndex] = val;
            columns.emplace_back(std::move(col));
            ptr = next;
            ++colIndex;
        }
        else
        {
            if (colIndex < columns.size())
            {
                columns[colIndex][rowIndex] = val;
                ptr = next;
                ++colIndex;
            }
            else
            {
                std::ostringstream oss;
                oss << errorMessagePrefix << ": row (" << rowIndex << ") has more columns ("
                    << colIndex + 1 << ") than the expected (" << columns.front().size() << ").";
                throw InputError(oss.str());
            }
        }
    }
    if (rowIndex != 0 && colIndex != columns.size())
    {
        std::ostringstream oss;
        oss << errorMessagePrefix << ": row (" << rowIndex << ") has less columns (" << colIndex
            << ") than the expected (" << columns.front().size() << ").";
        throw InputError(oss.str());
    }
    return colIndex != 0;
}

static std::vector<std::vector<double>> readCSV(const std::filesystem::path& filename, char sep)
{
    // Check file size first
    std::error_code ec;
    auto sz = std::filesystem::file_size(filename, ec);
    if (ec)
    {
        throw InputError("Error reading CSV file( " + filename.string() + "):" + ec.message());
    }
    if (sz == 0)
    {
        return {}; // empty or inaccessible
    }
    const auto& fileName = filename.string();
    boost::iostreams::mapped_file_source file(fileName);

    if (!file.is_open())
    {
        throw InputError("Failed to open file: " + fileName);
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

        if (ParseRow(start, start + lineLen, columns, lineNumber, lineCount, sep, fileName))
        {
            ++lineNumber;
        }
        start = endLine + 1;
    }
    if (lineCount != lineNumber)
    {
        for (auto& col: columns)
        {
            col.resize(lineNumber);
        }
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
        throw InputError("Not a directory: " + path.string());
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
