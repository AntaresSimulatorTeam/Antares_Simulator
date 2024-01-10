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
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
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
    return (uint)areas.size() + (uint)links.size() + (uint)ThClusters.size()
           + (uint)RnClusters.size() + (uint)constraints.size();
}

} // namespace Inspector
} // namespace Window
} // namespace Antares
