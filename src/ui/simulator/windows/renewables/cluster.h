// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_WINDOW_RENEWABLE_COMMON_H__
#define __ANTARES_APPLICATION_WINDOW_RENEWABLE_COMMON_H__

// #include <ui/common/component/panel.h>
// #include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/input/renewable-cluster.h"
// #include "../../toolbox/validator.h"
#include "../../windows/inspector/data.h"

#include "../../toolbox/input/renewable-cluster.h"

namespace Antares::Window::Renewable
{
class CommonProperties: public Component::Panel, public Yuni::IEventObserver<CommonProperties>
{
public:
    CommonProperties(wxWindow* parent, Toolbox::InputSelector::RenewableCluster* notifier);
    virtual ~CommonProperties();

private:
    void onClusterChanged(Data::RenewableCluster* cluster);

    void renewableEventConnect();
    void renewableEventDisconnect();

    void onStudyRenewableClusterRenamed(Data::RenewableCluster* cluster);

    void renewableSettingsChanged();

    void onStudyClosed();

private:
    //! The main sizer
    wxSizer* pMainSizer;
    Data::RenewableCluster* pAggregate;
    Toolbox::InputSelector::RenewableCluster* pNotifier;
    bool pGroupHasChanged;

    Yuni::Bind<void(const Window::Inspector::InspectorData::Ptr&)> pUpdateInfoAboutPlant;

}; // class Aggregate

} // namespace Antares::Window::Renewable

#endif // __ANTARES_APPLICATION_WINDOW_RENEWABLE_COMMON_H__
