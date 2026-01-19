// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
