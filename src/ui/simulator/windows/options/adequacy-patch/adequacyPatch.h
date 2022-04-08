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
#ifndef __ANTARES_APPLICATION_WINDOWS_OPTIONS_ADEQUACY_PATCH_H__
#define __ANTARES_APPLICATION_WINDOWS_OPTIONS_ADEQUACY_PATCH_H__

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
    AdequacyPatchOptions(wxWindow* parent);
    //! Destructor
    virtual ~AdequacyPatchOptions();
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
    void onEditSeedTSDraws(wxCommandEvent&);
private:
    Component::Button* pBtnAdequacyPatch;
    Component::Button* pBtnAdequacyPatchNTC12;
    Component::Button* pBtnAdequacyPatchNTC11;
    Component::Button* pBtnAdequacyPatchPTO;
    Component::Button* pBtnAdequacyPatchSaveIntermediateResults;
    wxTextCtrl* pEditSeeds[Data::AdequacyPatch::seedThresholdMax];
    bool* pTargetRef;

}; // class AdequacyPatchOptions

} // namespace Options
} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOWS_OPTIONS_ADEQUACY_PATCH_H__
