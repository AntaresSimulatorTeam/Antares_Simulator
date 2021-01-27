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

#include "context.h"

using namespace Yuni;

namespace Antares
{
namespace Action
{
Context::Context(Data::Study& targetStudy, const size_t layer) :
 study(targetStudy),
 extStudy(nullptr),
 shouldDestroyExtStudy(false),
 area(nullptr),
 cluster(nullptr),
 link(nullptr),
 originalPlant(nullptr),
 constraint(nullptr),
 layerID(layer)
{
}

Context::~Context()
{
    if (extStudy && shouldDestroyExtStudy)
    {
        delete extStudy;
        extStudy = nullptr;
    }
}

void Context::reset()
{
    area = nullptr;
    cluster = nullptr;
    link = nullptr;
    constraint = nullptr;

    view.clear();
    property.clear();
    areaNameMapping.clear();
    areaLowerNameMapping.clear();
}

} // namespace Action
} // namespace Antares
