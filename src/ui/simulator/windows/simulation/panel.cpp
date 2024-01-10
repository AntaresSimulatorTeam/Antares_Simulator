/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "panel.h"
#include <antares/date/date.h>
#include <ui/common/component/scrollbox/scrollbox.h>
#include "../../toolbox/components/notebook/notebook.h"
#include "../../toolbox/components/captionpanel.h"
#include "../../toolbox/components/datagrid/renderer/ts-management-aggregated-as-renewables.h"
#include "../../toolbox/components/datagrid/renderer/ts-management-clusters-as-renewables.h"
#include "../../application/main.h"
#include "../../windows/options/advanced/advanced.h"
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
        using TSmanagementAggregatedAsRenewable
          = Component::Datagrid::Renderer::TSmanagementAggregatedAsRenewable;
        using TSmanagementRenewableCluster
          = Component::Datagrid::Renderer::TSmanagementRenewableCluster;

        verticalSizer_ = new wxBoxSizer(wxVERTICAL);
        verticalSizer_->Add(
          new Component::CaptionPanel(this, wxT("Time-Series Management")), 0, wxALL | wxEXPAND);

        auto renderer_agg = new TSmanagementAggregatedAsRenewable();
        auto renderer_rn_cl = new TSmanagementRenewableCluster();

        grid_ts_mgt_ = new DatagridType(this, renderer_agg, wxEmptyString, false, true, true);
        grid_ts_mgt_rn_cluster_
          = new DatagridType(this, renderer_rn_cl, wxEmptyString, false, true, true);

        verticalSizer_->Add(grid_ts_mgt_, 1, wxALL | wxEXPAND);
        verticalSizer_->Add(grid_ts_mgt_rn_cluster_, 1, wxALL | wxEXPAND);

        renderer_agg->control(grid_ts_mgt_);
        renderer_rn_cl->control(grid_ts_mgt_rn_cluster_);

        hz->Add(verticalSizer_, 1, wxALL | wxEXPAND);
    }

    SetSizer(hz);

    // External events
    Options::OnRenewableGenerationModellingChanged.connect(
      this, &Panel::onRenewableGenerationModellingChanged);
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

void Panel::onRenewableGenerationModellingChanged(bool)
{
    auto study = GetCurrentStudy();
    if (!study)
        return;

    if (study->parameters.renewableGeneration.isAggregated())
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
    auto study = GetCurrentStudy();
    if (!(!study))
    {
        auto data = std::make_shared<Window::Inspector::InspectorData>(study);
        data->studies.insert(study);
        pUpdateInfoStudy(data);
    }
    else
        pUpdateInfoStudy(nullptr);
}

void Panel::onUpdatePlaylist()
{
    auto study = GetCurrentStudy();
    if (!(!study))
    {
        auto data = std::make_shared<Window::Inspector::InspectorData>(study);
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
