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
bool isSeparator(char value, std::string_view separators)
{
    return separators.find(value) != std::string_view::npos;
}

bool readCSVBuffer(std::string_view data,
                   std::string_view separators,
                   const CSVRowCallback& callback)
{
    std::vector<CSVCell> row;
    row.reserve(16);

    std::size_t rowIndex = 0;
    std::size_t rowStart = 0;
    std::size_t cellStart = 0;
    char lastSeparator = '\0';
    bool hasCell = false;

    for (std::size_t index = 0; index <= data.size();)
    {
        const bool atEnd = index == data.size();
        const char value = atEnd ? '\0' : data[index];
        const bool atNewline = !atEnd && (value == '\n' || value == '\r');

        if (atEnd || atNewline)
        {
            // Do not manufacture a row after a final newline.
            if (atEnd && index == rowStart && !hasCell)
            {
                break;
            }

            if (hasCell)
            {
                row.push_back({data.substr(cellStart, index - cellStart),
                               cellStart,
                               cellStart == index ? lastSeparator : value});
            }

            if (!callback(rowIndex, row))
            {
                return false;
            }

            row.clear();
            ++rowIndex;
            hasCell = false;
            lastSeparator = '\0';

            if (atEnd)
            {
                break;
            }

            ++index;
            if (value == '\r' && index < data.size() && data[index] == '\n')
            {
                ++index;
            }
            rowStart = index;
            cellStart = index;
            continue;
        }

        if (isSeparator(value, separators))
        {
            row.push_back({data.substr(cellStart, index - cellStart), cellStart, value});
            hasCell = true;
            lastSeparator = value;
            ++index;
            cellStart = index;
            continue;
        }

        hasCell = true;
        ++index;
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
} // namespace Antares::IO
