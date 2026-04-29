// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "output.h"

#include <antares/logs/logs.h>

using namespace Yuni;
using namespace Antares;

void Output::incrementError()
{
    if (++errors == 100)
    {
        logs.fatal() << "Too many errors for " << path;
    }
}
