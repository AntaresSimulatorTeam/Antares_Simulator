// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_WINDOW_THERMAL_COMMON_H__
#define __ANTARES_APPLICATION_WINDOW_THERMAL_COMMON_H__

#include <ui/common/component/panel.h>
#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/input/thermal-cluster.h"
#include "../../toolbox/validator.h"
#include "../../windows/inspector/data.h"

namespace Antares::Window::Thermal
{
class CommonProperties: public Component::Panel, public Yuni::IEventObserver<CommonProperties>
{
public:
    CommonProperties(wxWindow* parent, Toolbox::InputSelector::ThermalCluster* notifier);
    virtual ~CommonProperties();

private:
    void onThermalClusterChanged(Data::ThermalCluster* cluster);

    void thermalEventConnect();
    void thermalEventDisconnect();

    void onStudyThermalClusterRenamed(Data::ThermalCluster* cluster);

    void thermalSettingsChanged();

    void onStudyClosed();

private:
    //! The main sizer
    wxSizer* pMainSizer;
    Data::ThermalCluster* pAggregate;
    Toolbox::InputSelector::ThermalCluster* pNotifier;
    bool pGroupHasChanged;

    Yuni::Bind<void(const Window::Inspector::InspectorData::Ptr&)> pUpdateInfoAboutPlant;

}; // class Aggregate

} // namespace Antares::Window::Thermal

#endif // __ANTARES_APPLICATION_WINDOW_THERMAL_COMMON_H__
