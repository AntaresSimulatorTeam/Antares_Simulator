// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/load/container.h"

#include "antares/study/parts/load/prepro.h"
#include "antares/study/study.h"

namespace Antares::Data::Load
{
Container::Container():
    series(tsNumbers)
{
}

} // namespace Antares::Data::Load
