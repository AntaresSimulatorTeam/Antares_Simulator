
/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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

namespace Antares::Optimisation::LinearProblemApi
{
/** \brief Context for filling linear problem data.
 * Contains temporal information
 */
class FillContext
{
public:
    FillContext(unsigned localFirstTimeStep,
                unsigned localLastTimeStep,
                unsigned globalFirstTimeStep,
                unsigned globalLastTimeStep,
                unsigned year):
        local{localFirstTimeStep, localLastTimeStep},
        global{globalFirstTimeStep, globalLastTimeStep},
        year_(year)
    {
    }

    [[nodiscard]] unsigned getLocalFirstTimeStep() const
    {
        return local.first;
    }

    [[nodiscard]] unsigned getLocalLastTimeStep() const
    {
        return local.last;
    }

    [[nodiscard]] unsigned getGlobalFirstTimeStep() const
    {
        return global.first;
    }

    [[nodiscard]] unsigned getGlobalLastTimeStep() const
    {
        return global.last;
    }

    [[nodiscard]] unsigned int getLocalNumberOfTimeSteps() const
    {
        return local.last - local.first + 1;
    }

    [[nodiscard]] std::vector<unsigned> getSelectedScenarios() const
    {
        return selectedScenario;
    }

    void addSelectedScenarios(unsigned scenario)
    {
        selectedScenario.push_back(scenario);
    }

    [[nodiscard]] unsigned getYear() const
    {
        return year_;
    }

private:
    std::vector<unsigned> selectedScenario;

    struct LocalTimeInterval
    {
        unsigned first = 0; // included
        unsigned last = 0;  // included
    };

    struct GlobalTimeInterval
    {
        unsigned first = 0; // included
        unsigned last = 0;  // included
    };

    LocalTimeInterval local;
    GlobalTimeInterval global;

    unsigned year_ = 0;
};

/**
 * \brief Interface for linear problem data.
 * Provides a method to retrieve data for a specific dataset, time series number, and hour.
 */
class ILinearProblemData
{
public:
    virtual ~ILinearProblemData() = default;

    [[nodiscard]] virtual double getData(const std::string& dataSetId,
                                         unsigned timeSeriesNumber,
                                         unsigned hour) const
      = 0;
};

} // namespace Antares::Optimisation::LinearProblemApi
