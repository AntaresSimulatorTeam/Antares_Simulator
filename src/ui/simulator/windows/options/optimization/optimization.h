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
#ifndef __ANTARES_APPLICATION_WINDOWS_OPTIONS_OPTIMIZATION_PREFS_H__
#define __ANTARES_APPLICATION_WINDOWS_OPTIONS_OPTIMIZATION_PREFS_H__

#include <antares/wx-wrapper.h>
#include "../../../toolbox/components/button.h"
#include <wx/dialog.h>

#include <antares/study/UnfeasibleProblemBehavior.hpp>

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

    template<int>
    void onSelectTransportCapacity(wxCommandEvent&);

    void onSelectLinkTypeLocal(wxCommandEvent& evt);
    void onSelectLinkTypeAC(wxCommandEvent& evt);

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
    void onPopupMenuUnfeasibleBehavior(Component::Button&, wxMenu& menu, void*);
    void onPopupMenuAdequacyPatch(Component::Button&, wxMenu& menu, void*, const PopupInfo& info);

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
    Component::Button* pBtnSplitExportedMPS;
    Component::Button* pBtnAdequacyPatch;
    Component::Button* pBtnAdqPatchOutsideInside;
    Component::Button* pBtnAdqPatchOutsideOutside;
    Component::Button* pBtnUnfeasibleProblemBehavior;
    bool* pTargetRef;

}; // class Optimization

} // namespace Options
} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOWS_OPTIONS_OPTIMIZATION_PREFS_H__
