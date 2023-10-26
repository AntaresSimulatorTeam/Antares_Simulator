/*
** Copyright 2007-2023 RTE
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

#include <antares/writer/i_writer.h>

#include "area.h"

namespace Antares::Data
{
    template<int TimeSeriesT>
    void singleAreaStoreTimeseriesNumbers(Solver::IResultWriter::Ptr writer, const Area& area);

    template<int TimeSeriesT>
    void storeTimeseriesNumbers(Solver::IResultWriter::Ptr writer, const AreaList& area);

    // Store time-series numbers
    void storeTimeseriesNumbersForLoad(Solver::IResultWriter::Ptr writer, const Area& area);
    void storeTimeseriesNumbersForSolar(Solver::IResultWriter::Ptr writer, const Area& area);
    void storeTimeseriesNumbersForWind(Solver::IResultWriter::Ptr writer, const Area& area);
    void storeTimeseriesNumbersForHydro(Solver::IResultWriter::Ptr writer, const Area& area);
    void storeTimeseriesNumbersForThermal(Solver::IResultWriter::Ptr writer, const Area& area);
    void storeTimeseriesNumbersForRenewable(Solver::IResultWriter::Ptr writer, const Area& area);
    void storeTimeseriesNumbersForTransmissionCapacities(Solver::IResultWriter::Ptr writer, const Area& area);
    void storeTimeseriesNumbersForHydroMaxPower(Solver::IResultWriter::Ptr writer, const Area& area);

} //Antares::Data

#include "store-timeseries-numbers.hxx"
