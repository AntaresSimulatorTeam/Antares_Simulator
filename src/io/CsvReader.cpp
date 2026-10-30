// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/CsvReader.h"

#include <stdexcept>
#include <string>
#include <vector>

#include <boost/iostreams/device/mapped_file.hpp>

namespace Antares::IO
{
namespace
{
bool readCSVBuffer(std::string_view data,
                   std::string_view separators,
                   const CSVRowCallback& callback)
{
    std::string delimiters(separators);
    delimiters += "\r\n";

    std::vector<CSVCell> row;
    row.reserve(16);

    std::size_t rowIndex = 0;
    std::size_t cellStart = 0;
    bool hasCell = false;

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
            if (atEnd && !hasCell)
            {
                break;
            }

            if (hasCell)
            {
                row.push_back({data.substr(cellStart, index - cellStart)});
            }

            if (!callback(rowIndex, row))
            {
                return false;
            }

            row.clear();
            ++rowIndex;
            hasCell = false;

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

        row.push_back({data.substr(cellStart, index - cellStart)});
        hasCell = true;
        ++index;
        cellStart = index;
    }

    return true;
}
} // namespace

bool readCSV(std::string_view data, std::string_view separators, const CSVRowCallback& callback)
{
    return readCSVBuffer(data, separators, callback);
}

bool readCSVFile(const std::filesystem::path& filename,
                 std::string_view separators,
                 const CSVRowCallback& callback)
{
    std::error_code error;
    const auto size = std::filesystem::file_size(filename, error);
    if (error)
    {
        throw std::runtime_error("Error reading CSV file( " + filename.string()
                                 + "):" + error.message());
    }

    if (size == 0)
    {
        return true;
    }

    boost::iostreams::mapped_file_source file(filename.string());
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + filename.string());
    }

    return readCSVBuffer(std::string_view(file.data(), file.size()), separators, callback);
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
