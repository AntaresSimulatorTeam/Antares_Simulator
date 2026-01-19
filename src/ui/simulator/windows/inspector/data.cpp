// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "data.h"

namespace Antares::Window::Inspector
{
InspectorData::InspectorData(Antares::Data::Study::Ptr s):
    study(s),
    empty(true)
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

} // namespace Antares::Window::Inspector
