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
#ifndef __ANTARES_APPLICATION_WINDOWS_SIMULATION_PANEL_H__
#define __ANTARES_APPLICATION_WINDOWS_SIMULATION_PANEL_H__

#include <antares/wx-wrapper.h>
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
    typedef Component::Datagrid::Component DatagridType;

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
    void onRenewableGenerationModellingChanged();
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
