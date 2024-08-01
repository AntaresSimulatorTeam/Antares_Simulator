/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once

#include <string>
#include <vector>
#include <map>

namespace Antares::Solver::Optim::Api
{

class LinearProblemData
{
public:
    virtual unsigned getTimeResolutionInMinutes() = 0;
    virtual bool hasScalarData(const std::string& key) = 0;
    virtual double getScalarData(const std::string& key, unsigned scenario) = 0;
    virtual bool hasTimedData(const std::string& key) = 0;
    virtual std::vector<double>& getTimedData(const std::string& key, unsigned scenario) = 0;

protected:
    std::vector<int> timeStamps_;
    unsigned timeResolutionInMinutes_;
    std::map<std::string, double> scalarData_;
    std::map<std::string, std::vector<double>> timedData_;
};

} // namespace Antares::Solver::Optim::Api
