
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

#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#include "dataSeries.h"

namespace Antares::Optimisation::LinearProblemDataImpl
{

class DataSeriesRepository
{
public:
    void addDataSeries(std::unique_ptr<IDataSeries> dataSeries);
    [[nodiscard]] IDataSeries& getDataSeries(const std::string& setId) const;
    // Constructeurs et opérateurs
    DataSeriesRepository() = default;
    DataSeriesRepository(const DataSeriesRepository&) = delete;
    DataSeriesRepository& operator=(const DataSeriesRepository&) = delete;

    DataSeriesRepository(DataSeriesRepository&&) noexcept = default;
    DataSeriesRepository& operator=(DataSeriesRepository&&) noexcept = default;

private:
    std::map<std::string, std::unique_ptr<IDataSeries>> dataSeries_;

public:
    // Exception classes
    class Empty: public std::invalid_argument
    {
    public:
        Empty();
    };

    class DataSeriesNotExist: public std::invalid_argument
    {
    public:
        explicit DataSeriesNotExist(const std::string&);
    };

    class DataSeriesAlreadyExists: public std::invalid_argument
    {
    public:
        explicit DataSeriesAlreadyExists(const std::string&);
    };
};

} // namespace Antares::Optimisation::LinearProblemDataImpl
