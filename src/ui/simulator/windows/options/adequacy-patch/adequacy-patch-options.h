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
#ifndef __ANTARES_APPLICATION_WINDOWS_OPTIONS_ADEQUACY_PATCH_H__
#define __ANTARES_APPLICATION_WINDOWS_OPTIONS_ADEQUACY_PATCH_H__

#include "../../../toolbox/components/button.h"
#include <wx/dialog.h>

#include <antares/study/UnfeasibleProblemBehavior.hpp>

namespace Antares::Window::Options
{
/*!
** \brief Startup Wizard User Interface
*/
class AdequacyPatchOptions final : public wxDialog
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    **
    ** \param parent The parent window
    */
    explicit AdequacyPatchOptions(wxWindow* parent);
    //! Destructor
    ~AdequacyPatchOptions() override;
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

    void refresh();
    void onClose(const void*);
    void onResetToDefault(void*);
    wxTextCtrl* insertEdit(wxWindow* parent,
                           wxSizer* sizer,
                           const wxString& text,
                           wxObjectEventFunction);

    void onSelectPtoIsDens(wxCommandEvent&);
    void onSelectPtoIsLoad(wxCommandEvent&);
    void onSelectModeInclude(wxCommandEvent& evt);
    void onSelectModeIgnore(wxCommandEvent& evt);

    void onPopupMenu(Component::Button&, wxMenu& menu, void*, const PopupInfo& info);
    void onPopupMenuSpecify(Component::Button&, wxMenu& menu, void*, const PopupInfo& info);
    void onPopupMenuNTC(Component::Button&, wxMenu& menu, void*, const PopupInfo& info);
    void onPopupMenuPTO(Component::Button&, wxMenu& menu, void*);

    void onInternalMotion(wxMouseEvent&);  
    void onEditThresholds(wxCommandEvent&);

    Component::Button* pBtnAdequacyPatch;
    Component::Button* pBtnNTCfromOutToInAdqPatch;
    Component::Button* pBtnNTCfromOutToOutAdqPatch;
    Component::Button* pBtnAdequacyPatchPTO;
    Component::Button* pBtnAdequacyPatchIncludeHurdleCostCsr;
    Component::Button* pBtnAdequacyPatchCheckCsrCostFunctionValue;
    wxTextCtrl* pThresholdCSRStart;
    wxTextCtrl* pThresholdLMRviolations;
    wxTextCtrl* pThresholdCSRVarBoundsRelaxation;
    bool* pTargetRef;

}; // class AdequacyPatchOptions

}

#endif // __ANTARES_APPLICATION_WINDOWS_OPTIONS_ADEQUACY_PATCH_H__
