// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/study/study.h>
#include "antares/solver/ts-generator/xcast/xcast.h"

namespace Antares::TSGenerator::XCast
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

} // namespace Antares::TSGenerator::XCast
