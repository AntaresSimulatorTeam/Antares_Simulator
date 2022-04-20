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
ClusterUpdater::ClusterUpdater(InspectorData::Ptr data, Frame& frame) : pFrame(frame), clusters()
{
}
bool ClusterUpdater::changeName(const wxVariant& value)
{
    if (clusters.size() != 1)
        return false;
    Data::ClusterName name;
    wxStringToString(value.GetString(), name);
    name.trim(" \r\n\t");
    if (!name)
        return false;

    Data::Cluster* cluster = *(clusters.begin());
    auto study = Data::Study::Current::Get();
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
    Data::ClusterName name;

    if (not newgroup.empty())
    {
        long index;
        if (newgroup.to(index))
        {
            if (index < 0 || index > groups.size())
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

    typedef Data::Area* AreaType;
    typedef std::set<AreaType> SetType;
    SetType set;

    for (auto cluster : clusters)
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
    uint d = static_cast<uint>(value.GetLong());
    if (d > 100)
    {
        logs.error() << "A cluster can not have more than 100 units";
        for (auto cluster : clusters)
            cluster->unitCount = 100;
        Accumulator<PClusterUnitCount>::Apply(unitCount, clusters);
    }
    else
    {
        for (auto cluster : clusters)
            cluster->unitCount = d;
    }
    // refresh the installed capacity
    Accumulator<PClusterInstalled, Add>::Apply(installedCapacity, clusters);

    // Notify
    OnCommonSettingsChanged();

    if (d > 100)
        pFrame.delayApply();
    return true;
}

bool ClusterUpdater::changeNominalCapacity(const wxVariant& value)
{
    double d = value.GetDouble();
    if (d < 0.)
    {
        for (auto cluster : clusters)
            cluster->nominalCapacity = 0.;
        pFrame.delayApply();
    }
    else
    {
        for (auto cluster : clusters)
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
    for (auto cluster : clusters)
        cluster->enabled = d;
    // Notify
    OnCommonSettingsChanged();
    return true;
}

// ClusterUpdaterThermal
ClusterUpdaterThermal::ClusterUpdaterThermal(InspectorData::Ptr data, Frame& frame) :
 ClusterUpdater(data, frame)
{
    // wxProperties
    unitCount = frame.pPGThClusterUnitCount;
    installedCapacity = frame.pPGThClusterInstalled;
    nominalCapacity = frame.pPGThClusterNominalCapacity;

    clusters = Data::Cluster::Set(data->ThClusters.begin(), data->ThClusters.end());
    groups = std::vector<const wxChar*>(std::begin(arrayClusterGroup), std::end(arrayClusterGroup));
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
 ClusterUpdater(data, frame)
{
    // wxProperties
    unitCount = frame.pPGRnClusterUnitCount;
    installedCapacity = frame.pPGRnClusterInstalled;
    nominalCapacity = frame.pPGRnClusterNominalCapacity;

    clusters = Data::Cluster::Set(data->RnClusters.begin(), data->RnClusters.end());
    groups
      = std::vector<const wxChar*>(std::begin(arrayRnClusterGroup), std::end(arrayRnClusterGroup));
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
