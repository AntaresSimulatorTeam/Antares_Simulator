// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "csv_table_writer.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace fs = std::filesystem;

namespace Antares::Writer
{

std::string escape(const std::string& cell)
{
    bool needs_quotes = cell.find_first_of(",\n\r\t\"") != std::string::npos;
    if (!needs_quotes)
    {
        return cell;
    }

    std::string out;
    out.reserve(cell.size() + 2);
    out.push_back('"');
    for (char c: cell)
    {
        if (c == '"')
        {
            out.push_back('"');
        }
        out.push_back(c);
    }
    out.push_back('"');
    return out;
}

std::string make_line(const std::vector<std::string>& cols)
{
    std::string out;
    for (size_t i = 0; i < cols.size(); ++i)
    {
        if (i)
        {
            out += ',';
        }
        out += escape(cols[i]);
    }
    out += '\n';
    return out;
}

void CsvTableWriter::writeTable(const fs::path& filePath,
                                const std::vector<std::string>& header,
                                const std::vector<std::vector<std::string>>& rows)
{
    if (filePath.empty())
    {
        throw std::invalid_argument("CsvTableWriter: empty output path");
    }

    if (filePath.has_parent_path())
    {
        fs::create_directories(filePath.parent_path());
    }

    std::ofstream out(filePath, std::ios::binary);
    if (!out)
    {
        throw std::runtime_error("CsvTableWriter: cannot open output file: " + filePath.string());
    }

    if (!header.empty())
    {
        out << make_line(header);
    }

    for (const auto& r: rows)
    {
        out << make_line(r);
    }
}

} // namespace Antares::Writer
