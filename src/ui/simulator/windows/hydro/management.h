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
#ifndef __ANTARES_APPLICATION_HYDROOW_HYDRO_MANAGEMENT_H__
#define __ANTARES_APPLICATION_HYDROOW_HYDRO_MANAGEMENT_H__

#include <antares/wx-wrapper.h>
#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/input/area.h"
#include <ui/common/component/panel.h>
#include "../../toolbox/components/button.h"

namespace Antares
{
namespace Window
{
namespace Hydro
{
class Management : public wxScrolledWindow, public Yuni::IEventObserver<Management>
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    Management(wxWindow* parent, Toolbox::InputSelector::Area* notifier);
    //! Destructor
    virtual ~Management();
    //@}

private:
    void createComponents();
    void onStudyClosed();
    void onAreaChanged(Data::Area* area);

    void onInterdailyBreakdownChanged(wxCommandEvent& evt);
    void onIntradailyModulationChanged(wxCommandEvent& evt);
    void onIntermonthlyBreakdownChanged(wxCommandEvent& evt);
    void onReservoirCapacityChanged(wxCommandEvent& evt);
    void onLeewayLowBoundChanged(wxCommandEvent& evt);
    void onLeewayUpperBoundChanged(wxCommandEvent& evt);
    void onPumpingEfficiencyChanged(wxCommandEvent& evt);
    void onToggleReservoirManagement(Component::Button&, wxMenu& menu, void*);
    void onToggleFollowLoad(Component::Button&, wxMenu& menu, void*);
    void onToggleUseLeeway(Component::Button&, wxMenu& menu, void*);
    void onToggleUseWaterValue(Component::Button&, wxMenu& menu, void*);
    void onToggleHardBoundsOnRuleCurves(Component::Button&, wxMenu& menu, void*);
    void onToggleInitializeReservoirLevelDate(Component::Button&, wxMenu& menu, void*);
    void onToggleUseHeuristicTarget(Component::Button&, wxMenu& menu, void*);
    void onTogglePowerToLevel(Component::Button&, wxMenu& menu, void*);

    void onEnableReserveManagement(wxCommandEvent& evt);
    void onDisableReserveManagement(wxCommandEvent& evt);

    void onEnableUseLeeway(wxCommandEvent& evt);
    void onDisableUseLeeway(wxCommandEvent& evt);

    void onFollowingLoadModulations(wxCommandEvent& evt);
    void onUnfollowingLoadModulations(wxCommandEvent& evt);

    void onEnableUseWaterValue(wxCommandEvent& evt);
    void onDisableUseWaterValue(wxCommandEvent& evt);

    void onEnableHardBoundsOnRuleCurves(wxCommandEvent& evt);
    void onDisableHardBoundsOnRuleCurves(wxCommandEvent& evt);

    void onChangingInitializeReservoirLevelDate(wxCommandEvent& evt);

    void onEnableUseHeuristicTarget(wxCommandEvent& evt);
    void onDisableUseHeuristicTarget(wxCommandEvent& evt);

    void onEnablePowerToLevel(wxCommandEvent& evt);
    void onDisablePowerToLevel(wxCommandEvent& evt);

private:
    //! The input area selector
    Toolbox::InputSelector::Area* pInputAreaSelector;
    Data::Area* pArea = nullptr;
    bool pComponentsAreReady = false;
    Component::Panel* pSupport = nullptr;
    wxTextCtrl* pIntermonthlyBreakdown = nullptr;
    wxTextCtrl* pInterdailyBreakdown = nullptr;
    wxTextCtrl* pIntradailyModulation = nullptr;
    wxTextCtrl* pReservoirCapacity = nullptr;
    wxTextCtrl* pLeewayUpperBound = nullptr;
    wxTextCtrl* pLeewayLowerBound = nullptr;
    wxTextCtrl* pPumpingEfficiency = nullptr;
    wxWindow* pLabelReservoirCapacity = nullptr;
    wxWindow* pLabelUseWaterValues = nullptr;
    wxWindow* pLabelHardBounds = nullptr;
    wxWindow* pLabelUseHeuristicTarget = nullptr;
    wxWindow* pLabelLeewayLow = nullptr;
    wxWindow* pLabelLeewayUp = nullptr;
    wxWindow* pLabelUseLeeway = nullptr;
    wxWindow* pLabelPowerToLevel = nullptr;

    Component::Button* pReservoirManagement = nullptr;
    Component::Button* pFollowLoad = nullptr;
    Component::Button* pUseLeeway = nullptr;
    Component::Button* pUseWaterValue = nullptr;
    Component::Button* pHardBoundsOnRuleCurves = nullptr;
    Component::Button* pInitializeReservoirLevelDate = nullptr;
    Component::Button* pUseHeuristicTarget = nullptr;
    Component::Button* pPowerToLevel = nullptr;

}; // class Management

} // namespace Hydro
} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_HYDROOW_HYDRO_MANAGEMENT_H__
