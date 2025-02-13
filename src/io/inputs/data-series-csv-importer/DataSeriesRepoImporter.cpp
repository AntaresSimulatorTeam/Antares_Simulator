/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
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

#include <boost/algorithm/string.hpp>

#include <antares/io/inputs/data-series-csv-importer/DataSeriesRepoImporter.h>
#include <antares/optimisation/linear-problem-data-impl/timeSeriesSet.h>
namespace fs = std::filesystem;

namespace Antares::IO::Inputs::DataSeriesCsvImporter
{
using namespace Optimisation::LinearProblemDataImpl;

static std::vector<double> readLine(std::string line,
                                    std::string csvSeparators,
                                    const std::string fileName)
{
    std::vector<double> row;
    std::vector<std::string> splitLine;
    boost::split(splitLine, line, boost::is_any_of(csvSeparators));
    std::transform(splitLine.begin(),
                   splitLine.end(),
                   std::inserter(row, row.end()),
                   [fileName](std::string& s)
                   {
                       boost::trim(s);
                       if (s.empty())
                       {
                           throw std::invalid_argument(
                             fileName + ": columns have inconsistent number of rows");
                       }
                       try
                       {
                           return std::stod(s);
                       }
                       catch (const std::invalid_argument&)
                       {
                           throw std::invalid_argument(fileName + ": \"" + s
                                                       + "\" is not a number");
                       }
                   });
    return row;
}

static std::vector<std::vector<double>> csvToMatrix(const std::filesystem::path& path,
                                                    std::string csvSeparators)
{
    std::vector<std::vector<double>> result;
    std::ifstream infile(path, std::ios_base::binary | std::ios_base::in);
    if (!infile.is_open())
    {
        throw std::invalid_argument("Could not open file " + path.filename().string());
    }
    std::string line;
    bool empty_line_found = false;
    while (std::getline(infile, line))
    {
        boost::trim(line);
        if (line.empty())
        {
            empty_line_found = true;
            continue;
        }
        if (empty_line_found)
        {
            // only accept empty lines in the end of the file
            throw std::invalid_argument(path.filename().string()
                                        + ": empty line in the middle of the file");
        }
        std::vector<double> row = readLine(line, csvSeparators, path.filename().string());
        if (!result.empty() && row.size() != result[0].size())
        {
            throw std::invalid_argument(path.filename().string()
                                        + ": rows have inconsistent number of columns");
        }
        result.push_back(row);
    }
    return result;
}

static TimeSeriesSet matrixToTimeSeriesSet(std::string id, std::vector<std::vector<double>> matrix)
{
    // We have to transpose the matrix
    // TODO: we may want to improve this by reading directly into the TimeSeriesSet object, or
    // by creating a specific IDataSeries implementation
    int nTimesteps = matrix.size();
    TimeSeriesSet timeSeriesSet(id, nTimesteps);
    if (nTimesteps == 0)
    {
        return timeSeriesSet;
    }
    int nSets = matrix[0].size();
    for (int i = 0; i < nSets; ++i)
    {
        std::vector<double> set;
        set.reserve(nTimesteps);
        for (int j = 0; j < nTimesteps; ++j)
        {
            set.push_back(matrix[j][i]);
        }
        timeSeriesSet.add(set);
    }
    return timeSeriesSet;
}

static TimeSeriesSet importFromFile(const std::filesystem::path& path, std::string csvSeparators)
{
    auto csvMatrix = csvToMatrix(path, csvSeparators);
    return matrixToTimeSeriesSet(path.stem().string(), csvMatrix);
}

bool hasRightExtension(const std::filesystem::directory_entry& e)
{
    auto ext = e.path().extension();
    return (ext == ".csv") || (ext == ".tsv");
}

DataSeriesRepository DataSeriesRepoImporter::importFromDirectory(const std::filesystem::path& path,
                                                                 std::string csvSeparators)
{
    if (!is_directory(path))
    {
        throw std::invalid_argument("Not a directory: " + path.string());
    }
    using std::views::filter;
    auto pathFilter = filter(static_cast<bool (*)(const fs::path&)>(&fs::is_regular_file))
                      | filter(&hasRightExtension);

    DataSeriesRepository repo{};
    for (auto paths = std::filesystem::directory_iterator{path};
         const auto& entry: paths | pathFilter)
    {
        auto timeSeriesSet = std::make_unique<TimeSeriesSet>(importFromFile(entry, csvSeparators));
        repo.addDataSeries(std::move(timeSeriesSet));
    }
    return repo;
}

} // namespace Antares::IO::Inputs::DataSeriesCsvImporter
