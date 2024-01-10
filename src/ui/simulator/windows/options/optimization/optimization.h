/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
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
#ifndef __ANTARES_APPLICATION_WINDOWS_OPTIONS_OPTIMIZATION_PREFS_H__
#define __ANTARES_APPLICATION_WINDOWS_OPTIONS_OPTIMIZATION_PREFS_H__

#include "toolbox/components/button.h"
#include <wx/dialog.h>

#include <antares/study/UnfeasibleProblemBehavior.hpp>
#include "application/menus.h"

namespace Antares
{
namespace Window
{
namespace Options
{
/*!
** \brief Startup Wizard User Interface
*/
class Optimization final : public wxDialog
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    **
    ** \param parent The parent window
    */
    Optimization(wxWindow* parent);
    //! Destructor
    virtual ~Optimization();
    //@}

private:
    class PopupInfo final
    {
    public:
        PopupInfo(bool& r, const wxChar* const t) : rval(r), text(t)
        {
        }
        bool& rval;
        const wxChar* const text;
    };

private:
    void refresh();
    void onClose(void*);
    void onResetToDefault(void*);

    void onSelectModeInclude(wxCommandEvent& evt);
    void onSelectModeIgnore(wxCommandEvent& evt);
    void onSelectSimplexDay(wxCommandEvent& evt);
    void onSelectSimplexWeek(wxCommandEvent& evt);

    void setTransmissionCapacity(Data::GlobalTransmissionCapacities newCapacity);
    template<Data::GlobalTransmissionCapacities>
    void onSelectTransmissionCapacity(wxCommandEvent&);

    template<Data::GlobalTransmissionCapacities>
    void createGlobalTransmissionCapacitiesItemIntoMenu(wxMenu& menu);

    void onSelectLinkTypeLocal(wxCommandEvent& evt);
    void onSelectLinkTypeAC(wxCommandEvent& evt);

    // Export MPS functions
    void onSelectExportMPS(const Data::mpsExportStatus& mps_export_status);

    template<Data::mpsExportStatus>
    void onSelectExportMPS(wxCommandEvent&);

    template<Data::mpsExportStatus>
    void createMPSexportItemIntoMenu(wxMenu& menu);

    // Unfeasible behavior problem functions
    void onSelectUnfeasibleBehaviorWarningDry(wxCommandEvent& evt);
    void onSelectUnfeasibleBehaviorWarningMps(wxCommandEvent& evt);
    void onSelectUnfeasibleBehaviorErrorDry(wxCommandEvent& evt);
    void onSelectUnfeasibleBehaviorErrorMps(wxCommandEvent& evt);
    void onSelectUnfeasibleBehavior(
      const Data::UnfeasibleProblemBehavior& unfeasibleProblemBehavior);

    void onPopupMenu(Component::Button&, wxMenu& menu, void*, const PopupInfo& info);
    void onPopupMenuSimplex(Component::Button&, wxMenu& menu, void*);
    void onPopupMenuSpecify(Component::Button&, wxMenu& menu, void*, const PopupInfo& info);
    void onPopupMenuTransmissionCapacities(Component::Button&, wxMenu& menu, void*);
    void onPopupMenuLinkType(Component::Button&, wxMenu& menu, void*);
    void onPopupMenuExportMPSstatus(Component::Button&, wxMenu& menu, void*);
    void onPopupMenuUnfeasibleBehavior(Component::Button&, wxMenu& menu, void*);
    
    void onInternalMotion(wxMouseEvent&);

private:
    Component::Button* pBtnConstraints;
    Component::Button* pBtnHurdleCosts;
    Component::Button* pBtnTransmissionCapacities;
    Component::Button* pBtnLinkType;
    Component::Button* pBtnThermalClusterMinStablePower;
    Component::Button* pBtnThermalClusterMinUPTime;
    Component::Button* pBtnDayAheadReserve;
    Component::Button* pBtnStrategicReserve;
    Component::Button* pBtnPrimaryReserve;
    Component::Button* pBtnSpinningReserve;
    Component::Button* pBtnSimplexOptimizationRange;

    Component::Button* pBtnExportMPS;
    Component::Button* pBtnUnfeasibleProblemBehavior;
    bool* pTargetRef;

}; // class Optimization

const char* mpsExportIcon(const Data::mpsExportStatus& mps_export_status);

template<Data::mpsExportStatus MPS_EXPORT_STATUS>
void Optimization::onSelectExportMPS(wxCommandEvent&)
{
    Optimization::onSelectExportMPS(MPS_EXPORT_STATUS);
}

template<Data::mpsExportStatus MPS_EXPORT_STATUS>
void Optimization::createMPSexportItemIntoMenu(wxMenu& menu)
{
    const wxMenuItem* it = Menu::CreateItem(&menu,
                                            wxID_ANY,
                                            mpsExportStatusToString(MPS_EXPORT_STATUS),
                                            mpsExportIcon(MPS_EXPORT_STATUS),
                                            wxEmptyString);

    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Optimization::onSelectExportMPS<MPS_EXPORT_STATUS>),
                 nullptr,
                 this);
}

const char* transmissionCapacityIcon(Data::GlobalTransmissionCapacities capacity);

template<Data::GlobalTransmissionCapacities CAPACITY>
void Optimization::createGlobalTransmissionCapacitiesItemIntoMenu(wxMenu& menu)
{
    const wxMenuItem* it = Menu::CreateItem(&menu,
                                            wxID_ANY,
                                            GlobalTransmissionCapacitiesToString_Display(CAPACITY),
                                            transmissionCapacityIcon(CAPACITY),
                                            wxEmptyString);

    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Optimization::onSelectTransmissionCapacity<CAPACITY>),
                 nullptr,
                 this);
}

} // namespace Options
} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOWS_OPTIONS_OPTIMIZATION_PREFS_H__
