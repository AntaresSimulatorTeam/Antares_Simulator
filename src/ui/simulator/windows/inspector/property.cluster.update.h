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
    ClusterUpdater(Frame& frame);
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
