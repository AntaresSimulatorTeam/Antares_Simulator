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

#include "data.h"

namespace Antares
{
namespace Window
{
namespace Inspector
{
InspectorData::InspectorData(Antares::Data::Study::Ptr s) : study(s), empty(true)
{
}

InspectorData::~InspectorData()
{
}

void InspectorData::clear()
{
    if (not empty)
    {
        areas.clear();
        links.clear();
        ThClusters.clear();
        RnClusters.clear();
        constraints.clear();
        studies.clear();
        empty = true;
    }
}

uint InspectorData::totalNbOfItems() const
{
    return (uint)areas.size() + (uint)links.size() + (uint)ThClusters.size() + (uint)RnClusters.size()
           + (uint)constraints.size();
}

} // namespace Inspector
} // namespace Window
} // namespace Antares
