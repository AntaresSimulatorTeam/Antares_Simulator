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

#include "saveas.h"
#include <yuni/io/directory.h>

#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/dirdlg.h>
#include <wx/frame.h>

#include "../toolbox/validator.h"
#include "../toolbox/components/wizardheader.h"
#include <ui/common/component/panel.h>
#include "../toolbox/create.h"
#include "../application/study.h"
#include "../application/main.h"
#include "message.h"
#include <antares/resources/resources.h>

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace // anonymous
{
class JobSaveAs final : public Yuni::Job::IJob
{
public:
    explicit JobSaveAs(const String& path, bool output, bool userdata, bool logs) :
     pCopyOutput(output), pCopyUserData(userdata), pCopyLogs(logs)
    {
        IO::Normalize(pPath, path);
    }

    virtual ~JobSaveAs()
    {
    }

protected:
    virtual void onExecute() override
    {
        Forms::ApplWnd& mainFrm = *Forms::ApplWnd::Instance();
        mainFrm.saveStudyAs(pPath, pCopyOutput, pCopyUserData, pCopyLogs);
    }

private:
    String pPath;
    bool pCopyOutput;
    bool pCopyUserData;
    bool pCopyLogs;

}; // class JobSaveAs

#ifdef YUNI_OS_WINDOWS
enum
{
    borderSize = 2
};
#else
enum
{
    borderSize = 0
};
#endif

} // anonymous namespace

BEGIN_EVENT_TABLE(SaveAs, wxDialog)
EVT_COLLAPSIBLEPANE_CHANGED(mnIDCollapsePanel, SaveAs::onCollapsePanel)
EVT_TEXT(mnIDEditStudyName, SaveAs::onStudyNameChanged)
EVT_TEXT(mnIDEditStudyFolderName, SaveAs::onStudyFolderNameChanged)
END_EVENT_TABLE()

SaveResult SaveAs::Execute(wxFrame* parent, Data::Study::Ptr study)
{
    SaveAs dialog(parent, study);
    dialog.ShowModal();
    const SaveResult result = dialog.result();
    return result;
}

SaveAs::SaveAs(wxFrame* parent) :
 wxDialog(parent,
          wxID_ANY,
          wxT("Save As"),
          wxDefaultPosition,
          wxDefaultSize,
          wxCLOSE_BOX | wxCAPTION | wxFRAME_FLOAT_ON_PARENT | wxCLIP_CHILDREN),
 pStudy(Data::Study::Current::Get()),
 pGridSizer(nullptr),
 pParentProperties(nullptr),
 pStudyName(nullptr),
 pStudyFolderName(nullptr),
 pCustomFolderName(false),
 pCopyOutput(nullptr),
 pCopyUserData(nullptr),
 pCopyLogs(nullptr),
 pResult(svsCancel)
{
    assert(parent);
    internalCreateComponents();
}

SaveAs::SaveAs(wxFrame* parent, Data::Study::Ptr study) :
 wxDialog(parent,
          wxID_ANY,
          wxT("Save As"),
          wxDefaultPosition,
          wxDefaultSize,
          wxCLOSE_BOX | wxCAPTION | wxFRAME_FLOAT_ON_PARENT | wxCLIP_CHILDREN),
 pStudy(study),
 pGridSizer(nullptr),
 pParentProperties(nullptr),
 pStudyName(nullptr),
 pStudyFolderName(nullptr),
 pCustomFolderName(false),
 pCopyOutput(nullptr),
 pCopyUserData(nullptr),
 pCopyLogs(nullptr),
 pResult(svsCancel)
{
    assert(parent);
    internalCreateComponents();
}

SaveAs::~SaveAs()
{
    // To avoid corrupt vtable in some rare cases / misuses
    // (when children try to access to this object for example),
    // we should destroy all children as soon as possible.
    auto* sizer = GetSizer();
    if (sizer)
        sizer->Clear(true);
}

void SaveAs::propCaption(const wxString& text, bool bold)
{
    auto* t = new wxStaticText(pParentProperties,
                               wxID_ANY,
                               wxString() << wxT("    ") << text << wxT("  "),
                               wxDefaultPosition,
                               wxDefaultSize,
                               wxALIGN_RIGHT);
    if (bold)
    {
        t->SetFont(wxFont(t->GetFont().GetPointSize(),
                          wxFONTFAMILY_DEFAULT,
                          wxFONTSTYLE_NORMAL,
                          wxFONTWEIGHT_BOLD));
    }
    pGridSizer->Add(t, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
}

wxTextCtrl* SaveAs::propEdit(const wxString& defaultValue, int id)
{
    auto* edit = new wxTextCtrl(pParentProperties,
                                id,
                                defaultValue,
                                wxDefaultPosition,
                                wxDefaultSize,
                                0,
                                Toolbox::Validator::Default());
    pGridSizer->Add(edit, 1, wxALL | wxEXPAND, borderSize);
    return edit;
}

wxWindow* SaveAs::propEdit(wxWindow* control)
{
    pGridSizer->Add(control, 1, wxALL | wxEXPAND, borderSize);
    return control;
}

wxTextCtrl* SaveAs::propEdit(const wxString& defaultValue, const wxTextValidator& validator, int id)
{
    wxTextCtrl* edit = new wxTextCtrl(
      pParentProperties, id, defaultValue, wxDefaultPosition, wxDefaultSize, 0, validator);
    pGridSizer->Add(edit, 1, wxALL | wxEXPAND, borderSize);
    return edit;
}

void SaveAs::internalCreateComponents()
{
    pParentProperties = this;
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    // Header
    auto* header = Toolbox::Components::WizardHeader::Create(
      this, wxT("Save the Study As..."), "images/32x32/saveas.png", wxT(""));
    sizer->Add(header, 0, wxALL | wxEXPAND);
    sizer->SetItemMinSize(header, 550, header->GetSize().GetHeight());

    pGridSizer = new wxFlexGridSizer(2, 0, 0);
    pGridSizer->AddGrowableCol(1, 1);

    // Name of the study
    propCaption(wxT("Name of the study"), true);
    pStudyName = propEdit(wxEmptyString, mnIDEditStudyName);
    if (pStudy->header.caption.size())
        pStudyName->ChangeValue(wxStringFromUTF8(pStudy->header.caption));
    else
        pStudyName->ChangeValue(wxT("No Title"));

    // In the directory
    propCaption(wxT("In the directory"));
    {
        Component::Panel* pnl = new Component::Panel(pParentProperties);
        propEdit(pnl);
        wxBoxSizer* pnlSizer = new wxBoxSizer(wxHORIZONTAL);
        pnl->SetSizer(pnlSizer);
        //
        wxButton* btnBrowse = new wxButton(
          pnl, mnIDBrowse, wxT(" Browse "), wxDefaultPosition, wxSize(-1, 22), wxBU_EXACTFIT);
        pnlSizer->Add(btnBrowse);
        btnBrowse->Connect(btnBrowse->GetId(),
                           wxEVT_COMMAND_BUTTON_CLICKED,
                           wxCommandEventHandler(SaveAs::onBrowse),
                           nullptr,
                           this);

        pFolder = new wxTextCtrl(
          pnl, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
        if (not pStudy->folder.empty())
        {
            // Get the parent folder
            wxString s = wxStringFromUTF8(pStudy->folder);
            size_t p = s.find_last_of(wxT("\\/"));
            if (p != std::string::npos)
                pFolder->SetValue(s.substr(0, p));
            else
                pFolder->SetValue(s);
        }
        pnlSizer->AddSpacer(5);
        pnlSizer->Add(pFolder, 1, wxALL | wxEXPAND);
    }

    sizer->Add(pGridSizer, 0, wxALL | wxEXPAND, 20);

    //
    // More Options
    //
    auto* colPanl = new wxCollapsiblePane(pParentProperties, mnIDCollapsePanel, wxT("More"));
    pParentProperties = colPanl->GetPane();
    pGridSizer = new wxFlexGridSizer(2, 0, 0);
    pGridSizer->AddGrowableCol(1, 1);
    pParentProperties->SetSizer(pGridSizer);

    propCaption(wxT("With the folder's name"));
    pStudyFolderName = (wxTextCtrl*)propEdit(wxEmptyString, mnIDEditStudyFolderName);
    pStudyFolderName->ChangeValue(pStudyName->GetValue());

    if (not pStudy->folder.empty())
    {
        propCaption(wxEmptyString);
        pCopyOutput = (wxCheckBox*)propEdit(
          new wxCheckBox(pParentProperties, wxID_ANY, wxT("Copy the output folder")));
        pCopyOutput->SetValue(false);

        propCaption(wxEmptyString);
        pCopyUserData = (wxCheckBox*)propEdit(
          new wxCheckBox(pParentProperties, wxID_ANY, wxT("Copy the user data folder")));
        pCopyUserData->SetValue(true);

        propCaption(wxEmptyString);
        pCopyLogs = (wxCheckBox*)propEdit(
          new wxCheckBox(pParentProperties, wxID_ANY, wxT("Copy the log files")));
        pCopyLogs->SetValue(false);
    }

    // Space at the end of the collapsing panel
    pGridSizer->AddSpacer(10);
    pGridSizer->AddSpacer(10);

    auto* hz = new wxBoxSizer(wxHORIZONTAL);
    hz->Add(20, 1);
    hz->Add(colPanl, 1, wxALL | wxEXPAND);
    hz->Add(20, 1);
    sizer->Add(hz, 0, wxALL | wxEXPAND);
    sizer->AddSpacer(50);

    pParentProperties = this;

    // Buttons
    auto* pPnlButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
    pPnlButtonsSizer->AddStretchSpacer();

    // Save
    auto* btnSaveAs = Component::CreateButton(this, wxT("Save the study"), this, &SaveAs::onSave);
    pPnlButtonsSizer->Add(btnSaveAs, 0, wxALL | wxEXPAND);
    pPnlButtonsSizer->AddSpacer(3);

    // Cancel
    auto* btnCancel = Component::CreateButton(this, wxT("Cancel"), this, &SaveAs::onCancel);
    pPnlButtonsSizer->Add(btnCancel, 0, wxALL | wxEXPAND);
    pPnlButtonsSizer->AddSpacer(20);

    sizer->Add(pPnlButtonsSizer, 0, wxALL | wxEXPAND);

    sizer->AddSpacer(10);
    sizer->Layout();

    btnSaveAs->SetDefault();

    // Recentre the window
    SetSizer(sizer);
    sizer->Fit(this);
    Centre(wxBOTH);
}

void SaveAs::onCollapsePanel(wxCollapsiblePaneEvent&)
{
    // Do nothing
}

void SaveAs::onStudyNameChanged(wxCommandEvent&)
{
    if (not pCustomFolderName)
    {
        wxString s = pStudyName->GetValue();
        s.Trim(false);
        s.Trim(true);
        pStudyFolderName->ChangeValue(s);
    }
}

void SaveAs::onStudyFolderNameChanged(wxCommandEvent&)
{
    pCustomFolderName = true;
}

void SaveAs::onBrowse(wxCommandEvent&)
{
    wxDirDialog dialog(
      this, _T("Browse"), pFolder->GetValue(), wxDD_DEFAULT_STYLE & ~wxDD_DIR_MUST_EXIST);
    if (dialog.ShowModal() == wxID_OK)
        pFolder->SetValue(dialog.GetPath());
}

void SaveAs::onCancel(void*)
{
    pResult = svsCancel;
    Dispatcher::GUI::Close(this);
}

void SaveAs::onSave(void*)
{
    // Informations about the target folder
    String wantedPath;
    wxStringToString(pFolder->GetValue(), wantedPath);
    String name;
    wxStringToString(pStudyName->GetValue(), name);
    String folderName;
    wxStringToString(pStudyFolderName->GetValue(), folderName);

    // Trimming
    name.trim();
    wantedPath.trim();
    folderName.trim();

    // The user data and the output can only be copied if it is not
    // a fresh study
    bool srcFolderNotEmpty = not pStudy->folder.empty();
    bool copyUserData = srcFolderNotEmpty and pCopyUserData and pCopyUserData->GetValue();
    bool copyOutput = srcFolderNotEmpty and pCopyOutput and pCopyOutput->GetValue();
    bool copyLogs = srcFolderNotEmpty and pCopyLogs and pCopyLogs->GetValue();

    // Checks
    if (name.empty())
    {
        logs.error() << "The name of the study must not be empty";
        return;
    }
    if (folderName.empty())
    {
        logs.error() << "The folder's name of the study must not be empty";
        return;
    }
    if (wantedPath.empty())
    {
        logs.error() << "Please select a folder";
        return;
    }
    if (not IO::Directory::Exists(wantedPath))
    {
        logs.error() << "The selected path does not exist : " << wantedPath;
        return;
    }

    // Write the study
    wantedPath << IO::Separator << folderName;
    String path;
    IO::Normalize(path, wantedPath);

    // reference to the main form
    auto& mainFrm = *Forms::ApplWnd::Instance();

    // Checking for the root folder
    if (System::windows)
    {
        String rootFolder;
        Resources::GetRootFolder(rootFolder);
        rootFolder.toLower();
        rootFolder << IO::Separator;

        // On a clean windows installation, we are in a `bin` folder
        if (rootFolder.endsWith("\\bin\\"))
        {
            rootFolder += "..\\";
            String tmp;
            IO::Normalize(tmp, rootFolder);
            rootFolder = tmp;
            rootFolder << IO::Separator;
        }

        String pathCopy = path;
        pathCopy.toLower();

        if (pathCopy.startsWith(rootFolder))
        {
            Window::Message message(&mainFrm,
                                    wxT("Save As"),
                                    wxT("Impossible to save the study at this location"),
                                    wxString()
                                      << wxT("Please choose another folder to save the study."));
            message.add(Window::Message::btnCancel);
            message.showModal();
            return;
        }
    }

    if (not pStudy->checkForFilenameLimits(false, path))
    {
        // Errors will be displayed by the logs
        return;
    }

    // Checking if the path is not contained into another study...
    if (CheckIfInsideAStudyFolder(path, true))
    {
        bool canOverwrite = false;

        // Checking if the path is the root folder of a study
        // In this case, the user will be asked for permission to overwrite
        // the existing study
        String title;
        if (Data::Study::TitleFromStudyFolder(path, title))
        {
            // \p path is really a study. Can we overwrite it ?
            Window::Message message(
              &mainFrm,
              wxT("Save As"),
              wxT("Impossible to save the study"),
              wxString() << wxT(
                "The selected folder is an existing study. Do you really want to overwrite it ?")
                         << wxT("\n\ntitle : ") << wxStringFromUTF8(title) << wxT("\nlocation : ")
                         << wxStringFromUTF8(path));
            message.add(Window::Message::btnContinue);
            message.add(Window::Message::btnCancel);
            if (message.showModal() == Window::Message::btnContinue)
            {
                canOverwrite = true;
            }
            else
                return;
        }

        if (not canOverwrite)
        {
            Window::Message message(
              &mainFrm,
              wxT("Save As"),
              wxT("Impossible to save the study"),
              wxT("The selected folder is part of an existing study. It can not be overwritten."));
            message.add(Window::Message::btnCancel);
            message.showModal();
            return;
        }
    }
    else
    {
        if (IO::Exists(path))
        {
            Window::Message message(&mainFrm,
                                    wxT("Save As"),
                                    wxT("Impossible to save the study"),
                                    wxString()
                                      << wxT("A file or a folder named '") << wxStringFromUTF8(path)
                                      << wxT("' already exists"));
            message.add(Window::Message::btnCancel);
            message.showModal();
            return;
        }
    }

    // Set the new study name
    pStudy->header.caption = name;
    // Remove the read-only mode
    pStudy->parameters.readonly = false;

    // Closing the Window
    pResult = svsSaved;
    this->Close();

    mainFrm.saveStudyAs(path, copyOutput, copyUserData, copyLogs);
}

} // namespace Window
} // namespace Antares
