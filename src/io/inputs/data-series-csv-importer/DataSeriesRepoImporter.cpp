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
#include <fstream>
#include <ranges>

#include <boost/regex.hpp>

#include <antares/io/inputs/data-series-csv-importer/DataSeriesRepoImporter.h>

namespace Antares::IO::Inputs::DataSeriesCsvImporter
{
using namespace Optimisation::LinearProblemDataImpl;

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
    auto pathFilter = filter(static_cast<bool (*)(const std::filesystem::path&)>(
                        &std::filesystem::is_regular_file))
                      | filter(&hasRightExtension);

    DataSeriesRepository repo{};
    for (auto paths = std::filesystem::directory_iterator{path};
         const auto& entry: paths | pathFilter)
    {
        std::unique_ptr<IDataSeries> timeSeriesSet = std::make_unique<TimeSeriesSet>(
          TimeSeriesSetImporter::importFromFile(entry, csvSeparator));
        repo.addDataSeries(std::move(timeSeriesSet));
    }
    return repo;
}

std::vector<std::vector<double>> TimeSeriesSetImporter::csvToMatrix(
  const std::filesystem::path& path,
  char csvSeparator)
{
    std::vector<std::vector<double>> result;
    std::ifstream infile(path, std::ios_base::binary | std::ios_base::in);
    if (!infile.is_open())
    {
        throw std::invalid_argument("Could not open file " + path.filename().string());
    }
    std::string line;
    std::string element;
    bool empty_line_found = false;
    while (!infile.eof())
    {
        std::vector<double> row;
        getline(infile, line);
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
        std::stringstream ss(line);
        while (getline(ss, element, csvSeparator))
        {
            if (element.empty())
            {
                throw std::invalid_argument(path.filename().string()
                                            + ": columns have inconsistent number of rows");
            }
            try
            {
                row.push_back(std::stod(element));
            }
            catch (const std::invalid_argument& e)
            {
                throw std::invalid_argument(path.filename().string() + ": \"" + element
                                            + "\" is not a number");
            }
        }
        if (!result.empty() && row.size() != result[0].size())
        {
            throw std::invalid_argument(path.filename().string()
                                        + ": rows have inconsistent number of columns");
        }
        result.push_back(row);
    }
    return result;
}

TimeSeriesSet TimeSeriesSetImporter::importFromFile(const std::filesystem::path& path,
                                                    char csvSeparator)
{
    auto csvMatrix = csvToMatrix(path, csvSeparator);
    // We have to transpose the matrix
    // TODO: we may want to improve this by reading directly into the TimeSeriesSet object, or
    // by creating a specific IDataSeries implementation
    int nTimesteps = csvMatrix.size();
    TimeSeriesSet timeSeriesSet(path.stem().string(), nTimesteps);
    if (nTimesteps == 0)
    {
        return timeSeriesSet;
    }
    int nSets = csvMatrix[0].size();
    for (int i = 0; i < nSets; ++i)
    {
        std::vector<double> set;
        set.reserve(nTimesteps);
        for (int j = 0; j < nTimesteps; ++j)
        {
            set.push_back(csvMatrix[j][i]);
        }
        timeSeriesSet.add(set);
    }
    return timeSeriesSet;
}

} // namespace Antares::IO::Inputs::DataSeriesCsvImporter
