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

#include <yuni/yuni.h>
#include <yuni/core/math.h>

#include <antares/logs/logs.h>
#include <antares/study/study.h>
#include "antares/solver/ts-generator/xcast/xcast.h"

using namespace Yuni;

namespace Antares
{
namespace TSGenerator
{
namespace XCast
{
double XCast::GammaInc(double s, double z)
{
    enum
    {
        length = 30,
    };

    double x = 1. / z;
    double somme = 0.;
    for (int i = 0; i < length + 1; ++i)
    {
        x *= z;
        x /= (s + double(i));
        somme += x;
    }
    somme *= exp(-z);
    somme *= pow(z, s);
    return somme;
}

} // namespace XCast
} // namespace TSGenerator
} // namespace Antares
