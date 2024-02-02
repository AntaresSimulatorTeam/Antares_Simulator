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
#include "../../study.h"
#include "container.h"

using namespace Yuni;

namespace Antares
{
namespace Data
{
namespace Wind
{
Container::Container() : prepro(nullptr), series(tsNumbers)
{
}

Container::~Container()
{
    delete prepro;
}

bool Container::forceReload(bool reload) const
{
    bool ret = true;
    ret = series.forceReload(reload) && ret;
    if (prepro)
        ret = prepro->forceReload(reload) && ret;
    return ret;
}

void Container::markAsModified() const
{
    series.markAsModified();
    if (prepro)
        prepro->markAsModified();
}

uint64_t Container::memoryUsage() const
{
    return sizeof(Container) + series.memoryUsage() + ((!prepro) ? 0 : prepro->memoryUsage());
}

void Container::resetToDefault()
{
    series.reset();
    if (prepro)
        prepro->resetToDefault();
}

} // namespace Wind
} // namespace Data
} // namespace Antares
