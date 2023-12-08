/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
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
**
** SPDX-License-Identifier: MPL 2.0
*/
#ifndef __ANTARES_APPLICATION_WINDOWS_SIMULATION_PANEL_H__
#define __ANTARES_APPLICATION_WINDOWS_SIMULATION_PANEL_H__

#include "../inspector/data.h"
#include <ui/common/component/panel.h>
#include "../../toolbox/components/datagrid/component.h"

namespace Antares
{
namespace Window
{
namespace Simulation
{
/*!
** \brief Panel to access to the simulation settings
*/
class Panel final : public Antares::Component::Panel, public Yuni::IEventObserver<Panel>
{
    using DatagridType = Component::Datagrid::Component;

public:
    //! \name Constructor && Destructor
    //@{
    /*!
    ** \brief Default constructor
    **
    ** \param parent The parent window
    */
    Panel(wxWindow* parent);
    //! Destructor
    virtual ~Panel();
    //@}

private:
    //! In case renewable generation modelling was changed
    void onRenewableGenerationModellingChanged(bool);
    //! The study has been closed
    void onStudyClosed();
    //! A study has been loaded (delayed)
    void onDelayedStudyLoaded();
    //!
    void onUpdatePlaylist();
    //! local inspector update about the study
    Yuni::Bind<void(const Window::Inspector::InspectorData::Ptr&)> pUpdateInfoStudy;

private:
    wxBoxSizer* verticalSizer_;
    DatagridType* grid_ts_mgt_;
    DatagridType* grid_ts_mgt_rn_cluster_;
}; // class Panel

} // namespace Simulation
} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOWS_SIMULATION_PANEL_H__
