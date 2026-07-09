// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "private/csv_table_writer.h"

#include <filesystem>
#include <fstream>
#include <utility>

#include <fmt/format.h>

#include <antares/exception/InvalidArgumentError.hpp>
#include <antares/exception/RuntimeError.hpp>

using namespace Antares::IO::Outputs;
using namespace Antares::Error;
namespace fs = std::filesystem;

namespace Antares::Writer
{

// Appends `cell` to `buf`, quoting it only when it contains a character that requires it.
// Appending straight into the shared buffer (rather than returning a std::string per cell)
// avoids one allocation/copy per cell in the common, non-quoted case.
void appendEscaped(fmt::memory_buffer& buf, const std::string& cell)
{
    bool needs_quotes = cell.find_first_of(",\n\r\t\"") != std::string::npos;
    if (!needs_quotes)
    {
        buf.append(cell);
        return;
    }

    buf.push_back('"');
    for (char c: cell)
    {
        if (c == '"')
        {
            buf.push_back('"');
        }
        buf.push_back(c);
    }
    buf.push_back('"');
}

void appendLine(fmt::memory_buffer& buf, const std::vector<std::string>& cols)
{
    for (size_t i = 0; i < cols.size(); ++i)
    {
        if (i)
        {
            buf.push_back(',');
        }
        appendEscaped(buf, cols[i]);
    }
    buf.push_back('\n');
}

CsvTableWriter::CsvTableWriter(const std::filesystem::path& filePath):
    ITableWriter(filePath)
{
}

void CsvTableWriter::writeTable(const SimulationTable& simuTable) const
{
    const auto& columns = simuTable.columns();
    std::vector<std::vector<std::string>> rows = simuTable.storageIntoRows();

    if (output_file_.empty())
    {
        throw InvalidArgumentError("CsvTableWriter: empty output path");
    }

    if (output_file_.has_parent_path())
    {
        fs::create_directories(output_file_.parent_path());
    }

    std::ofstream out(output_file_, std::ios::binary);
    if (!out)
    {
        throw RuntimeError("CsvTableWriter: cannot open output file: " + output_file_.string());
    }

    fmt::memory_buffer buf;

    if (!columns.empty())
    {
        for (size_t i = 0; i < columns.size(); ++i)
        {
            if (i)
            {
                buf.push_back(',');
            }
            appendEscaped(buf, columns[i]->name());
        }
        buf.push_back('\n');
    }

    for (const auto& r: rows)
    {
        appendLine(buf, r);
    }

    out.write(buf.data(), static_cast<std::streamsize>(buf.size()));
}

} // namespace Antares::Writer
