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

#include <stdexcept>
#include <string>
#include <vector>

#include "dataSeries.h"

namespace Antares::Optimisation::LinearProblemDataImpl
{
class TimeSeriesSet: public IDataSeries
{
public:
    explicit TimeSeriesSet(std::string name, unsigned height);
    void add(const std::vector<double>& ts);
    double getData(unsigned tsNumber, unsigned hour) const override;

private:
    unsigned height_ = 0;
    std::vector<std::vector<double>> tsSet_;

public:
    class AddTSofWrongSize: public std::invalid_argument
    {
    public:
        explicit AddTSofWrongSize(const std::string& name,
                                  const size_t& tsSize,
                                  const unsigned& height);
    };

    class Empty: public std::invalid_argument
    {
    public:
        explicit Empty(const std::string& name);
    };

    class RankTooBig: public std::invalid_argument
    {
    public:
        explicit RankTooBig(const std::string& name, unsigned rank, unsigned tsSetSize);
    };

    class HourTooBig: public std::invalid_argument
    {
    public:
        explicit HourTooBig(const std::string& name, unsigned hour);
    };
};

} // namespace Antares::Optimisation::LinearProblemDataImpl
