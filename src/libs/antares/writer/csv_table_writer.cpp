// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "private/csv_table_writer.h"

#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <utility>

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

// Reads row `rowIndex` straight from the columns, so the table never has to be transposed
// into a vector of rows just to be written.
void appendRow(fmt::memory_buffer& buf,
               const std::vector<std::shared_ptr<IColumn>>& columns,
               size_t rowIndex)
{
    for (size_t i = 0; i < columns.size(); ++i)
    {
        if (i)
        {
            buf.push_back(',');
        }
        appendEscaped(buf, columns[i]->toString(rowIndex));
    }
    buf.push_back('\n');
}

// Above this size, flush the accumulated buffer to disk instead of growing it further,
// so writing a very large table doesn't require holding the whole CSV in memory at once.
constexpr size_t chunkFlushThreshold = 1 << 20; // 1 MiB

void flushIfNeeded(fmt::memory_buffer& buf, std::ofstream& out, bool force = false)
{
    if (buf.size() == 0 || (!force && buf.size() < chunkFlushThreshold))
    {
        return;
    }
    out.write(buf.data(), static_cast<std::streamsize>(buf.size()));
    buf.clear();
}

CsvTableWriter::CsvTableWriter(const std::filesystem::path& filePath):
    ITableWriter(filePath)
{
}

void CsvTableWriter::writeTable(const SimulationTable& simuTable) const
{
    const auto& columns = simuTable.columns();

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

    if (columns.empty())
    {
        return;
    }

    fmt::memory_buffer buf;

    for (size_t i = 0; i < columns.size(); ++i)
    {
        if (i)
        {
            buf.push_back(',');
        }
        appendEscaped(buf, columns[i]->name());
    }
    buf.push_back('\n');

    const size_t rowCount = simuTable.rowCount();
    for (size_t row = 0; row < rowCount; ++row)
    {
        appendRow(buf, columns, row);
        flushIfNeeded(buf, out);
    }

    flushIfNeeded(buf, out, /*force=*/true);
}

} // namespace Antares::Writer
