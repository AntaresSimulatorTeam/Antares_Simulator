/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include <yuni/yuni.h>
#include "../../study.h"
#include "container.h"
#include "antares/study/parts/parts.h"

using namespace Yuni;

namespace Antares
{
namespace Data
{
namespace Solar
{
Container::Container() : prepro(nullptr), series(nullptr)
{
}

Container::~Container()
{
    delete prepro;
    delete series;
}

bool Container::forceReload(bool reload) const
{
    bool ret = true;
    if (series)
        ret = series->forceReload(reload) && ret;
    if (prepro)
        ret = prepro->forceReload(reload) && ret;
    return ret;
}

void Container::markAsModified() const
{
    if (series)
        series->markAsModified();
    if (prepro)
        prepro->markAsModified();
}

uint64_t Container::memoryUsage() const
{
    return sizeof(Container) + ((!series) ? 0 : DataSeriesSolarMemoryUsage(series))
           + ((!prepro) ? 0 : prepro->memoryUsage());
}

void Container::resetToDefault()
{
    if (series)
        series->timeSeries.reset(1, HOURS_PER_YEAR);
    if (prepro)
        prepro->resetToDefault();
}

} // namespace Solar
} // namespace Data
} // namespace Antares
