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

#include "notes.h"
#include <yuni/io/file.h>
#include <yuni/io/directory/system.h>
#include <yuni/uuid/uuid.h>
#include "../toolbox/resources.h"
#include "../toolbox/components/button.h"
#include "../toolbox/create.h"
#include <wx/richtext/richtextctrl.h>
#include <wx/richtext/richtextstyles.h>
#include <wx/richtext/richtextxml.h>
#include <wx/richtext/richtextxml.h>
#include <wx/richtext/richtextformatdlg.h>
#include <wx/richtext/richtextsymboldlg.h>
#include <wx/richtext/richtextstyledlg.h>
#include <wx/richtext/richtextprint.h>
#include <wx/statline.h>
#include <antares/study.h>
#include "../application/study.h"

using namespace Yuni;

namespace Antares
{
namespace Window
{
Yuni::Event<void(Notes*)> Notes::OnChanged;

static uint pGlobalRevision = 0;
static bool pLocked = false;

BEGIN_EVENT_TABLE(Notes, Antares::Component::Panel)
EVT_RICHTEXT_CHARACTER(wxID_ANY, Notes::onUserNotesCharacter)
EVT_RICHTEXT_STYLE_CHANGED(wxID_ANY, Notes::onUserNotesStyleChanged)
EVT_RICHTEXT_CONTENT_INSERTED(wxID_ANY, Notes::onUserNotesStyleChanged)
EVT_RICHTEXT_CONTENT_DELETED(wxID_ANY, Notes::onUserNotesStyleChanged)
EVT_RICHTEXT_BUFFER_RESET(wxID_ANY, Notes::onUserNotesStyleChanged)
END_EVENT_TABLE()

Notes::Notes(wxWindow* parent, uint margin) :
 Antares::Component::Panel(parent),
 pRichEdit(nullptr),
 pStyleSheet(nullptr),
 pLocalRevision(0),
 pUpdatesToSkip(0)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* ssz = new wxBoxSizer(wxHORIZONTAL);
    ssz->AddSpacer(margin);

    Component::Button* btn;

    btn = new Component::Button(this, wxEmptyString, "images/16x16/bold.png", this, &Notes::onBold);
    ssz->Add(btn);
    btn = new Component::Button(
      this, wxEmptyString, "images/16x16/italic.png", this, &Notes::onItalic);
    ssz->Add(btn);
    btn = new Component::Button(
      this, wxEmptyString, "images/16x16/underline.png", this, &Notes::onUnderline);
    ssz->Add(btn);

    Antares::Component::AddVerticalSeparator(this, ssz);

    btn = new Component::Button(
      this, wxEmptyString, "images/16x16/alignleft.png", this, &Notes::onAlignLeft);
    ssz->Add(btn);
    btn = new Component::Button(
      this, wxEmptyString, "images/16x16/aligncentre.png", this, &Notes::onAlignCenter);
    ssz->Add(btn);
    btn = new Component::Button(
      this, wxEmptyString, "images/16x16/alignright.png", this, &Notes::onAlignRight);
    ssz->Add(btn);

    Antares::Component::AddVerticalSeparator(this, ssz);

    btn = new Component::Button(
      this, wxEmptyString, "images/16x16/list_bullet.png", this, &Notes::onListBullet);
    ssz->Add(btn);
    btn = new Component::Button(
      this, wxEmptyString, "images/16x16/list_numbered.png", this, &Notes::onListNumbered);
    ssz->Add(btn);

    Antares::Component::AddVerticalSeparator(this, ssz);

    btn = new Component::Button(this,
                                wxEmptyString,
                                "images/16x16/format_decreaseindent.png",
                                this,
                                &Notes::onIndentDecrease);
    ssz->Add(btn);
    btn = new Component::Button(this,
                                wxEmptyString,
                                "images/16x16/format_increaseindent.png",
                                this,
                                &Notes::onIndentIncrease);
    ssz->Add(btn);

    sizer->Add(ssz, 0, wxALL | wxEXPAND, 2);

    // Rich Edit
    wxBoxSizer* richSizer = new wxBoxSizer(wxHORIZONTAL);
    pRichEdit = new wxRichTextCtrl(this,
                                   wxID_ANY,
                                   wxEmptyString,
                                   wxDefaultPosition,
                                   wxSize(200, 200),
                                   wxVSCROLL | wxHSCROLL | wxNO_BORDER | wxWANTS_CHARS);
    wxPanel* p = new Antares::Component::Panel(this);
    p->SetSize(margin, 10);
    p->SetBackgroundColour(pRichEdit->GetBackgroundColour());
    richSizer->Add(p, 0, wxALL | wxEXPAND);
    richSizer->Add(pRichEdit, 1, wxALL | wxEXPAND);
    sizer->Add(richSizer, 1, wxALL | wxEXPAND);

    // Sizer
    SetSizer(sizer);

    appendStyles();

    OnChanged.connect(this, &Notes::onNotesModified);
    OnStudyClosed.connect(this, &Notes::onStudyClosed);
}

Notes::~Notes()
{
    // Remove the temporary file
    if (not pTempFile.empty() and not IO::File::Delete(pTempFile))
    {
        if (IO::File::Exists(pTempFile))
            logs.warning() << "I/O error: impossible to delete " << pTempFile;
        pTempFile.clear();
    }

    destroyBoundEvents();
    delete pStyleSheet;
}

void Notes::onStudyClosed()
{
    // Resetting the internal pseudo revision numbers
    pGlobalRevision = 0;
    pLocalRevision = 0;
}

void Notes::onUserNotesCharacter(wxRichTextEvent&)
{
    notifyChanges();
}

void Notes::onUserNotesStyleChanged(wxRichTextEvent&)
{
    notifyChanges();
}

void Notes::notifyChanges()
{
    if (Data::Study::Current::Valid() and not pLocked)
    {
        saveToStudy();
        MarkTheStudyAsModified();
        ++pUpdatesToSkip;
        OnChanged(this);
    }
}

void Notes::onNotesModified(Notes* sender)
{
    assert(pRichEdit);

    if (sender != this and pGlobalRevision != pLocalRevision and not pLocked)
    {
        if (pUpdatesToSkip)
        {
            if (0 != (--pUpdatesToSkip))
                return;
        }
        // Reloading the user notes
        loadFromStudy();
        // Updating our local revision number
        pLocalRevision = pGlobalRevision;
    }
}

void Notes::appendStyles()
{
    pStyleSheet = new wxRichTextStyleSheet;
    // Bullet
    auto* bulletList = new wxRichTextListStyleDefinition(wxT("Bullet List"));
    wxString bulletText;
    for (int i = 0; i < 10; ++i)
    {
        bulletText.Clear();
        if (i == 0)
            bulletText = wxT("standard/circle");
        else if (i == 1)
            bulletText = wxT("standard/square");
        else if (i == 2)
            bulletText = wxT("standard/circle");
        else if (i == 3)
            bulletText = wxT("standard/square");
        else
            bulletText = wxT("standard/circle");

        bulletList->SetAttributes(
          i, (i + 1) * 60, 60, wxTEXT_ATTR_BULLET_STYLE_STANDARD, bulletText);
    }
    pStyleSheet->AddListStyle(bulletList);

    auto* numberedList = new wxRichTextListStyleDefinition(wxT("Numbered List"));
    for (int i = 0; i < 10; ++i)
    {
        long numberStyle;
        if (i == 0)
            numberStyle = wxTEXT_ATTR_BULLET_STYLE_ARABIC | wxTEXT_ATTR_BULLET_STYLE_PERIOD;
        else if (i == 1)
            numberStyle
              = wxTEXT_ATTR_BULLET_STYLE_LETTERS_LOWER | wxTEXT_ATTR_BULLET_STYLE_PARENTHESES;
        else if (i == 2)
            numberStyle
              = wxTEXT_ATTR_BULLET_STYLE_ROMAN_LOWER | wxTEXT_ATTR_BULLET_STYLE_PARENTHESES;
        else if (i == 3)
            numberStyle
              = wxTEXT_ATTR_BULLET_STYLE_ROMAN_UPPER | wxTEXT_ATTR_BULLET_STYLE_PARENTHESES;
        else
            numberStyle = wxTEXT_ATTR_BULLET_STYLE_ARABIC | wxTEXT_ATTR_BULLET_STYLE_PERIOD;

        numberStyle |= wxTEXT_ATTR_BULLET_STYLE_ALIGN_RIGHT;
        numberedList->SetAttributes(i, (i + 1) * 60, 60, numberStyle);
    }
    pStyleSheet->AddListStyle(numberedList);

    pRichEdit->SetStyleSheet(pStyleSheet);
}

void Notes::onBold(void*)
{
    assert(pRichEdit);
    pRichEdit->ApplyBoldToSelection();
}

void Notes::onItalic(void*)
{
    assert(pRichEdit);
    pRichEdit->ApplyItalicToSelection();
}

void Notes::onUnderline(void*)
{
    assert(pRichEdit);
    pRichEdit->ApplyUnderlineToSelection();
}

void Notes::onAlignLeft(void*)
{
    assert(pRichEdit);
    pRichEdit->ApplyAlignmentToSelection(wxTEXT_ALIGNMENT_LEFT);
}

void Notes::onAlignRight(void*)
{
    assert(pRichEdit);
    pRichEdit->ApplyAlignmentToSelection(wxTEXT_ALIGNMENT_RIGHT);
}

void Notes::onAlignCenter(void*)
{
    assert(pRichEdit);
    pRichEdit->ApplyAlignmentToSelection(wxTEXT_ALIGNMENT_CENTRE);
}

void Notes::onListBullet(void*)
{
    assert(pRichEdit);

    if (pRichEdit->HasSelection())
    {
        wxRichTextRange range = pRichEdit->GetSelectionRange();
        pRichEdit->SetListStyle(
          range, wxT("Bullet List"), wxRICHTEXT_SETSTYLE_WITH_UNDO | wxRICHTEXT_SETSTYLE_RENUMBER);
    }
}

void Notes::onListNumbered(void*)
{
    assert(pRichEdit);
    if (pRichEdit->HasSelection())
    {
        wxRichTextRange range = pRichEdit->GetSelectionRange();
        pRichEdit->SetListStyle(range,
                                wxT("Numbered List"),
                                wxRICHTEXT_SETSTYLE_WITH_UNDO | wxRICHTEXT_SETSTYLE_RENUMBER);
    }
}

void Notes::onIndentIncrease(void*)
{
    assert(pRichEdit);

    wxTextAttrEx attr;
    attr.SetFlags(wxTEXT_ATTR_LEFT_INDENT);

    if (pRichEdit->GetStyle(pRichEdit->GetInsertionPoint(), attr))
    {
        wxRichTextRange range(pRichEdit->GetInsertionPoint(), pRichEdit->GetInsertionPoint());
        if (pRichEdit->HasSelection())
            range = pRichEdit->GetSelectionRange();

        attr.SetLeftIndent(attr.GetLeftIndent() + 100);

        attr.SetFlags(wxTEXT_ATTR_LEFT_INDENT);
        pRichEdit->SetStyle(range, attr);
    }
}

void Notes::onIndentDecrease(void*)
{
    assert(pRichEdit);

    wxTextAttrEx attr;
    attr.SetFlags(wxTEXT_ATTR_LEFT_INDENT);

    if (pRichEdit->GetStyle(pRichEdit->GetInsertionPoint(), attr))
    {
        wxRichTextRange range(pRichEdit->GetInsertionPoint(), pRichEdit->GetInsertionPoint());
        if (pRichEdit->HasSelection())
            range = pRichEdit->GetSelectionRange();

        if (attr.GetLeftIndent() > 0)
        {
            attr.SetLeftIndent(wxMax(0, attr.GetLeftIndent() - 100));
            pRichEdit->SetStyle(range, attr);
        }
    }
}

void Notes::saveToStudy()
{
    auto study = Data::Study::Current::Get();
    if (!pRichEdit || !study)
        return;

    if (!pTempFile)
    {
        if (not initializeTemporaryFile())
        {
            logs.error() << "impossible to write temporary file. The user's notes won't be saved.";
            return;
        }
    }

    // Incrementing the pseudo revision
    pLocalRevision = ++pGlobalRevision;
    // Saving the content into an XML file...
    pRichEdit->SaveFile(wxStringFromUTF8(pTempFile), wxRICHTEXT_TYPE_XML);
    // ... and reloading it
    IO::File::LoadFromFile(study->simulationMetadata.comments, pTempFile);
}

void Notes::connectToNotification()
{
    OnChanged.remove(this);
    OnChanged.connect(this, &Notes::onNotesModified);
}

void Notes::disconnectFromNotification()
{
    OnChanged.remove(this);
}

void Notes::loadFromStudy()
{
    if (!pRichEdit)
        return;

    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    if (not pTempFile)
    {
        if (not initializeTemporaryFile())
        {
            logs.error() << "impossible to write temporary file. The user's notes won't be loaded";
            return;
        }
    }

    if (not pLocked)
    {
        pLocked = true;
        if (study.simulationMetadata.comments.empty())
        {
            pRichEdit->Clear();
        }
        else
        {
            // <?xml ?
            if (study.simulationMetadata.comments.startsWith("<?xml "))
            {
                // XML Version
                // It seems that the file is a XML one. We have to use the
                // method LoadFile(), which does not support raw buffer.
                // Consequently, we will have to write the content into a
                // temporary file and to load it...

                // logs.info() << " notes: exporting to " << s;
                if (IO::File::SetContent(pTempFile, study.simulationMetadata.comments))
                {
                    if (!pRichEdit->LoadFile(wxStringFromUTF8(pTempFile), wxRICHTEXT_TYPE_XML))
                        logs.error() << "Impossible to load the comments file";

                    // cleanup
                    IO::File::Delete(pTempFile);
                }
                else
                    logs.error() << "Impossible to write '" << pTempFile << "'";
            }
            else
            {
                // Txt version
                pRichEdit->SetValue(wxStringFromUTF8(study.simulationMetadata.comments));
            }
        }
        pLocked = false;
    }
}

bool Notes::initializeTemporaryFile()
{
    IO::Directory::System::Temporary(pTempFile);
    pTempFile << IO::Separator << "antares-comments-";
    UUID uuid;
    uuid.generate();
    pTempFile << uuid << ".xml";

    if (!IO::File::CreateEmptyFile(pTempFile))
    {
        pTempFile.clear();
        return false;
    }
    return true;
}

} // namespace Window
} // namespace Antares
