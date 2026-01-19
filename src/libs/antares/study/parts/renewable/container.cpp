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

bool PartRenewable::forceReload(bool reload) const
{
    bool ret = true;
    ret = list.forceReload(reload) && ret;
    return ret;
}

void PartRenewable::markAsModified() const
{
    list.markAsModified();
}

PartRenewable::~PartRenewable()
{
}

void PartRenewable::reset()
{
    list.clearAll();
}

} // namespace Antares::Data
