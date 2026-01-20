// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_WINDOWS_SIMULATION_PANEL_H__
#define __ANTARES_APPLICATION_WINDOWS_SIMULATION_PANEL_H__

#include "../inspector/data.h"
#include <ui/common/component/panel.h>
#include "../../toolbox/components/datagrid/component.h"

namespace Antares::Window::Simulation
{
/*!
** \brief Panel to access to the simulation settings
*/
class Panel final: public Antares::Component::Panel, public Yuni::IEventObserver<Panel>
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

} // namespace Antares::Window::Simulation

#endif // __ANTARES_APPLICATION_WINDOWS_SIMULATION_PANEL_H__
