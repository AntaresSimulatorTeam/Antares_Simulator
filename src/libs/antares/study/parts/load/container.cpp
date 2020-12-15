/*
** Copyright 2007-2018 RTE
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

using namespace Yuni;

namespace Antares
{
namespace Data
{
namespace Load
{
Container::Container() : prepro(nullptr), series(nullptr)
{
}

Container::~Container()
{
    delete prepro;
    prepro = nullptr;
    delete series;
    series = nullptr;
}

bool Container::invalidate(bool reload) const
{
    bool ret = true;
    if (series)
        ret = series->invalidate(reload) && ret;
    if (prepro)
        ret = prepro->invalidate(reload) && ret;
    return ret;
}

void Container::markAsModified() const
{
    if (series)
        series->markAsModified();
    if (prepro)
        prepro->markAsModified();
}

Yuni::uint64 Container::memoryUsage() const
{
    return sizeof(Container) + ((!series) ? 0 : DataSeriesLoadMemoryUsage(series))
           + ((!prepro) ? 0 : prepro->memoryUsage());
}

void Container::resetToDefault()
{
    if (series)
        series->series.reset(1, HOURS_PER_YEAR);
    if (prepro)
        prepro->resetToDefault();
}

} // namespace Load
} // namespace Data
} // namespace Antares
