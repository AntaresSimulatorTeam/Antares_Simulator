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
#include <antares/optimisation/linear-problem-data-impl/dataSeriesRepo.h>
#include <antares/optimisation/linear-problem-data-impl/timeSeriesSet.h>

namespace Antares::IO::Inputs::DataSeriesCsvImporter
{
using namespace std;
using namespace boost;
using namespace Optimisation::LinearProblemDataImpl;

DataSeriesRepository DataSeriesRepoImporter::importFromDirectory(const string& path,
                                                                 char csvSeparator)
{
    return DataSeriesRepository();
}

TimeSeriesSet TimeSeriesSetImporter::importFromFile(const string& path, char csvSeparator)
{
    // used to split the file in lines
    const regex linesregx("\\r\\n|\\n\\r|\\n|\\r");
    // used to split each line to tokens, assuming ',' as column separator
    const regex fieldsregx(csvSeparator + "(?=(?:[^\"]*\"[^\"]*\")*(?![^\"]*\"))");
    vector<vector<double>> result;
    ifstream infile;
    infile.open(path);
    char data[1024];
    infile.read(data, sizeof(data));
    data[infile.tellg()] = '\0';
    unsigned int length = strlen(data);

    // iterator splits data to lines
    cregex_token_iterator li(data, data + length, linesregx, -1);
    cregex_token_iterator end;

    while (li != end)
    {
        string line = li->str();
        ++li;

        // Split line to tokens
        sregex_token_iterator ti(line.begin(), line.end(), fieldsregx, -1);
        sregex_token_iterator end2;

        vector<double> row;
        while (ti != end2)
        {
            double token = stod(ti->str());
            ++ti;
            row.push_back(token);
        }
        result.push_back(row);
    }
    // We have to transpose the matrix
    string id = path; // TODO
    if (result.empty())
    {
        return TimeSeriesSet(id, 0);
    }
    int nTimestamps = result.size();
    TimeSeriesSet timeSeriesSet(id, nTimestamps);
    int nSets = result[0].size();
    for (int i = 0; i < nSets; ++i)
    {
        vector<double> set;
        set.reserve(nTimestamps);
        for (int j = 0; j < nTimestamps; ++j)
        {
            set.push_back(result[j][i]);
        }
        timeSeriesSet.add(set);
    }
    return timeSeriesSet;
}

} // namespace Antares::IO::Inputs::DataSeriesCsvImporter
