
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
