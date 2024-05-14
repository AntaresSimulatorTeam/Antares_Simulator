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
#pragma once

#include <antares/writer/i_writer.h>

#include "area.h"

namespace Antares::Data
{
template<int TimeSeriesT>
void singleAreaStoreTimeseriesNumbers(Solver::IResultWriter& writer, const Area& area);

template<int TimeSeriesT>
void storeTimeseriesNumbers(Solver::IResultWriter& writer, const AreaList& area);

// Store time-series numbers
void storeTimeseriesNumbersForLoad(Solver::IResultWriter& writer, const Area& area);
void storeTimeseriesNumbersForSolar(Solver::IResultWriter& writer, const Area& area);
void storeTimeseriesNumbersForWind(Solver::IResultWriter& writer, const Area& area);
void storeTimeseriesNumbersForHydro(Solver::IResultWriter& writer, const Area& area);
void storeTimeseriesNumbersForThermal(Solver::IResultWriter& writer, const Area& area);
void storeTimeseriesNumbersForRenewable(Solver::IResultWriter& writer, const Area& area);
void storeTimeseriesNumbersForTransmissionCapacities(Solver::IResultWriter& writer,
                                                     const Area& area);
void storeTimeseriesNumbersForHydroMaxPower(Solver::IResultWriter& writer, const Area& area);

} // namespace Antares::Data

#include "store-timeseries-numbers.hxx"
