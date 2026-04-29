// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <iostream>

#include <yuni/yuni.h>
#include <yuni/core/math.h>

#include <antares/logs/logs.h>
#include <antares/study/study.h>
#include "antares/solver/ts-generator/xcast/xcast.h"

using namespace Yuni;

#define PI 3.141592653589793

namespace Antares::TSGenerator::XCast
{
double XCast::GammaEuler(double z)
{
    if (Yuni::Logs::Verbosity::Debug::enabled)
    {
        assert((z > 0. && z <= 100.) && "error in the Gamma function (invalid parameter)");
    }
    else
    {
        if (z <= 0. || z > 100.)
        {
            logs.error() << "Internal error in the Gamma function (invalid parameter)";
            return 0.;
        }
    }

    enum
    {
        g = 6,
    };

    double x;
    const double t = z + double(g) + 0.5;
    double rho;
    static const double p[9] = {0.99999999999980993,
                                676.5203681218851,
                                -1259.1392167224028,
                                771.32342877765313,
                                -176.61502916214059,
                                12.507343278686905,
                                -0.13857109526572012,
                                9.9843695780195716e-6,
                                1.5056327351493116e-7};

    if (z < 0.5)
    {
        z = 1. - z;
        rho = PI / sin(PI * z);
        z -= 1.;
        x = p[0];

        for (int i = 1; i < g + 3; ++i)
        {
            x += p[i] / (z + double(i));
        }

        x = 1. / (2.506628275 * pow(t, z + 0.5) * exp(-t) * x);
    }
    else
    {
        rho = 1.;
        z -= 1.;
        x = p[0];

        for (int i = 1; i < g + 3; ++i)
        {
            x += p[i] / (z + double(i));
        }

        x = 2.506628275 * pow(t, z + 0.5) * exp(-t) * x;
    }

    return (rho * x);
}

} // namespace Antares::TSGenerator::XCast
