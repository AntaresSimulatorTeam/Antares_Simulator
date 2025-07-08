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
#include "property.cluster.update.h"
#include <antares/study/area/constants.h>
#include <ui/simulator/application/study.h>

using namespace Yuni;

#include "accumulator.hxx"

namespace Antares
{
namespace Window
{
namespace Inspector
{
// ClusterUpdater
ClusterUpdater::ClusterUpdater(Frame& frame) : pFrame(frame)
{
}
bool ClusterUpdater::changeName(const wxVariant& value)
{
    if (clusters.size() != 1)
        return false;
    YString name;
    wxStringToString(value.GetString(), name);
    name.trim(" \r\n\t");
    if (!name)
        return false;

    Data::Cluster* cluster = *(clusters.begin());
    auto study = GetCurrentStudy();
    if (!(!study) && study->clusterRename(cluster, name))
    {
        // Notify
        OnStudyClusterRenamed(cluster);
        cluster->markAsModified();
        return true;
    }
    return false;
}

bool ClusterUpdater::changeGroup(const wxVariant& value)
{
    wxString vs = value.GetString();
    String newgroup;
    wxStringToString(vs, newgroup);
    YString name;

    if (not newgroup.empty())
    {
        uint index;
        if (newgroup.to(index))
        {
            if (index > groups.size())
            {
                logs.error() << "The group index is invalid";
                return false;
            }
            const wxChar* const wName = groups[index];
            wxStringToString(wName, name);
            name.trim(" \r\n\t");
            if (!name)
                return false;
        }
        else
            name = newgroup;
    }
    else
        name = newgroup;

    // TODO RegEx are good sometimes...
    name.replace('/', '-');
    name.replace('\\', '-');
    name.replace(',', '-');
    name.replace('(', '-');
    name.replace(')', '-');
    name.replace('?', '-');
    name.replace(':', '-');

    using AreaType = Data::Area*;
    using SetType = std::set<AreaType>;
    SetType set;

    for (auto& cluster : clusters)
    {
        if (cluster->group() != name)
        {
            cluster->setGroup(name);
            set.insert(cluster->parentArea);
        }
    }

    if (!set.empty())
    {
        const SetType::iterator end = set.end();
        for (SetType::iterator i = set.begin(); i != end; ++i)
            OnStudyClusterGroupChanged(*i);
    }
    return true;
}

bool ClusterUpdater::changeUnit(const wxVariant& value)
{
    using unitT = decltype(Antares::Data::ThermalCluster::unitCount);
    const auto nbUnits = static_cast<unitT>(value.GetLong());
    for (auto& cluster : clusters)
        cluster->unitCount = nbUnits;

    // refresh the installed capacity
    Accumulator<PClusterInstalled, Add>::Apply(installedCapacity, clusters);

    // Notify
    OnCommonSettingsChanged();

    return true;
}

bool ClusterUpdater::changeNominalCapacity(const wxVariant& value)
{
    double d = value.GetDouble();
    if (d < 0.)
    {
        for (auto& cluster : clusters)
            cluster->nominalCapacity = 0.;
        pFrame.delayApply();
    }
    else
    {
        for (auto& cluster : clusters)
            cluster->nominalCapacity = d;
    }

    // refresh the installed capacity
    Accumulator<PClusterNomCapacity>::Apply(nominalCapacity, clusters);
    Accumulator<PClusterInstalled, Add>::Apply(installedCapacity, clusters);

    // Notify
    OnCommonSettingsChanged();
    return true;
}

bool ClusterUpdater::changeEnabled(const wxVariant& value)
{
    const bool d = value.GetBool();
    for (auto& cluster : clusters)
        cluster->enabled = d;
    // Notify
    OnCommonSettingsChanged();
    return true;
}

// ClusterUpdaterThermal
ClusterUpdaterThermal::ClusterUpdaterThermal(InspectorData::Ptr data, Frame& frame) :
 ClusterUpdater(frame)
{
    // wxProperties
    unitCount = frame.pPGThClusterUnitCount;
    installedCapacity = frame.pPGThClusterInstalled;
    nominalCapacity = frame.pPGThClusterNominalCapacity;

    clusters = Data::Cluster::Set(data->ThClusters.begin(), data->ThClusters.end());
    groups
      = std::vector<const wxChar*>(arrayClusterGroup, arrayClusterGroup + arrayClusterGroupCount);
}

void ClusterUpdaterThermal::OnCommonSettingsChanged()
{
    OnStudyThermalClusterCommonSettingsChanged();
}
void ClusterUpdaterThermal::OnStudyClusterGroupChanged(Data::Area* area)
{
    OnStudyThermalClusterGroupChanged(area);
}
void ClusterUpdaterThermal::OnStudyClusterRenamed(Data::Cluster* cluster)
{
    OnStudyThermalClusterRenamed(dynamic_cast<Data::ThermalCluster*>(cluster));
}

// ClusterUpdaterRenewable
ClusterUpdaterRenewable::ClusterUpdaterRenewable(InspectorData::Ptr data, Frame& frame) :
 ClusterUpdater(frame)
{
    // wxProperties
    unitCount = frame.pPGRnClusterUnitCount;
    installedCapacity = frame.pPGRnClusterInstalled;
    nominalCapacity = frame.pPGRnClusterNominalCapacity;

    clusters = Data::Cluster::Set(data->RnClusters.begin(), data->RnClusters.end());
    groups = std::vector<const wxChar*>(arrayRnClusterGroup,
                                        arrayRnClusterGroup + arrayRnClusterGroupCount);
}

void ClusterUpdaterRenewable::OnCommonSettingsChanged()
{
    OnStudyRenewableClusterCommonSettingsChanged();
}
void ClusterUpdaterRenewable::OnStudyClusterGroupChanged(Data::Area* area)
{
    OnStudyRenewableClusterGroupChanged(area);
}
void ClusterUpdaterRenewable::OnStudyClusterRenamed(Data::Cluster* cluster)
{
    OnStudyRenewableClusterRenamed(dynamic_cast<Data::RenewableCluster*>(cluster));
}
} // namespace Inspector
} // namespace Window
} // namespace Antares
