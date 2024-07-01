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

#include <antares/study/study.h>
#include "antares/solver/ts-generator/xcast/xcast.h"

namespace Antares
{
namespace TSGenerator
{
namespace XCast
{
void XCast::normal(float& x, float& y)
{
    assert(random);

    double z;
    double xd;
    double yd;

    do
    {
        xd = 2. * random->next() - 1.;
        yd = 2. * random->next() - 1.;
        z = (xd * xd) + (yd * yd);
    } while (z > 1.);

    const double u = sqrt(-2. * log(z) / z);

    x = float(xd * u);
    y = float(yd * u);
}

} // namespace XCast
} // namespace TSGenerator
} // namespace Antares
