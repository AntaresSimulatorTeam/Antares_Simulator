// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/CsvReader.h"

#include <charconv>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/iostreams/device/mapped_file.hpp>

namespace Antares::IO
{
bool parseCSVDouble(std::string_view text, double& value, CSVDoublePolicy policy)
{
    constexpr std::string_view whitespace = " \t\r\n\f\v";
    const auto first = text.find_first_not_of(whitespace);
    if (first == std::string_view::npos)
    {
        return false;
    }
    text = text.substr(first, text.find_last_not_of(whitespace) - first + 1);
    if (text.front() == '+')
    {
        text.remove_prefix(1);
        if (text.empty() || text.front() == '+' || text.front() == '-')
        {
            return false;
        }
    }
    double parsed = 0.;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), parsed);
    if (error != std::errc{} || end != text.data() + text.size() || std::isnan(parsed)
        || (policy == CSVDoublePolicy::FiniteOnly && std::isinf(parsed)))
    {
        return false;
    }
    value = parsed;
    return true;
}

namespace
{
bool readCSVBuffer(std::string_view data,
                   std::string_view separators,
                   const CSVRowCallback& callback,
                   std::size_t maxCellsPerRow)
{
    // UTF-8 BOM support
    if (data.starts_with("\xEF\xBB\xBF"))
    {
        data.remove_prefix(3);
    }

    std::string delimiters(separators);
    delimiters += "\r\n";

    std::vector<CSVCell> row;
    row.reserve(16);

    std::size_t rowIndex = 0;
    std::size_t cellStart = 0;
    bool hasCell = false;
    bool afterSeparator = false;

    for (std::size_t index = 0; index <= data.size();)
    {
        const auto delimiterIndex = data.find_first_of(delimiters, index);
        const bool atEnd = delimiterIndex == std::string_view::npos;
        index = atEnd ? data.size() : delimiterIndex;
        const char value = atEnd ? '\0' : data[index];
        const bool atNewline = value == '\n' || value == '\r';

        if (index > cellStart)
        {
            hasCell = true;
        }

        if (atEnd || atNewline)
        {
            // Do not manufacture a row after a final newline.
            if (atEnd && !hasCell && !afterSeparator)
            {
                return true;
            }

            if (maxCellsPerRow && row.size() >= maxCellsPerRow)
            {
                throw std::length_error("CSV row exceeds the maximum number of cells");
            }
            row.push_back({data.substr(cellStart, index - cellStart)});

            if (!callback(rowIndex, row))
            {
                return false;
            }

            row.clear();
            ++rowIndex;
            hasCell = false;
            afterSeparator = false;

            if (atEnd)
            {
                break;
            }

            ++index;
            if (value == '\r' && index < data.size() && data[index] == '\n')
            {
                ++index;
            }
            cellStart = index;
            continue;
        }

        if (maxCellsPerRow && row.size() >= maxCellsPerRow)
        {
            throw std::length_error("CSV row exceeds the maximum number of cells");
        }
        row.push_back({data.substr(cellStart, index - cellStart), value});
        hasCell = true;
        afterSeparator = true;
        ++index;
        cellStart = index;
    }

    return true;
}
} // namespace

bool readCSV(std::string_view data, std::string_view separators, const CSVRowCallback& callback)
{
    return readCSVBuffer(data, separators, callback, 0);
}

bool readCSVFile(const std::filesystem::path& filename,
                 std::string_view separators,
                 const CSVRowCallback& callback,
                 std::size_t maxCellsPerRow)
{
    std::error_code error;
    const auto size = std::filesystem::file_size(filename, error);
    if (error)
    {
        throw CSVReaderError("Error reading CSV file( " + filename.string()
                             + "):" + error.message());
    }

    if (size == 0)
    {
        return true;
    }

    boost::iostreams::mapped_file_source file(filename.string());
    if (!file.is_open())
    {
        throw CSVReaderError("Failed to open file: " + filename.string());
    }

    return readCSVBuffer(std::string_view(file.data(), file.size()),
                         separators,
                         callback,
                         maxCellsPerRow);
}

CSVTable readCSVFile(const std::filesystem::path& filename, std::string_view separators)
{
    CSVTable table;
    readCSVFile(filename,
                separators,
                [&table](std::size_t, CSVRow row)
                {
                    auto& values = table.emplace_back();
                    values.reserve(row.size());
                    for (const auto& cell: row)
                    {
                        values.emplace_back(cell.value);
                    }
                    return true;
                });
    return table;
}
} // namespace Antares::IO
