// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <yuni/yuni.h>
#include <yuni/core/math.h>

#include <antares/logs/logs.h>
#include <antares/study/study.h>
#include "antares/solver/ts-generator/xcast/xcast.h"

using namespace Yuni;

namespace Antares::TSGenerator::XCast
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

} // namespace Antares::TSGenerator::XCast
