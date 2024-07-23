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
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_APPLICATION_WINDOW_RENEWABLE_COMMON_H__
#define __ANTARES_APPLICATION_WINDOW_RENEWABLE_COMMON_H__

// #include <ui/common/component/panel.h>
// #include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/input/renewable-cluster.h"
// #include "../../toolbox/validator.h"
#include "../../windows/inspector/data.h"

#include "../../toolbox/input/renewable-cluster.h"

namespace Antares
{
namespace Window
{
namespace Renewable
{
class CommonProperties : public Component::Panel, public Yuni::IEventObserver<CommonProperties>
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

} // namespace Renewable
} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOW_RENEWABLE_COMMON_H__
