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

#include "antares/study/study.h"

#include <cassert>

using namespace Yuni;

#define SEP IO::Separator

namespace Antares::Data
{
bool Study::importTimeseriesIntoInput()
{
    // Special case: some thermal clusters may force TS generation
    const bool importThermal = parameters.haveToImport(timeSeriesThermal)
                               && runtime->thermalTSRefresh;
    // Something to import ?
    if ((parameters.exportTimeSeriesInInput && parameters.timeSeriesToGenerate) || importThermal)
    {
        // Return status
        bool ret = true;

        // Let's see what data generated by the preprocessors we have to import
        logs.notice() << "Updating the input data...";
        logs.info() << "Importing the generated time-series into the input data";

        Solver::Progression::Task progression(*this, Solver::Progression::sectImportTS);

        // Load
        if (parameters.haveToImport(timeSeriesLoad))
        {
            logs.info() << "Importing load timeseries...";
            for (const auto& [areaName, area] : areas)
            {
                logs.info() << "Importing load timeseries : " << areaName;
                buffer.clear() << folderInput << SEP << "load" << SEP << "series";
                ret = area->load.series.saveToFolder(area->id, buffer.c_str(), "load_") && ret;
                ++progression;
            }
        }

        // Solar
        if (parameters.haveToImport(timeSeriesSolar))
        {
            logs.info() << "Importing solar timeseries...";
            for (const auto& [areaName, area] : areas)
            {
                logs.info() << "Importing solar timeseries : " << areaName;
                buffer.clear() << folderInput << SEP << "solar" << SEP << "series";
                ret = area->solar.series.saveToFolder(area->id, buffer.c_str(), "solar_") && ret;
                ++progression;
            }
        }

        // Hydro
        if (parameters.haveToImport(timeSeriesHydro))
        {
            logs.info() << "Importing hydro timeseries...";
            for (const auto& [areaName, area] : areas)
            {
                logs.info() << "Importing hydro timeseries : " << areaName;
                buffer.clear() << folderInput << SEP << "hydro" << SEP << "series";
                ret = area->hydro.series->saveToFolder(area->id, buffer) && ret;
                ++progression;
            }
        }

        // Wind
        if (parameters.haveToImport(timeSeriesWind))
        {
            logs.info() << "Importing wind timeseries...";
            for (const auto& [areaName, area] : areas)
            {
                logs.info() << "Importing wind timeseries : " << areaName;
                buffer.clear() << folderInput << SEP << "wind" << SEP << "series";
                area->wind.series.saveToFolder(area->id, buffer.c_str(), "wind_") && ret;
                ++progression;
            }
        }

        // Thermal
        if (importThermal)
        {
            logs.info() << "Importing thermal timeseries...";
            String msg;

            for (const auto& [areaName, area] : areas)
            {
                msg.clear() << "Importing thermal timeseries : " << areaName;

                // Spinning
                area->thermal.list.reverseCalculationOfSpinning();

                buffer.clear() << folderInput << SEP << "thermal" << SEP << "series";
                ret = area->thermal.list.saveDataSeriesToFolder(buffer.c_str()) && ret;
                ++progression;
            }
        }

        return ret;
    }
    return true;
}

} // namespace Antares::Data