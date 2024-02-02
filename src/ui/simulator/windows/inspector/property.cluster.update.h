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
#pragma once

#include "data.h"
#include "frame.h"

namespace Antares
{
namespace Window
{
namespace Inspector
{
class ClusterUpdater
{
public:
    explicit ClusterUpdater(Frame& frame);
    bool changeName(const wxVariant& value);
    bool changeGroup(const wxVariant& value);
    bool changeUnit(const wxVariant& value);
    bool changeNominalCapacity(const wxVariant& value);
    bool changeEnabled(const wxVariant& value);

protected:
    Data::Cluster::Set clusters;
    std::vector<const wxChar*> groups;
    Frame& pFrame;
    wxPGProperty* unitCount;
    wxPGProperty* installedCapacity;
    wxPGProperty* nominalCapacity;

private:
    virtual void OnCommonSettingsChanged() = 0;
    virtual void OnStudyClusterGroupChanged(Data::Area*) = 0;
    virtual void OnStudyClusterRenamed(Data::Cluster*) = 0;
};

class ClusterUpdaterThermal : public ClusterUpdater
{
public:
    ClusterUpdaterThermal(InspectorData::Ptr data, Frame& frame);

private:
    virtual void OnCommonSettingsChanged() override;
    virtual void OnStudyClusterGroupChanged(Data::Area* area);
    virtual void OnStudyClusterRenamed(Data::Cluster* cluster);
};

class ClusterUpdaterRenewable : public ClusterUpdater
{
public:
    ClusterUpdaterRenewable(InspectorData::Ptr data, Frame& frame);

private:
    virtual void OnCommonSettingsChanged() override;
    virtual void OnStudyClusterGroupChanged(Data::Area* area);
    virtual void OnStudyClusterRenamed(Data::Cluster* cluster);
};

} // namespace Inspector
} // namespace Window
} // namespace Antares
