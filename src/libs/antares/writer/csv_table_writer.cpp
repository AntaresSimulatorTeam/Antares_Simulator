/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include "csv_table_writer.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace fs = std::filesystem;

namespace Antares::Writer
{

std::string CsvTableWriter::escape(const std::string& cell)
{
    bool needs_quotes = cell.find_first_of(",\n\r\t\"") != std::string::npos;
    if (!needs_quotes)
        return cell;

    std::string out;
    out.reserve(cell.size() + 2);
    out.push_back('"');
    for (char c : cell)
    {
        if (c == '"')
            out.push_back('"');
        out.push_back(c);
    }
    out.push_back('"');
    return out;
}

void CsvTableWriter::writeTable(const fs::path& filePath,
                                const std::vector<std::string>& header,
                                const std::vector<std::vector<std::string>>& rows)
{
    if (filePath.empty())
        throw std::invalid_argument("CsvTableWriter: empty output path");

    if (filePath.has_parent_path())
        fs::create_directories(filePath.parent_path());

    std::ofstream out(filePath, std::ios::binary);
    if (!out)
        throw std::runtime_error("CsvTableWriter: cannot open output file: " + filePath.string());

    auto write_line = [&out](const std::vector<std::string>& cols) {
        for (size_t i = 0; i < cols.size(); ++i)
        {
            if (i)
                out.put(',');
            out << CsvTableWriter::escape(cols[i]);
        }
        out.put('\n');
    };

    if (!header.empty())
        write_line(header);

    for (const auto& r : rows)
        write_line(r);
}

} // namespace Antares::Writer
