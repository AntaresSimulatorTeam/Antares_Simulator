// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <charconv>
#include <ranges>
#include <sstream>
#include <vector>

#include <antares/io/CsvReader.h>
#include <antares/io/inputs/data-series-csv-importer/DataSeriesRepoImporter.h>
#include <antares/optimisation/linear-problem-data-impl/timeSeriesSet.h>
#include "antares/io/inputs/InputError.h"

namespace fs = std::filesystem;

namespace Antares::IO::Inputs::DataSeriesCsvImporter
{
using namespace LinearProblem::DataImpl;

static std::vector<std::vector<double>> readCSV(const std::filesystem::path& filename, char sep)
{
    std::vector<std::vector<double>> columns;
    std::string separators = " \t";
    separators += sep;
    std::size_t dataRowIndex = 0;

    try
    {
        IO::readCSVFile(filename,
                        separators,
                        [&](std::size_t, IO::CSVRow row)
                        {
                            std::vector<double> values;
                            values.reserve(row.size());
                            for (const auto& cell: row)
                            {
                                if (cell.value.empty())
                                {
                                    continue;
                                }

                                double value = 0.;
                                const auto* first = cell.value.data();
                                const auto* last = first + cell.value.size();
                                const auto [end, error] = std::from_chars(first, last, value);
                                if (error != std::errc{} || end != last)
                                {
                                    const char invalid = end < last ? *end : *first;
                                    throw InputError(filename.string() + ": \"" + invalid
                                                     + "\" is not a number");
                                }
                                values.push_back(value);
                            }

                            if (values.empty())
                            {
                                return true;
                            }

                            const auto rowIndex = dataRowIndex++;

                            if (columns.empty())
                            {
                                columns.resize(values.size());
                            }
                            else if (values.size() != columns.size())
                            {
                                std::ostringstream message;
                                if (values.size() < columns.size())
                                {
                                    message << filename.string() << ": row (" << rowIndex
                                            << ") has less columns (" << values.size()
                                            << ") than the expected (" << columns.size() << ").";
                                }
                                else
                                {
                                    message << filename.string() << ": row (" << rowIndex
                                            << ") has more columns (" << values.size()
                                            << ") than the expected (" << columns.size() << ").";
                                }
                                throw InputError(message.str());
                            }

                            for (std::size_t column = 0; column < values.size(); ++column)
                            {
                                columns[column].push_back(values[column]);
                            }
                            return true;
                        });
    }
    catch (const InputError&)
    {
        throw;
    }
    catch (const std::exception& error)
    {
        throw InputError(error.what());
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
