/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include <algorithm>
#include <fstream>
#include <ranges>
#include <vector>

#include <antares/io/inputs/data-series-csv-importer/DataSeriesRepoImporter.h>
#include <antares/optimisation/linear-problem-data-impl/timeSeriesSet.h>

namespace fs = std::filesystem;

namespace Antares::IO::Inputs::DataSeriesCsvImporter
{
using namespace Optimisation::LinearProblemDataImpl;
std::vector<std::vector<double>> readCSV(const std::filesystem::path& filename, char sep)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open file " + filename.string());
    }

    std::vector<std::vector<double>> columns;
    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        std::vector<double> row;
        const char* ptr = line.c_str();
        char* end;

        while (*ptr)
        {
            double val = std::strtod(ptr, &end);

            row.push_back(val);
            ptr = end;

            // skip separator and whitespace
            while (*ptr == sep || *ptr == ' ' || *ptr == '\t')
            {
                ++ptr;
            }
            if (*(ptr + 1) == '\0')
            {
                break;
            }
        }

        // initialize columns on first row
        if (columns.empty())
        {
            columns.resize(row.size());
        }

        if (row.size() != columns.size())
        {
            throw std::runtime_error("Inconsistent number of columns in CSV");
        }

        for (size_t i = 0; i < row.size(); ++i)
        {
            columns[i].push_back(row[i]);
        }
    }
    return columns;
}

static TimeSeriesSet importFromFile(const std::filesystem::path& path, char csvSeparator)
{
    auto cols = readCSV(path, csvSeparator);
    int nTimesteps = cols.empty() ? 0 : cols[0].size();
    TimeSeriesSet timeSeriesSet(path.stem().string(), nTimesteps);
    for (auto&& col: cols)
    {
        timeSeriesSet.add(col);
    }
    return timeSeriesSet;
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
        throw std::invalid_argument("Not a directory: " + path.string());
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
        auto timeSeriesSet = std::make_unique<TimeSeriesSet>(importFromFile(entry, csvSeparator));
        repo.addDataSeries(std::move(timeSeriesSet));
    }
    return repo;
}

} // namespace Antares::IO::Inputs::DataSeriesCsvImporter
