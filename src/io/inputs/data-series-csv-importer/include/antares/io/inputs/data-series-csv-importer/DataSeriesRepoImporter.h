/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include <string>

namespace Antares::Optimisation::LinearProblemDataImpl
{
class DataSeriesRepository;
class TimeSeriesSet;
} // namespace Antares::Optimisation::LinearProblemDataImpl

/**
 * Reads a DataSeriesRepo from a directory
 * Every csv file found represents a DataSeries, its id will be the name of the file
 */
namespace Antares::IO::Inputs::DataSeriesCsvImporter
{

class TimeSeriesSetImporter
{
public:
    static Optimisation::LinearProblemDataImpl::TimeSeriesSet importFromFile(
      const std::string& path,
      char csvSeparator = ';');
};

class DataSeriesRepoImporter
{
public:
    static Optimisation::LinearProblemDataImpl::DataSeriesRepository importFromDirectory(
      const std::string& path,
      char csvSeparator = ';');
};

} // namespace Antares::IO::Inputs::DataSeriesCsvImporter
