// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "private/csv_table_writer.h"

#include <filesystem>
#include <fstream>
#include <utility>

#include <antares/exception/InvalidArgumentError.hpp>
#include <antares/exception/RuntimeError.hpp>

using namespace Antares::IO::Outputs;
using namespace Antares::Error;
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

    std::vector<std::string> names;
    std::ranges::transform(columns,
                           std::back_inserter(names),
                           [](const auto& col) { return col->name(); });
    if (!names.empty())
    {
        out << make_line(names);
    }

    for (const auto& r: rows)
    {
        out << make_line(r);
    }
}

} // namespace Antares::Writer
