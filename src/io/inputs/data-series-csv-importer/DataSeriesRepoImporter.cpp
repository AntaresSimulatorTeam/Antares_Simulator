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

#include <boost/regex.hpp>

#include <antares/io/inputs/data-series-csv-importer/DataSeriesRepoImporter.h>

namespace Antares::IO::Inputs::DataSeriesCsvImporter
{
using namespace std;
using namespace boost;
using namespace Optimisation::LinearProblemDataImpl;

DataSeriesRepository DataSeriesRepoImporter::importFromDirectory(const std::filesystem::path& path,
                                                                 char csvSeparator)
{
    if (!is_directory(path))
    {
        throw invalid_argument("Not a directory: " + path.string());
    }
    DataSeriesRepository repo{};
    for (const auto& entry: std::filesystem::directory_iterator(path))
    {
        if (!is_regular_file(entry))
        {
            continue;
        }
        if (entry.path().extension() == ".csv")
        {
            std::unique_ptr<IDataSeries> timeSeriesSet = make_unique<TimeSeriesSet>(
              TimeSeriesSetImporter::importFromFile(entry, csvSeparator));
            repo.addDataSeries(std::move(timeSeriesSet));
        }
    }
    return repo;
}

vector<vector<double>> TimeSeriesSetImporter::csvToMatrix(const std::filesystem::path& path,
                                                          char csvSeparator)
{
    // TODO: add logs?
    // logs.debug() << "Loading data-series from " << path;
    vector<vector<double>> result;
    ifstream infile(path, std::ios_base::binary | std::ios_base::in);
    if (!infile.is_open())
    {
        throw invalid_argument("Could not open file " + path.filename().string());
    }
    string line;
    string element;
    while (!infile.eof())
    {
        vector<double> row;
        getline(infile, line);
        std::stringstream ss(line);
        while (getline(ss, element, csvSeparator))
        {
            row.push_back(stod(element));
        }
        result.push_back(row);
    }
    return move(result);
}

TimeSeriesSet TimeSeriesSetImporter::importFromFile(const std::filesystem::path& path,
                                                    char csvSeparator)
{
    auto csvMatrix = csvToMatrix(path, csvSeparator);
    // We have to transpose the matrix
    // TODO: we may want to improve this by reading directly into the TimeSeriesSet object, or
    // by creating a specific IDataSeries implementation
    int nTimestamps = csvMatrix.size();
    TimeSeriesSet timeSeriesSet(path.stem(), nTimestamps);
    if (nTimestamps == 0)
    {
        return timeSeriesSet;
    }
    int nSets = csvMatrix[0].size();
    for (int i = 0; i < nSets; ++i)
    {
        vector<double> set;
        set.reserve(nTimestamps);
        for (int j = 0; j < nTimestamps; ++j)
        {
            set.push_back(csvMatrix[j][i]);
        }
        timeSeriesSet.add(set);
    }
    return timeSeriesSet;
}

} // namespace Antares::IO::Inputs::DataSeriesCsvImporter
