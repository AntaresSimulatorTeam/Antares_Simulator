/*
** Copyright 2007-2024 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#pragma once

#include <utility>

#include "vector"

using namespace operations_research;
using namespace std;

namespace Antares::optim::api
{
    class LinearProblemData
    {
    private:
        vector<int> timeStamps_;
        int timeResolutionInMinutes_;
        map<string, double> scalarData_;
        map<string, vector<double>> timedData_;
    public:
        explicit LinearProblemData(vector<int> timeStamps, int timeResolutionInMinutes, map<string, double> scalarData,
                                   map <string, vector<double>> timedData) :
                timeStamps_(std::move(timeStamps)), timeResolutionInMinutes_(timeResolutionInMinutes),
                scalarData_(std::move(scalarData)), timedData_(std::move(timedData))
        {};
        vector<int> getTimeStamps() const { return timeStamps_; }
        int getTimeResolutionInMinutes() const { return timeResolutionInMinutes_; }
        bool hasScalarData(const string& key) const { return scalarData_.contains(key); }
        double getScalarData(const string& key) const { return scalarData_.at(key); }
        bool hasTimedData(const string& key) const { return timedData_.contains(key); }
        vector<double> getTimedData(const string& key) const { return timedData_.at(key); }
    };
}
