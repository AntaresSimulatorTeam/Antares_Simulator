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

#include "panel.h"
#include <antares/date.h>
#include <ui/common/component/scrollbox/scrollbox.h>
#include "../../toolbox/components/notebook/notebook.h"
#include "../../toolbox/components/captionpanel.h"
#include "../../toolbox/components/datagrid/renderer/ts-management.h"
#include "../../toolbox/components/datagrid/renderer/ts-management-clusters-as-renewables.h"
#include "../../application/main.h"
#include "../../application/study.h"
#include "../../toolbox/validator.h"
#include "../../toolbox/resources.h"
#include "../../toolbox/create.h"
#include "../inspector.h"
#include "../../windows/message.h"
#include "../../windows/inspector/frame.h"
#include "../../toolbox/components/captionpanel.h"

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace Simulation
{
Panel::Panel(wxWindow* parent) : Antares::Component::Panel(parent)
{
    auto* hz = new wxBoxSizer(wxHORIZONTAL);

    // General Parameters
    {
        auto* vs = new wxBoxSizer(wxVERTICAL);
        vs->Add(new Component::CaptionPanel(this, wxT("General Parameters")), 0, wxALL | wxEXPAND);

        auto* inspector = new Window::Inspector::Frame(this);
        pUpdateInfoStudy.bind(inspector, &Window::Inspector::Frame::apply);
        vs->Add(inspector, 1, wxALL | wxEXPAND);
        vs->SetItemMinSize(inspector, 300, 200);

        hz->Add(vs, 0, wxALL | wxEXPAND);
    }

    // Separator
    // Component::AddVerticalSeparator(this, hz);

    // TS Management
    {
        typedef Component::Datagrid::Renderer::TSmanagement TSmanagement;
        typedef Component::Datagrid::Renderer::TSmanagementRenewableCluster TSmanagementRenewableCluster;
        
        verticalSizer_ = new wxBoxSizer(wxVERTICAL);
        verticalSizer_->Add(
            new Component::CaptionPanel(this, wxT("Time-Series Management")), 0, wxALL | wxEXPAND);

        TSmanagement* renderer = new TSmanagement();
        TSmanagementRenewableCluster* renderer_rn_cl = new TSmanagementRenewableCluster();

        grid_ts_mgt_ = new DatagridType(this, renderer, wxEmptyString, false, true, true);
        grid_ts_mgt_rn_cluster_ = new DatagridType(this, renderer_rn_cl, wxEmptyString, false, true, true);

        verticalSizer_->Add(grid_ts_mgt_, 1, wxALL | wxEXPAND);
        verticalSizer_->Add(grid_ts_mgt_rn_cluster_, 1, wxALL | wxEXPAND);

        renderer->control(grid_ts_mgt_);
        renderer_rn_cl->control(grid_ts_mgt_rn_cluster_);

        hz->Add(verticalSizer_, 1, wxALL | wxEXPAND);
    }

    SetSizer(hz);

    // External events
    OnStudyLoaded.connect(this, &Panel::onStudyLoaded);
    OnStudyClosed.connect(this, &Panel::onStudyClosed);
    OnStudyUpdatePlaylist.connect(this, &Panel::onUpdatePlaylist);
}

Panel::~Panel()
{
    pUpdateInfoStudy.clear();
    destroyBoundEvents();

    // To avoid corrupt vtable in some rare cases / misuses
    // (when children try to access to this object for example),
    // we should destroy all children as soon as possible.
    wxSizer* sizer = GetSizer();
    if (sizer)
        sizer->Clear(true);
}

void Panel::onStudyLoaded()
{
    auto study = Data::Study::Current::Get();
    if (!study)
        return;
    
    if (study->parameters.renewableGeneration.rgModelling == Antares::Data::rgAggregated)
    {
        verticalSizer_->Hide(grid_ts_mgt_rn_cluster_);
        verticalSizer_->Show(grid_ts_mgt_);
        grid_ts_mgt_->forceRefresh();
    }
    else
    {
        verticalSizer_->Hide(grid_ts_mgt_);
        verticalSizer_->Show(grid_ts_mgt_rn_cluster_);
        grid_ts_mgt_rn_cluster_->forceRefresh();
    }
    verticalSizer_->Layout();
    
    Dispatcher::GUI::Post(this, &Panel::onDelayedStudyLoaded, 20 /*ms*/);
}

void Panel::onDelayedStudyLoaded()
{
    auto study = Data::Study::Current::Get();
    if (!(!study))
    {
        auto* data = new Window::Inspector::InspectorData(*study);
        data->studies.insert(study);
        pUpdateInfoStudy(data);
    }
    else
        pUpdateInfoStudy(nullptr);
}

void Panel::onUpdatePlaylist()
{
    auto study = Data::Study::Current::Get();
    if (!(!study))
    {
        auto* data = new Window::Inspector::InspectorData(*study);
        data->studies.insert(study);
        pUpdateInfoStudy(data);
    }
    else
        pUpdateInfoStudy(nullptr);
}

void Panel::onStudyClosed()
{
    pUpdateInfoStudy(nullptr);
}

} // namespace Simulation
} // namespace Window
} // namespace Antares
