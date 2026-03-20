// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/renewable/container.h"

#include <yuni/yuni.h>

#include <antares/logs/logs.h>
#include "antares/study/study.h"

using namespace Yuni;
using namespace Antares;

namespace Antares::Data
{
PartRenewable::PartRenewable()
{
}

PartRenewable::~PartRenewable()
{
}

void PartRenewable::reset()
{
    list.clearAll();
}

} // namespace Antares::Data
