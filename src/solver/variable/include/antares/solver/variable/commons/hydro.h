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

#include <vector>

#include "timeseries_base.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Economy
{

// Hydro-specific traits
struct HydroTraits
{
    inline static constexpr std::string_view kCaption = "H. ROR";
    inline static constexpr std::string_view kDescription = "Hydro generation, thoughout all MC "
                                                            "years";
};

// VCard for hydro
using VCardTimeSeriesValuesHydro = VCardTimeSeriesBase<HydroTraits>;

// Hydro implementation
template<class NextT = Container::EndOfList>
class TimeSeriesValuesHydro
    : public TimeSeriesValuesBase<TimeSeriesValuesHydro<NextT>, NextT, VCardTimeSeriesValuesHydro>
{
public:
    using BaseType = TimeSeriesValuesBase<TimeSeriesValuesHydro<NextT>,
                                          NextT,
                                          VCardTimeSeriesValuesHydro>;

    void initializeDerivedFromStudy(Data::Study& study)
    {
        // Initialize the vector for fatal values (modern C++ approach)
        fatalValues.resize(BaseType::nbYearsParallel, nullptr);
    }

    void yearBeginImpl(unsigned int year, unsigned int space)
    {
        // The current time-series
        auto& ror = BaseType::areaPtr->hydro.series->ror;
        const unsigned int nbchro = ror.getSeriesIndex(year);
        fatalValues[space] = &(ror.timeSeries.entry[nbchro]);
    }

    void hourForEachAreaImpl(State& state, unsigned int space)
    {
        BaseType::yearlyValues[space][state.hourInTheYear] = (*fatalValues[space])
          [state.hourInTheYear];
    }

private:
    // Modern C++ approach: use vector instead of raw pointer array
    std::vector<Matrix<>::ColumnType*> fatalValues;
};

} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares
