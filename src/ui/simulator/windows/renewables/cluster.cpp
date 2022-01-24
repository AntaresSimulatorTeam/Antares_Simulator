/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
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
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "cluster.h"
#include <wx/sizer.h>
#include "../../windows/inspector/frame.h"

#include "../../application/study.h"

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace Renewable
{
CommonProperties::CommonProperties(wxWindow* parent,
                                   Toolbox::InputSelector::RenewableCluster* notifier) :
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

    // Connection with the notifier
    renewableEventConnect();

    OnStudyRenewableClusterRenamed.connect(this, &CommonProperties::onStudyRenewableClusterRenamed);
    OnStudyRenewableClusterCommonSettingsChanged.connect(
      this, &CommonProperties::renewableSettingsChanged);
    OnStudyClosed.connect(this, &CommonProperties::onStudyClosed);
}

CommonProperties::~CommonProperties()
{
    pUpdateInfoAboutPlant.clear();
    destroyBoundEvents();
}

void CommonProperties::onClusterChanged(Data::RenewableCluster* cluster)
{
    if (cluster)
    {
        auto data = Window::Inspector::InspectorData(Data::Study::Current::Get());
        data.RnClusters.insert(cluster);
        // TODO[FO]
        // pUpdateInfoAboutPlant(data);
    }
    else
        pUpdateInfoAboutPlant(nullptr);

    pGroupHasChanged = false;
    pAggregate = cluster;
}

void CommonProperties::onStudyClosed()
{
    onClusterChanged(nullptr);
}

void CommonProperties::renewableEventConnect()
{
    if (pNotifier)
        pNotifier->onClusterChanged.connect(this, &CommonProperties::onClusterChanged);
}

void CommonProperties::renewableEventDisconnect()
{
    if (pNotifier)
        pNotifier->onClusterChanged.remove(this);
}

void CommonProperties::onStudyRenewableClusterRenamed(Data::RenewableCluster* cluster)
{
    if (cluster == pAggregate and cluster)
        onClusterChanged(cluster);
    Dispatcher::GUI::Refresh(this);
}

void CommonProperties::renewableSettingsChanged()
{
    Dispatcher::GUI::Refresh(this);
}

} // namespace Renewable
} // namespace Window
} // namespace Antares
