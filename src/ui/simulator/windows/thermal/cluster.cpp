// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "cluster.h"
#include "../../windows/inspector.h"
#include "../../toolbox/components/datagrid/renderer/area/thermalmodulation.h"
#include "../../toolbox/components/notebook/notebook.h"
#include "../../toolbox/components/refresh.h"
#include <antares/study/parts/thermal/cluster.h>
#include <wx/sizer.h>
#include "../../windows/inspector/frame.h"
#include <ui/common/dispatcher/gui.h>

using namespace Yuni;

namespace Antares::Window::Thermal
{
CommonProperties::CommonProperties(wxWindow* parent,
                                   Toolbox::InputSelector::ThermalCluster* notifier):
    Component::Panel(parent),
    pMainSizer(nullptr),
    pAggregate(nullptr),
    pNotifier(notifier),
    pGroupHasChanged(false)
{
    // The main sizer
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    pMainSizer = sizer;
    SetSizer(sizer);

    auto* inspector = new Window::Inspector::Frame(this);
    pUpdateInfoAboutPlant.bind(inspector, &Window::Inspector::Frame::apply);

    wxBoxSizer* vs = new wxBoxSizer(wxVERTICAL);
    vs->Add(inspector, 1, wxALL | wxEXPAND);
    sizer->Add(vs, 0, wxALL | wxEXPAND);
    sizer->SetItemMinSize(inspector, 280, 50);

    sizer->Add(new Component::Datagrid::Component(
                 this,
                 new Component::Datagrid::Renderer::ThermalClusterCommonModulation(this, notifier)),
               1,
               wxALL | wxEXPAND);

    // Connection with the notifier
    thermalEventConnect();

    OnStudyThermalClusterRenamed.connect(this, &CommonProperties::onStudyThermalClusterRenamed);
    OnStudyThermalClusterCommonSettingsChanged.connect(this,
                                                       &CommonProperties::thermalSettingsChanged);
    OnStudyClosed.connect(this, &CommonProperties::onStudyClosed);
}

CommonProperties::~CommonProperties()
{
    pUpdateInfoAboutPlant.clear();
    destroyBoundEvents();
}

void CommonProperties::onThermalClusterChanged(Data::ThermalCluster* cluster)
{
    if (cluster)
    {
        auto data = std::make_shared<Window::Inspector::InspectorData>(GetCurrentStudy());
        data->ThClusters.insert(cluster);
        pUpdateInfoAboutPlant(data);
    }
    else
    {
        pUpdateInfoAboutPlant(nullptr);
    }

    pGroupHasChanged = false;
    pAggregate = cluster;
}

void CommonProperties::onStudyClosed()
{
    onThermalClusterChanged(nullptr);
}

void CommonProperties::thermalEventConnect()
{
    if (pNotifier)
    {
        pNotifier->onThermalClusterChanged.connect(this,
                                                   &CommonProperties::onThermalClusterChanged);
    }
}

void CommonProperties::thermalEventDisconnect()
{
    if (pNotifier)
    {
        pNotifier->onThermalClusterChanged.remove(this);
    }
}

void CommonProperties::onStudyThermalClusterRenamed(Data::ThermalCluster* cluster)
{
    if (cluster == pAggregate and cluster)
    {
        onThermalClusterChanged(cluster);
    }
    Dispatcher::GUI::Refresh(this);
}

void CommonProperties::thermalSettingsChanged()
{
    Dispatcher::GUI::Refresh(this);
}

} // namespace Antares::Window::Thermal
