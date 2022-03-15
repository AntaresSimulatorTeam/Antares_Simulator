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

#include "exportmap.h"
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
#include "../toolbox/components/map/settings.h"

using namespace Yuni;

static const char* formatNames[Antares::Map::mfFormatCount] = {"PNG", "JPEG", "SVG"};
static const char* formatExt[Antares::Map::mfFormatCount] = {".png", ".jpg", ".svg"};

namespace Antares
{
namespace Window
{
namespace // anonymous
{
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

#define SEP IO::Separator

BEGIN_EVENT_TABLE(ExportMap, wxDialog)
EVT_COLLAPSIBLEPANE_CHANGED(mnIDCollapsePanel, ExportMap::onCollapsePanel)
EVT_TEXT(mnIDEditMapName, ExportMap::onMapNameChanged)
EVT_TEXT(mnIDEditMapFolderName, ExportMap::onMapFolderNameChanged)
END_EVENT_TABLE()

wxString ExportMap::defaultPath = wxEmptyString;

SaveResult ExportMap::Execute(wxFrame* parent, Data::Study::Ptr study)
{
    ExportMap dialog(parent, study);
    dialog.ShowModal();
    const SaveResult result = dialog.result();
    return result;
}

ExportMap::ExportMap(wxFrame* parent) :
 wxDialog(parent,
          wxID_ANY,
          wxT("Export Map"),
          wxDefaultPosition,
          wxDefaultSize,
          wxCLOSE_BOX | wxCAPTION | wxFRAME_FLOAT_ON_PARENT | wxCLIP_CHILDREN),
 pStudy(Data::Study::Current::Get()),
 pGridSizer(nullptr),
 pParentProperties(nullptr),
 pMapName(nullptr),
 pMapFolderName(nullptr),
 pMapFolderPath(nullptr),
 pCustomMapFolderName(false),
 pMapFormat(Antares::Map::mfPNG),
 pLayers(nullptr),
 pSplit(nullptr),
 pSplitNumberChoice(nullptr),
 pUseBackgroundColor(nullptr),
 pBackgroundColor(nullptr),
 pResult(svsCancel)
{
    assert(parent);
    internalCreateComponents();
}

ExportMap::ExportMap(wxFrame* parent, Data::Study::Ptr study) :
 wxDialog(parent,
          wxID_ANY,
          wxT("Export Map"),
          wxDefaultPosition,
          wxDefaultSize,
          wxCLOSE_BOX | wxCAPTION | wxFRAME_FLOAT_ON_PARENT | wxCLIP_CHILDREN),
 pStudy(study),
 pGridSizer(nullptr),
 pParentProperties(nullptr),
 pMapName(nullptr),
 pMapFolderName(nullptr),
 pMapFolderPath(nullptr),
 pCustomMapFolderName(false),
 pMapFormat(Antares::Map::mfPNG),
 pLayers(nullptr),
 pSplit(nullptr),
 pSplitNumberChoice(nullptr),
 pUseBackgroundColor(nullptr),
 pBackgroundColor(nullptr),
 pResult(svsCancel)
{
    assert(parent);
    internalCreateComponents();
}

ExportMap::~ExportMap()
{
    // To avoid corrupt vtable in some rare cases / misuses
    // (when children try to access to this object for example),
    // we should destroy all children as soon as possible.
    auto* sizer = GetSizer();
    if (sizer)
        sizer->Clear(true);
}

wxString ExportMap::getFormatAsString(Antares::Map::mapImageFormat format)
{
    return wxStringFromUTF8(formatNames[format]);
}

YString getFormatAsYString(Antares::Map::mapImageFormat format)
{
    return YString(formatNames[format]);
}

void ExportMap::propCaption(const wxString& text, bool bold)
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

wxTextCtrl* ExportMap::propEdit(const wxString& defaultValue, int id)
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

wxWindow* ExportMap::propEdit(wxWindow* control)
{
    pGridSizer->Add(control, 1, wxALL | wxEXPAND, borderSize);
    return control;
}

wxTextCtrl* ExportMap::propEdit(const wxString& defaultValue,
                                const wxTextValidator& validator,
                                int id)
{
    wxTextCtrl* edit = new wxTextCtrl(
      pParentProperties, id, defaultValue, wxDefaultPosition, wxDefaultSize, 0, validator);
    pGridSizer->Add(edit, 1, wxALL | wxEXPAND, borderSize);
    return edit;
}

void ExportMap::internalCreateComponents()
{
    pParentProperties = this;
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    // Header
    auto* header = Toolbox::Components::WizardHeader::Create(
      this, wxT("Export map options..."), "images/32x32/exportmap.png", wxT(""));
    sizer->Add(header, 0, wxALL | wxEXPAND);
    sizer->SetItemMinSize(header, 550, header->GetSize().GetHeight());

    pGridSizer = new wxFlexGridSizer(2, 0, 0);
    pGridSizer->AddGrowableCol(1, 1);

    // Name and path of the study
    propCaption(wxT("File name:"));
    pMapName = propEdit(wxEmptyString, mnIDEditMapName);

    String folderPath;
    propCaption(wxT("Save in:"));
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
                           wxCommandEventHandler(ExportMap::onBrowse),
                           nullptr,
                           this);

        pMapFolderPath = new wxTextCtrl(
          pnl, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
        if (not pStudy->folder.empty() && defaultPath == wxEmptyString)
        {
            // Get the parent folder
            folderPath = pStudy->folder + SEP << "maps";

            if (not IO::Directory::Exists(folderPath))
            {
                folderPath = pStudy->folder;
                int sepPos = pStudy->folder.find_last_of(SEP);
                if (sepPos != YString::npos)
                {
                    // found it
                    folderPath.truncate(sepPos);
                    defaultPath = folderPath.to<std::string>();
                }
            }
        }
        else
        {
            folderPath = defaultPath.ToStdString();
        }

        pMapFolderPath->SetValue(wxStringFromUTF8(folderPath));

        pnlSizer->AddSpacer(5);
        pnlSizer->Add(pMapFolderPath, 1, wxALL | wxEXPAND);
    }

    ensureUniqueFileName(Yuni::String(defaultPath.ToStdString()));

    // Image format of the image map
    wxArrayString formatsList;
    formatsList.Add(wxStringFromUTF8(formatNames[Antares::Map::mfPNG]));
    formatsList.Add(wxStringFromUTF8(formatNames[Antares::Map::mfJPG]));
    formatsList.Add(wxStringFromUTF8(formatNames[Antares::Map::mfSVG]));

    pMapFormatChoice = new wxChoice(
      pParentProperties, mnIDMapFormat, wxDefaultPosition, wxSize(-1, 22), formatsList);
    pMapFormatChoice->SetSelection(0);
    pMapFormatChoice->Connect(pMapFormatChoice->GetId(),
                              wxEVT_COMMAND_CHOICE_SELECTED,
                              wxCommandEventHandler(ExportMap::onMapFormatChanged),
                              nullptr,
                              this);

    sizer->AddSpacer(5);
    propCaption(wxT("Image format:"));
    pGridSizer->Add(pMapFormatChoice, 1, wxLEFT, borderSize);

    sizer->Add(pGridSizer, 0, wxALL | wxEXPAND, 20);

    //
    // More Options
    //
    auto* colPanl
      = new wxCollapsiblePane(pParentProperties, mnIDCollapsePanel, wxT("More options"));
    pParentProperties = colPanl->GetPane();
    pGridSizer = new wxFlexGridSizer(2, 0, 0);
    pGridSizer->AddGrowableCol(1, 1);
    pParentProperties->SetSizer(pGridSizer);

    // propCaption(wxT("With the folder's name"));
    // pMapFolderName = (wxTextCtrl*)propEdit(wxEmptyString, mnIDEditMapFolderName);
    // pMapFolderName->ChangeValue(pMapName->GetValue());

    // Which layers are exported. Default should be the current layer
    // Image format of the image map
    auto* layersDiv = new wxBoxSizer(wxHORIZONTAL);

    /*wxArrayString layersList;
    layersList.Add(wxStringFromUTF8("1"));
    layersList.Add(wxStringFromUTF8("2"));
    layersList.Add(wxStringFromUTF8("3"));
    layersList.Add(wxStringFromUTF8("4"));
    layersList.Add(wxStringFromUTF8("5"));

    propCaption(wxT("Export layers"));
    pLayers = new wxComboBox(pParentProperties, wxID_ANY, layersList.Item(0), wxDefaultPosition,
    wxSize(-1,22), layersList); pGridSizer->Add(pLayers);*/

    // Splitting the exported map
    /*pSplit = (wxCheckBox*)propEdit(new wxCheckBox(pParentProperties, wxID_ANY,
            wxT("Split the map")));
    pSplit->SetValue(false);

    // How many tiles
    wxArrayString splitList;
    splitList.Add(wxStringFromUTF8("2"));
    splitList.Add(wxStringFromUTF8("4"));
    splitList.Add(wxStringFromUTF8("8"));
    pSplitNumberChoice = new wxChoice(pParentProperties, wxID_ANY, wxDefaultPosition, wxSize(-1,22),
    splitList); pSplitNumberChoice->SetSelection(1);
    */

    // Using a background color. Default should be set to true if JPEG is selected, false otherwise.
    pUseBackgroundColor = (wxCheckBox*)propEdit(
      new wxCheckBox(pParentProperties, wxID_ANY, wxT("Use background colour")));
    pUseBackgroundColor->SetValue(false);
    pUseBackgroundColor->Connect(pUseBackgroundColor->GetId(),
                                 wxEVT_COMMAND_CHECKBOX_CLICKED,
                                 wxCommandEventHandler(ExportMap::onUseBackgroundChanged),
                                 nullptr,
                                 this);
    // pGridSizer->Add(pUseBackgroundColor);
    // Colour picker to choose the background color, if using a one
    pBackgroundColor = new wxColourPickerCtrl(
      pParentProperties,
      wxID_ANY,
      Map::Settings::background); // On windows we avoid WX_WHITE and WX_BLACK because of a bug that
                                  // prevent to pick the colour
    pBackgroundColor->Enable(false);

    pGridSizer->Add(pBackgroundColor);

    // Space at the end of the collapsing panel
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

    // Export - Default button
    auto* btnExportMap
      = Component::CreateButton(this, wxT("Export"), this, &ExportMap::onExportMap);
    btnExportMap->SetDefault();
    pPnlButtonsSizer->Add(btnExportMap, 0, wxALL | wxEXPAND);
    pPnlButtonsSizer->AddSpacer(3);

    // Cancel
    auto* btnCancel = Component::CreateButton(this, wxT("Cancel"), this, &ExportMap::onCancel);
    pPnlButtonsSizer->Add(btnCancel, 0, wxALL | wxEXPAND);
    pPnlButtonsSizer->AddSpacer(20);

    sizer->Add(pPnlButtonsSizer, 0, wxALL | wxEXPAND);

    sizer->AddSpacer(10);
    sizer->Layout();

    btnExportMap->SetDefault();

    // Recentre the window
    SetSizer(sizer);
    sizer->Fit(this);
    Centre(wxBOTH);
}

// Tracking events
void ExportMap::onCollapsePanel(wxCollapsiblePaneEvent&)
{
    // Do nothing
}

void ExportMap::onMapNameChanged(wxCommandEvent&)
{
    if (not pCustomMapFolderName)
    {
        wxString s = pMapName->GetValue();
        s.Trim(false);
        s.Trim(true);
        pMapName->ChangeValue(s);
    }
}

void ExportMap::onMapFolderNameChanged(wxCommandEvent&)
{
    pCustomMapFolderName = true;
}

void ExportMap::onBrowse(wxCommandEvent&)
{
    wxDirDialog dialog(this,
                       _T("Save map in directory:"),
                       pMapFolderPath->GetValue(),
                       wxDD_DEFAULT_STYLE & ~wxDD_DIR_MUST_EXIST);
    if (dialog.ShowModal() == wxID_OK)
    {
        pMapFolderPath->SetValue(dialog.GetPath());
        defaultPath = dialog.GetPath();
        ensureUniqueFileName(Yuni::String(defaultPath.ToStdString()));
    }
}

void ExportMap::ensureUniqueFileName(Yuni::String folderPath)
{
    // Ensure unique name for file -- begin
    String baseMapFileName, currentMapName;
    String mapNameWithoutExtension;
    String mapFileName;
    String fullPathMapName;

    baseMapFileName = "study_map";

    // Current layer name
    currentMapName = pStudy->layers[pStudy->activeLayerID];

    // replace " " by "_"
    if (currentMapName.find(" "))
        currentMapName.replace(" ", "_");

    mapNameWithoutExtension = baseMapFileName << "-" << currentMapName;
    mapFileName = mapNameWithoutExtension + formatExt[Antares::Map::mfDefaultFormat];

    // if a name has already been chosen by the user we only do changes for uniqueness
    if (pMapName->GetValue() != wxEmptyString
        && pMapName->GetValue() != wxStringFromUTF8(mapFileName))
    {
        mapFileName = Yuni::String(pMapName->GetValue().ToStdString());
        // get the name without extension
        int posOfsep;
        posOfsep = mapFileName.find_last_of(".");

        mapNameWithoutExtension = Yuni::String(mapFileName, 0, posOfsep);
        baseMapFileName = mapNameWithoutExtension;

        posOfsep = mapFileName.find_last_of("-");

        if (posOfsep != YString::npos)
        {
            String offsetStr = mapNameWithoutExtension;
            offsetStr.consume(posOfsep + 1);
            if (offsetStr.to<int>())
                baseMapFileName = Yuni::String(mapNameWithoutExtension, 0, posOfsep);
        }
    }

    fullPathMapName = folderPath + SEP << mapFileName;

    uint offsetIndex = 0;

    // Check if exists
    while (IO::Exists(fullPathMapName))
    {
        // get the offset from the name
        int posOfsep;
        posOfsep = mapNameWithoutExtension.find_last_of("-");

        if (posOfsep == YString::npos)
        {
            offsetIndex = 1;
        }
        else
        {
            String offsetStr = mapNameWithoutExtension;
            offsetStr.consume(posOfsep + 1);

            offsetIndex = (offsetStr.to(offsetIndex)) ? ++offsetIndex : 1;
        }

        mapNameWithoutExtension = baseMapFileName + "-" << String(offsetIndex);
        mapFileName = mapNameWithoutExtension + formatExt[Antares::Map::mfDefaultFormat];
        fullPathMapName = folderPath + SEP << mapFileName;
    }
    // Ensure unique name for file -- end
    pMapName->ChangeValue(wxStringFromUTF8(mapFileName));
}

void ExportMap::onMapFormatChanged(wxCommandEvent&)
{
    wxString s = pMapName->GetValue().BeforeLast(wxT('.'));
    s.Trim(false);
    s.Trim(true);

    pMapName->ChangeValue(s + wxStringFromUTF8(formatExt[pMapFormatChoice->GetSelection()]));

    pMapFormat = static_cast<Antares::Map::mapImageFormat>(pMapFormatChoice->GetSelection());
}

void ExportMap::onUseBackgroundChanged(wxCommandEvent&)
{
    pBackgroundColor->Enable(pUseBackgroundColor->IsChecked());
}

void ExportMap::onCancel(void*)
{
    pResult = svsCancel;
    Dispatcher::GUI::Close(this);
}

void ExportMap::onExportMap(void*)
{
    // Informations about the target folder
    String wantedPath;
    wxStringToString(pMapFolderPath->GetValue(), wantedPath);
    String name;
    wxStringToString(pMapName->GetValue(), name);

    // Trimming
    name.trim();
    wantedPath.trim();

    // The user data and the output can only be copied if it is not
    // a fresh study
    bool srcFolderNotEmpty = not pStudy->folder.empty();
    bool split = srcFolderNotEmpty and pSplit and pSplit->GetValue();
    int nbSplitParts
      = srcFolderNotEmpty and pSplitNumberChoice and pSplitNumberChoice->GetSelection();
    std::list<uint16_t> layers(1, 1); // list of the layers ID to save on disk
    bool transparentBackground = !pUseBackgroundColor->GetValue();
    wxColor backgroundColor = pBackgroundColor->GetColour();

    // Checks
    if (name.empty())
    {
        logs.error() << "The name of the map must not be empty";
        return;
    }
    /*if (mapFileName.empty())
    {
            logs.error() <<"The folder's name of the study must not be empty";
            return;
    }*/
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
    wantedPath << SEP << name;
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
        rootFolder << SEP;

        // On a clean windows installation, we are in a `bin` folder
        if (rootFolder.endsWith("\\bin\\"))
        {
            rootFolder += "..\\";
            String tmp;
            IO::Normalize(tmp, rootFolder);
            rootFolder = tmp;
            rootFolder << SEP;
        }

        String pathCopy = path;
        pathCopy.toLower();

        if (pathCopy.startsWith(rootFolder))
        {
            Window::Message message(&mainFrm,
                                    wxT("Save Map"),
                                    wxT("Impossible to save the map at this location"),
                                    wxString()
                                      << wxT("Please choose another folder to save the map."));
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

    if (IO::Exists(path))
    {
        Window::Message message(&mainFrm,
                                wxT("Save As"),
                                wxT("Impossible to save the map"),
                                wxString() << wxT("A file or a folder named '")
                                           << wxStringFromUTF8(path) << wxT("' already exists"));
        message.add(Window::Message::btnCancel);
        message.showModal();
        return;
    }

    // Closing the Window
    Dispatcher::GUI::Close(this);

    mainFrm.exportMap(
      path, transparentBackground, backgroundColor, layers, nbSplitParts, pMapFormat);
}

} // namespace Window
} // namespace Antares
