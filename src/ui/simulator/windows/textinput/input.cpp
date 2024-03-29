/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <antares/study/constraint.h>
#include "../../application/study.h"
#include "../../application/main/main.h"
#include "../../toolbox/components/wizardheader.h"
#include "../../toolbox/create.h"

#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/window.h>
#include <wx/stattext.h>

using namespace Yuni;

namespace Antares::Window
{
namespace // anonymous
{
class ValueDialog final : public wxDialog
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    ValueDialog(wxWindow* parent,
                const wxString& out,
                const wxString& title,
                const wxString& caption);
    //! Destructor
    virtual ~ValueDialog()
    {
    }
    //@}

public:
    bool canceled;
    wxString value;

private:
    void onCancel(void*);
    void onPerform(void*);

private:
    wxFlexGridSizer* pGridSizer;
    wxTextCtrl* pTextInput;

}; // class ValueDialog

//! The previous value entered by the user (persistence)
static wxString gPreviousValue;

ValueDialog::ValueDialog(wxWindow* parent,
                         const wxString& out,
                         const wxString& title,
                         const wxString& caption) :
 wxDialog(parent,
          wxID_ANY,
          title,
          wxDefaultPosition,
          wxDefaultSize,
          wxCLOSE_BOX | wxCAPTION | wxSYSTEM_MENU | wxCLIP_CHILDREN)
{
    canceled = true;

    auto* sizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add(Toolbox::Components::WizardHeader::Create(
                 this, title, "images/32x32/resize-matrix.png", wxEmptyString, 200),
               0,
               wxALL | wxEXPAND);

    sizer->AddSpacer(10);

    pGridSizer = new wxFlexGridSizer(2, 0, 0);
    pGridSizer->AddGrowableCol(1);

    {
        wxStaticText* t = Component::CreateLabel(this, caption, true);
        wxSizer* hz = new wxBoxSizer(wxHORIZONTAL);
        hz->Add(t, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        hz->AddSpacer(5);
        pGridSizer->Add(hz, 0, wxALL | wxEXPAND);

        wxString s;
        if (gPreviousValue.empty() || !out.empty())
            s << out;
        else
            s << gPreviousValue;
        pTextInput = new wxTextCtrl(this, wxID_ANY, s, wxDefaultPosition, wxDefaultSize);
        pGridSizer->Add(pTextInput, 1, wxALL | wxEXPAND, 1);
    }

    sizer->Add(pGridSizer, 1, wxALL | wxEXPAND, 20);

    sizer->AddSpacer(25);

    wxBoxSizer* pnlBtns = new wxBoxSizer(wxHORIZONTAL);
    pnlBtns->AddStretchSpacer();

    // Cancel
    auto* pBtnCancel = Component::CreateButton(this, wxT("Cancel"), this, &ValueDialog::onCancel);
    pnlBtns->Add(pBtnCancel);
    pnlBtns->AddSpacer(5);

    // Gogogo !
    auto* pBtnSave = Component::CreateButton(this, wxT("Continue"), this, &ValueDialog::onPerform);
    pnlBtns->Add(pBtnSave);
    pBtnSave->SetDefault();

    pnlBtns->AddSpacer(20);
    sizer->Add(pnlBtns, 0, wxALL | wxEXPAND);
    sizer->AddSpacer(10);

    sizer->Layout();

    SetSizer(sizer);

    GetSizer()->Fit(this);
    wxSize p = GetSize();
    p.SetWidth(380);
    SetSize(p);
    Centre(wxBOTH);

    pTextInput->SetSelection(-1, -1);
    pTextInput->SetFocus();
}

void ValueDialog::onCancel(void*)
{
    value.clear();
    Dispatcher::GUI::Close(this);
}

void ValueDialog::onPerform(void*)
{
    assert(pTextInput);

    value = pTextInput->GetValue();
    gPreviousValue = value;
    canceled = false;
    Dispatcher::GUI::Close(this);
}

} // anonymous namespace

bool AskForInput(wxString& out, const wxString& title, const wxString& caption)
{
    ValueDialog dlg(Antares::Forms::ApplWnd::Instance(), out, title, caption);
    dlg.ShowModal();
    if (not dlg.canceled)
    {
        out = dlg.value;
        return true;
    }
    return false;
}

} // namespace Antares::Window

