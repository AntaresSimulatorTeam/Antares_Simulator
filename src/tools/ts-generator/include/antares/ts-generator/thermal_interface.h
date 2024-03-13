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
# pragma once

#include <vector>

#include <antares/study/parts/thermal/cluster.h>

namespace Antares::TsGenerator
{

class preproVector
{
public:
    preproVector() : mVec(8760) {}
    double& operator[](std::size_t idx) { return mVec[idx];}

private:
    std::vector<double> mVec;
};

class ThermalInterface
{
public:

    template<class T>
    ThermalInterface(T source);

    preproVector foDuration;
    preproVector poDuration;
    preproVector foRate;
    preproVector poRate;
    preproVector npoMin;
    preproVector npoMax;

    unsigned unitCount;
    float nominalCapacity;

    double forcedVolatility;
    double plannedVolatility;

    Data::ThermalLaw forcedLaw = Data::thermalLawUniform;
    Data::ThermalLaw plannedLaw = Data::thermalLawUniform;
};

template<class T>
ThermalInterface::ThermalInterface(T source)
{}

} // namespace Antares::TsGenerator
