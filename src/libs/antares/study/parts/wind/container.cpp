// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/wind/container.h"

#include <yuni/yuni.h>

#include "antares/study/study.h"

using namespace Yuni;

namespace Antares::Data::Wind
{
Container::Container():
    series(tsNumbers)
{
}

void Container::resetToDefault()
{
    series.reset();
    if (prepro)
    {
        prepro->resetToDefault();
    }
}

} // namespace Antares::Data::Wind
