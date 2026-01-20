// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_HYDROOW_HYDRO_PREPRO_H__
#define __ANTARES_APPLICATION_HYDROOW_HYDRO_PREPRO_H__

#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/input/area.h"
#include <ui/common/component/panel.h>
#include "../../toolbox/components/button.h"

namespace Antares::Window::Hydro
{
class Prepro: public Component::Panel, public Yuni::IEventObserver<Prepro>
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    Prepro(wxWindow* parent, Toolbox::InputSelector::Area* notifier);
    //! Destructor
    virtual ~Prepro();
    //@}

private:
    void createComponents();
    void onStudyClosed();
    void onAreaChanged(Data::Area* area);

    void onIntermonthlyCorrelationChanged(wxCommandEvent& evt);
    // void onPumpingEfficiencyChanged(wxCommandEvent& evt);

private:
    //! The input area selector
    Toolbox::InputSelector::Area* pInputAreaSelector;
    Data::Area* pArea;
    bool pComponentsAreReady;
    Component::Panel* pSupport;
    wxTextCtrl* pIntermonthlyCorrelation;
    // wxTextCtrl* pPumpingEfficiency;

}; // class Prepro

} // namespace Antares::Window::Hydro

#endif // __ANTARES_APPLICATION_HYDROOW_HYDRO_PREPRO_H__
