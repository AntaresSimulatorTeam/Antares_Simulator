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

#include <antares/antares.h>
#include "constraintsbuilder.h"
#include <yuni/io/directory.h>
#include <yuni/io/directory/info.h>

#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/timer.h>
#include <wx/app.h>
#include <wx/dialog.h>
#include <wx/frame.h>
#include <wx/statline.h>
#include <wx/textctrl.h>
#include <wx/dirdlg.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/valtext.h>

#include <ui/common/component/panel.h>
#include "../../toolbox/resources.h"
#include "../../toolbox/create.h"
#include "../../toolbox/components/wizardheader.h"
#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/components/notebook/notebook.h"
#include "../../toolbox/components/button.h"
#include "../../application/main/main.h"
#include "../../application/menus.h"
#include "../../application/study.h"
#include "../message.h"
#include <antares/logs.h>
#include <antares/memory/memory.h>
#include <antares/inifile.h>
#include "../../application/study.h"
#include <antares/config.h>
#include <antares/io/statistics.h>
#include "../../application/main.h"

using namespace Yuni;

#define SEP Yuni::IO::Separator

namespace Antares
{
namespace Window
{
namespace // anonymous
{
} // namespace

BEGIN_EVENT_TABLE(ConstraintsBuilderWizard, wxDialog)

END_EVENT_TABLE()

ConstraintsBuilderWizard::ConstraintsBuilderWizard(wxFrame* parent) :
 wxDialog(parent,
          wxID_ANY,
          wxT("Constraints Builder"),
          wxDefaultPosition,
          wxSize(800, 420),
          wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN),
 pProceedTimer(nullptr)
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Header
    mainSizer->Add(
      Toolbox::Components::WizardHeader::Create(
        this, wxT("Kirchhoff's constraints generator"), "images/32x32/network.png", wxT("")),
      0,
      wxALL | wxEXPAND | wxFIXED_MINSIZE);

    wxBoxSizer* s = new wxBoxSizer(wxHORIZONTAL);

    Component::Notebook* n = new Component::Notebook(this, Component::Notebook::orTop);
    n->theme(Component::Notebook::themeLight);
    mainSizer->Add(n, 1, wxEXPAND | wxALL);

    auto* panelGrid = new Component::Panel(n);
    panelGrid->SetSizer(s);
    Component::Notebook::Page* pageGrid = n->add(panelGrid, wxT(" Dashboard "));

    auto* panelGenerationSettings = new Component::Panel(n);
    wxBoxSizer* sizerGenerationSettings = new wxBoxSizer(wxVERTICAL);
    panelGenerationSettings->SetSizer(sizerGenerationSettings);
    n->add(panelGenerationSettings, wxT(" Advanced settings "));

    {
        wxFlexGridSizer* flexSz = new wxFlexGridSizer(3, 1, 0);
        // Space
        flexSz->AddSpacer(8);
        flexSz->AddSpacer(8);
        flexSz->AddSpacer(8);

        auto* tmpF
          = Component::CreateLabel(panelGenerationSettings,
                                   wxString(wxT("   ")) << wxT("CHECK LOOP FLOW") << wxT("       "),
                                   true);
        tmpF->Enable(false);
        flexSz->Add(tmpF, 0, wxRIGHT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

        flexSz->AddSpacer(5);
        flexSz->AddSpacer(5);
        flexSz->AddSpacer(3);
        flexSz->AddSpacer(3);
        flexSz->AddSpacer(3);

        pCheckNodalLoopFlow = new wxCheckBox(panelGenerationSettings, wxID_ANY, wxT(""));
        pCheckNodalLoopFlow->SetValue(true);
        pCheckNodalLoopFlow->Bind(
          wxEVT_CHECKBOX, &ConstraintsBuilderWizard::onCheckNodalLoopFlow, this);
        flexSz->Add(pCheckNodalLoopFlow, 0, wxALIGN_BOTTOM | wxALIGN_RIGHT);
        flexSz->Add(Antares::Component::CreateLabel(
                      panelGenerationSettings, wxT(" Check loop flow sum at node "), true),
                    0,
                    wxALIGN_BOTTOM | wxALIGN_LEFT);
        flexSz->AddSpacer(5);

        flexSz->AddSpacer(30);
        flexSz->AddSpacer(30);
        flexSz->AddSpacer(30);

        auto label = Antares::Component::CreateLabel(panelGenerationSettings,
                                                     wxString(wxT("   "))
                                                       << wxT("INFINITE VALUE") << wxT("       "),
                                                     true);
        label->Enable(false);
        flexSz->Add(label, 0, wxRIGHT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        flexSz->AddSpacer(5);
        flexSz->AddSpacer(5);

        flexSz->AddSpacer(5);
        flexSz->AddSpacer(5);
        flexSz->AddSpacer(5);

        // flexSz->Add(pCheckNodalLoopFlow, 0, wxALIGN_BOTTOM | wxALIGN_RIGHT);
        flexSz->Add(
          Antares::Component::CreateLabel(panelGenerationSettings, wxT(" Infinite Value: "), true),
          0,
          wxALIGN_BOTTOM | wxALIGN_RIGHT,
          3);

        pInfiniteValue = new wxTextCtrl(
          panelGenerationSettings, wxID_ANY, wxT("1000000"), wxDefaultPosition, wxSize(80, 20));
        pInfiniteValue->Bind(wxEVT_TEXT, &ConstraintsBuilderWizard::onUpdateInfiniteValue, this);
        flexSz->Add(pInfiniteValue, 0, wxALIGN_BOTTOM | wxALIGN_LEFT, 2);
        flexSz->AddSpacer(5);

        sizerGenerationSettings->Add(flexSz, 5, wxALL | wxEXPAND, 5);
        // sizerGenerationSettings->Add(tmpF, 0, wxEXPAND | wxALL);
        // sizerGenerationSettings->AddSpacer(5);

        /*pCheckNodalLoopFlow = new wxCheckBox(panelGenerationSettings, wxID_ANY, wxT(""));
        pCheckNodalLoopFlow->SetValue(true);
        sizerGenerationSettings->Add(Antares::Component::CreateLabel(panelGrid, wxT(" Check loop
        flow sum at nodes "), true), 0, wxALIGN_BOTTOM | wxALIGN_LEFT);*/
    }

    wxFlexGridSizer* flexSz = new wxFlexGridSizer(3, 1, 0);

    // Space
    flexSz->AddSpacer(8);
    flexSz->AddSpacer(8);
    flexSz->AddSpacer(8);
    // Informations about the INPUT
    {
        auto* label = Antares::Component::CreateLabel(
          panelGrid, wxString(wxT("   ")) << wxT("WORKING MAP") << wxT("       "), true);
        label->Enable(false);
        flexSz->Add(label, 0, wxRIGHT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

        flexSz->AddSpacer(5);
        flexSz->AddSpacer(5);
        flexSz->AddSpacer(3);
        flexSz->AddSpacer(3);
        flexSz->AddSpacer(3);

        std::string layerName
          = Data::Study::Current::Get()->layers.at(Data::Study::Current::Get()->activeLayerID);
        mapName = Antares::Component::CreateLabel(panelGrid, wxString(layerName));

        flexSz->Add(Antares::Component::CreateLabel(panelGrid, wxT("Map name : "), true),
                    0,
                    wxALIGN_BOTTOM | wxALIGN_RIGHT);
        flexSz->Add(mapName, 0, wxALIGN_BOTTOM | wxALIGN_LEFT);
        flexSz->AddSpacer(5);
        // gridAppend(*flexSz, wxT("		"), wxT("Map name : "), mapName, true);

        flexSz->AddSpacer(30);
        flexSz->AddSpacer(30);
        flexSz->AddSpacer(30);

        label = Antares::Component::CreateLabel(
          panelGrid, wxString(wxT("   ")) << wxT("TIME FRAME") << wxT("       "), true);
        label->Enable(false);
        flexSz->Add(label, 0, wxRIGHT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        flexSz->AddSpacer(5);
        flexSz->AddSpacer(5);

        flexSz->Add(
          Antares::Component::CreateLabel(
            panelGrid, wxString(wxT("   ")) << wxString(wxT("   ")) << wxT("From hour : "), true),
          0,
          wxALIGN_BOTTOM | wxALIGN_RIGHT);
        wxBoxSizer* hS = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer* vS = new wxBoxSizer(wxVERTICAL);
        hS->AddSpacer(5);
        /*hS->AddSpacer(5);
        hS->Add(Antares::Component::CreateLabel(this, wxT(" to "), true));
        hS->AddSpacer(5);
        flexSz->Add(hS);*/

        // time frame
        String::Vector split;
        Data::Study::Current::Get()->calendar.text.hours[0].split(split, "-");
        auto labelText = wxString("(") << wxStringFromUTF8(split[1]) << ")";
        startingHourLabel = Antares::Component::CreateLabel(panelGrid, labelText, true, true);
        vS->Add(startingHourLabel, 0, wxALIGN_CENTER);
        /*auto btn = new Component::Button(this,"", "images/16x16/hour_pref.png");
        btn->enabled(false);
        hS->Add(btn);*/
        startingHourCtrl
          = new wxTextCtrl(panelGrid, wxID_ANY, wxT("1"), wxDefaultPosition, wxSize(40, 20));
        startingHourCtrl->Bind(
          wxEVT_TEXT, &ConstraintsBuilderWizard::updateBeginningHourLabel, this);
        vS->Add(startingHourCtrl, 0, wxALIGN_CENTER);
        hS->Add(vS, 0, wxALIGN_LEFT);
        hS->AddSpacer(5);
        startingHourSizer = vS;
        pFlexSizer = flexSz;

        vS = new wxBoxSizer(wxVERTICAL);

        vS->AddSpacer(15);
        vS->Add(Antares::Component::CreateLabel(panelGrid, wxT(" to "), true), 0, wxALIGN_BOTTOM);
        hS->Add(vS, 0, wxALIGN_LEFT);

        hS->AddSpacer(5);

        vS = new wxBoxSizer(wxVERTICAL);
        split.clear();
        Data::Study::Current::Get()->calendar.text.hours[8759].split(split, "-");
        labelText = wxString("(") << wxStringFromUTF8(split[1]) << ")";
        endHourLabel = Antares::Component::CreateLabel(panelGrid, labelText, true, true);
        vS->Add(endHourLabel, 0, wxALIGN_CENTER);
        /*btn = new Component::Button(this, "", "images/16x16/hour_pref.png");
        btn->enabled(false);
        hS->Add(btn);*/
        endHourCtrl
          = new wxTextCtrl(panelGrid, wxID_ANY, wxT("8760"), wxDefaultPosition, wxSize(40, 20));
        endHourCtrl->Bind(wxEVT_TEXT, &ConstraintsBuilderWizard::updateEndHourLabel, this);
        vS->Add(endHourCtrl, 0, wxALIGN_CENTER);
        hS->Add(vS);
        /*btn = new Component::Button(this, wxString::Format(wxT("%i"), 1),
        "images/16x16/hour_pref.png"); btn->menu(true);*/
        flexSz->Add(hS, 0, wxALIGN_LEFT);

        flexSz->AddSpacer(5);

        endHourSizer = vS;

        flexSz->AddSpacer(30);
        flexSz->AddSpacer(30);
        flexSz->AddSpacer(30);

        label = Antares::Component::CreateLabel(
          panelGrid, wxString(wxT("   ")) << wxT("LOOP FLOW") << wxT("       "), true);
        label->Enable(false);
        flexSz->Add(label, 0, wxRIGHT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        flexSz->AddSpacer(5);
        flexSz->AddSpacer(5);

        flexSz->AddSpacer(5);
        flexSz->AddSpacer(5);
        flexSz->AddSpacer(5);

        // Include passive loopflow
        {
            pIncludeLoopFlow = new wxCheckBox(panelGrid, wxID_ANY, wxT(""));
            pIncludeLoopFlow->SetValue(false);
            pIncludeLoopFlow->Bind(
              wxEVT_CHECKBOX, &ConstraintsBuilderWizard::onIncludeLoopFlow, this);
            flexSz->Add(pIncludeLoopFlow, 0, wxALIGN_BOTTOM | wxALIGN_RIGHT);
            flexSz->Add(
              Antares::Component::CreateLabel(panelGrid, wxT(" Include passive Loop Flows "), true),
              0,
              wxALIGN_BOTTOM | wxALIGN_LEFT);
            flexSz->AddSpacer(5);
        }

        // Include passive loopflow
        {
            pIncludePhaseShifts = new wxCheckBox(panelGrid, wxID_ANY, wxT(""));
            pIncludePhaseShifts->SetValue(false);
            pIncludePhaseShifts->Bind(
              wxEVT_CHECKBOX, &ConstraintsBuilderWizard::onUsePhaseShift, this);
            flexSz->Add(pIncludePhaseShifts, 0, wxALIGN_BOTTOM | wxALIGN_RIGHT);
            flexSz->Add(Antares::Component::CreateLabel(
                          panelGrid, wxT(" Include active phase-shifts "), true),
                        0,
                        wxALIGN_BOTTOM | wxALIGN_LEFT);
            flexSz->AddSpacer(5);
        }
    }

    s->Add(flexSz, 5, wxALL | wxEXPAND, 5);

    // Link List
    auto* text1 = Component::CreateLabel(panelGrid, wxT("Mapping Links"), true);
    wxBoxSizer* sLinks = new wxBoxSizer(wxVERTICAL);
    auto* h = new wxBoxSizer(wxHORIZONTAL);
    h->AddSpacer(12);
    h->Add(text1, 0, wxEXPAND | wxALL);
    s->Add(sLinks, 5, wxALL | wxEXPAND, 5);
    sLinks->Add(h, 0, wxEXPAND | wxALL);
    sLinks->AddSpacer(6);

    // Constraint builder object
    pCBuilder = new CBuilder(Data::Study::Current::Get());
    pCBuilder->completeFromStudy();
    pCBuilder->completeCBuilderFromFile();
    pIncludeLoopFlow->SetValue(pCBuilder->getLoopFlowInclusion());
    pIncludePhaseShifts->SetValue(pCBuilder->getPhaseShiftInclusion());
    startingHourCtrl->SetValue(wxString("") << pCBuilder->getCalendarStart());
    endHourCtrl->SetValue(wxString("") << pCBuilder->getCalendarEnd());
    pCheckNodalLoopFlow->SetValue(pCBuilder->getCheckNodalLoopFlow());
    pInfiniteValue->SetValue(wxString() << pCBuilder->getInfinite());

    // Mapping links
    // \_ renderer
    pRenderer = new RendererType(pCBuilder);
    pRenderer->study = Data::Study::Current::Get();
    pRenderer->initializeFromStudy();

    pGrid
      = new Component::Datagrid::Component(panelGrid, pRenderer, wxEmptyString, false, true, true);
    pGrid->markTheStudyAsModified(false);
    pGrid->Enable(false);

    sLinks->Add(pGrid, 1, wxEXPAND | wxALL);

    pageGrid->select();

    // mainSizer -> Add(s, -1, wxEXPAND | wxALL);

    // warning
    wxStaticLine* line = new wxStaticLine(this, -1);
    mainSizer->Add(line, 0, wxEXPAND);
    // check if network constraints are already existing
    pDelete = pCBuilder->alreadyExistingNetworkConstraints(CB_PREFIX);
    if (pDelete)
    {
        wxBoxSizer* mediumSizer = new wxBoxSizer(wxHORIZONTAL);

        pTextDelete = new wxStaticText(
          this,
          -1,
          "Warning: Previous generation was detected. Reset may be used to remove \""
            + std::string(CB_PREFIX) + "\" constraints before generating new ones");
        wxSize size = GetSize();
        pTextDelete->Wrap(size.GetWidth() * 9 / 10);
        mediumSizer->Add(pTextDelete, 0, wxEXPAND | wxALL, 5);
        mainSizer->Add(mediumSizer, 0, wxEXPAND | wxALL, 5);
    }
    // Buttons
    wxBoxSizer* bottomSizer = new wxBoxSizer(wxHORIZONTAL);

    wxWindow* pBtnDelete
      = Component::CreateButton(this, wxT(" Reset "), this, &ConstraintsBuilderWizard::onDelete);
    bottomSizer->Add(pBtnDelete, 0, wxALL | wxEXPAND);
    pBtnDelete->Enable(pDelete);

    bottomSizer->AddSpacer(5);
    wxWindow* pBtnStatus = Component::CreateButton(
      this, wxT(" Show Status "), this, &ConstraintsBuilderWizard::onUpdateLinesStatus);
    bottomSizer->Add(pBtnStatus, 0, wxALL | wxEXPAND);
    pBtnStatus->Enable(true);

    bottomSizer->AddSpacer(5);
    bottomSizer->Add(
      new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL),
      0,
      wxALL | wxEXPAND);
    bottomSizer->AddSpacer(5);

    bottomSizer->AddSpacer(5);
    wxWindow* pBtnBuild
      = Component::CreateButton(this, wxT(" Run "), this, &ConstraintsBuilderWizard::onBuild);
    bottomSizer->Add(pBtnBuild, 0, wxALL | wxEXPAND);
    pBtnBuild->Enable(true);

    bottomSizer->AddSpacer(5);

    wxWindow* btnCancel
      = Component::CreateButton(this, wxT(" Cancel "), this, &ConstraintsBuilderWizard::onCancel);
    bottomSizer->Add(btnCancel, 0, wxALL | wxEXPAND);

    mainSizer->Add(bottomSizer, 0, wxALIGN_RIGHT | wxALL, 5);

    // set the sizer
    SetSizer(mainSizer);
    pGrid->forceRefresh();
    pGrid->Enable(true);
}

ConstraintsBuilderWizard::~ConstraintsBuilderWizard()
{
    delete pCBuilder;
}

void ConstraintsBuilderWizard::onCheckNodalLoopFlow(wxCommandEvent& evt)
{
    auto value = pCheckNodalLoopFlow->GetValue();

    // update Cbuilder
    pCBuilder->setCheckNodalLoopFlow(value);
    pCBuilder->setUpToDate(false);
    pCBuilder->saveCBuilderToFile();
}

void ConstraintsBuilderWizard::onUpdateInfiniteValue(wxCommandEvent& evt)
{
    // Validate();
    auto value = pInfiniteValue->GetValue();
    double infinite = 1000000;

    try
    {
        infinite = std::stod(value.ToStdString());
    }
    catch (...)
    {
        Window::Message message(this,
                                wxT("Constraint Generator"),
                                wxT("Constraint Generator"),
                                wxT("The infinite value must be a number."));
        message.add(Window::Message::btnOk);
        message.showModal();
    }
    pCBuilder->setInfinite(infinite);
    pCBuilder->setUpToDate(false);
    pCBuilder->saveCBuilderToFile();
}

void ConstraintsBuilderWizard::onIncludeLoopFlow(wxCommandEvent& evt)
{
    auto value = pIncludeLoopFlow->GetValue();

    // update Cbuilder
    pCBuilder->setLoopFlowInclusion(value);
    pCBuilder->setUpToDate(false);
    pCBuilder->saveCBuilderToFile();

    // update all lines in the study
    /*for (auto link : pCBuilder->pLink)
    {
            link->ptr->useLoopFlow = value;
            //onConnectionChanged(link->ptr);
            OnStudyLinkChanged(link->ptr);
    }
    MarkTheStudyAsModified();*/
    // Forms::ApplWnd::Instance()->pageLinksDetails->refresh();
    pGrid->forceRefresh();
}

void ConstraintsBuilderWizard::onUsePhaseShift(wxCommandEvent& evt)
{
    auto value = pIncludePhaseShifts->GetValue();

    // update Cbuilder
    pCBuilder->setPhaseShiftInclusion(value);
    pCBuilder->setUpToDate(false);
    pCBuilder->saveCBuilderToFile();

    // update all lines in the study
    /*for (auto link : pCBuilder->pLink)
    {
            link->ptr->usePST = value;
            //onConnectionChanged(link->ptr);
            OnStudyLinkChanged(link->ptr);
    }*/
    // MarkTheStudyAsModified();
    // Forms::ApplWnd::Instance()->pageLinksDetails->refresh();
    pGrid->forceRefresh();
}

void ConstraintsBuilderWizard::onUpdateLinesStatus(void*)
{
    if (StudyHasBeenModified())
    {
        Window::Message message(this,
                                wxT("Constraint Generator"),
                                wxT("Constraint Generator"),
                                wxT("The study has been modified. All changes must be written to "
                                    "the disk\nbefore updating the status."));
        message.add(Window::Message::btnSaveChanges);
        message.add(Window::Message::btnCancel, true);
        if (message.showModal() == Window::Message::btnCancel)
        {
            return;
        }

        const SaveResult r = ::Antares::SaveStudy();
        if (!(r == svsDiscard or r == svsSaved))
        {
            return;
        }
    }
    // update Cbuilder
    // pCBuilder->kruskal();
    pCBuilder->update();
    pGrid->forceRefresh();
}

void ConstraintsBuilderWizard::onCancel(void*)
{
    Dispatcher::GUI::Close(this);
}

void ConstraintsBuilderWizard::onBuild(void*)
{
    // Check for restrictions
    if (not Data::Study::Current::Valid())
        return;

    if (StudyHasBeenModified())
    {
        Window::Message message(this,
                                wxT("Constraint Generator"),
                                wxT("Constraint Generator"),
                                wxT("The study has been modified. All changes must be written to "
                                    "the disk\nbefore using the constraint generator."));
        message.add(Window::Message::btnSaveChanges);
        message.add(Window::Message::btnCancel, true);
        if (message.showModal() == Window::Message::btnCancel)
        {
            return;
        }

        const SaveResult r = ::Antares::SaveStudy();
        if (!(r == svsDiscard or r == svsSaved))
        {
            return;
        }
    }

    if (pDelete)
    {
        Window::Message message(
          this,
          wxT("Constraint Generator"),
          wxT("Constraint Generator"),
          wxT("Warning :  current study includes the result of previous generations. To remove all "
              "such constraints (named  \"@UTO-\" ) , use \"Reset\" before \"Run\""));
        message.add(Window::Message::btnOk);
        message.add(Window::Message::btnCancel, true);
        if (message.showModal() == Window::Message::btnCancel)
        {
            return;
        }
    }

    // Block all matrices
    // It will be unlocked later
    OnStudyBeginUpdate();

    // Disabling all components
    // enableAll(false);
    pGrid->Enable(false);

    {
        String file;
        file << Antares::memory.cacheFolder() << SEP << "antares-" << Antares::memory.processID()
             << ".antares-cbuilder";

        if (saveToFile(file))
        {
            cBuilderInfoFile(file);
        }
        else
        {
            logs.error() << "Impossible to create a temporary file which would contain the "
                            "required informations for the constraint generator";
        }
    }

    // String filename = "D:/settings.ini";
    auto& mainFrm = *Forms::ApplWnd::Instance();
    Dispatcher::GUI::Close(this);
    mainFrm.launchConstraintsBuilder(cBuilderInfoFile());
}

void ConstraintsBuilderWizard::onDelete(void*)
{
    // Check for restrictions
    if (not Data::Study::Current::Valid())
        return;
    auto studyptr = Data::Study::Current::Get();
    if (not studyptr)
        return;
    auto& study = *studyptr;

    if (StudyHasBeenModified())
    {
        Window::Message message(this,
                                wxT("Constraint Generator"),
                                wxT("Constraint Generator"),
                                wxT("The study has been modified. All changes must be written to "
                                    "the disk\nbefore using the constraint generator."));
        message.add(Window::Message::btnSaveChanges);
        message.add(Window::Message::btnCancel, true);
        if (message.showModal() == Window::Message::btnCancel)
        {
            return;
        }

        const SaveResult r = ::Antares::SaveStudy();
        if (!(r == svsDiscard or r == svsSaved))
        {
            return;
        }
    }
    else
    {
        Window::Message message(this,
                                wxT("Constraint Generator"),
                                wxT("Constraint Generator"),
                                wxT("The generated constraints will be deleted. Are you sure?"));
        message.add(Window::Message::btnYes);
        message.add(Window::Message::btnCancel, true);
        if (message.showModal() == Window::Message::btnCancel)
        {
            return;
        }

        if (pCBuilder->deletePreviousConstraints())
        {
            // pGrid->markTheStudyAsModified();
            /*const SaveResult r = ::Antares::SaveStudy();
            if (!(r == svsDiscard or r == svsSaved))
            {
                    Enable(false);
                    return;
            }*/
            study.uiinfo->reloadBindingConstraints();

            InvalidateBestSize();
            OnStudyConstraintDelete(nullptr);

            MarkTheStudyAsModified();
            // Dispatcher::GUI::Refresh(this);
            pDelete = false;
            pTextDelete->Hide();
            // pBtnDelete->Enable(false);
            GetSizer()->Layout();
        }
    }
}

void ConstraintsBuilderWizard::enableAll(bool v)
{
    pGrid->Enable(v);
    pBtnBuild->Enable(false);
}

void ConstraintsBuilderWizard::fileMapping(FileMapping* m)
{
    pFileMapping = m;

    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, mnIDUpdateFileMapping);
    AddPendingEvent(evt);
}

bool ConstraintsBuilderWizard::saveToFile(const String& filename) const
{
    return pCBuilder->saveCBuilderToFile(filename);
}

void ConstraintsBuilderWizard::gridAppend(wxFlexGridSizer& sizer,
                                          const wxString& title,
                                          const wxString& key,
                                          wxWindow* value,
                                          bool bold)
{
    if (title.empty())
    {
        sizer.AddStretchSpacer();
    }
    else
    {
        auto* t = Antares::Component::CreateLabel(
          this, wxString(wxT("   ")) << title << wxT("       "), true);
        t->Enable(false);
        sizer.Add(t, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    }

    // Key
    wxStaticText* keyLbl = Antares::Component::CreateLabel(this, key, bold);
    sizer.Add(keyLbl, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

    // Value
    sizer.Add(value, 0, wxRIGHT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
}

void ConstraintsBuilderWizard::updateBeginningHourLabel(wxEvent& evt)
{
    wxString texValue = startingHourCtrl->GetValue();
    int num;
    num = wxAtoi(texValue);
    if (num > 0 && num < 8760)
    {
        String::Vector split;
        Data::Study::Current::Get()->calendar.text.hours[num - 1].split(split, "-");
        auto labelText = wxString("(") << wxStringFromUTF8(split[1]) << ")";
        startingHourLabel->SetLabel(labelText);
        pCBuilder->setCalendarStart(num);
        pCBuilder->saveCBuilderToFile();
    }
    else
    {
        startingHourLabel->SetLabel(wxT("???"));
    }
    startingHourSizer->Layout();
    pFlexSizer->Layout();
}

void ConstraintsBuilderWizard::updateEndHourLabel(wxEvent& evt)
{
    wxString texValue = endHourCtrl->GetValue();
    int num;
    num = wxAtoi(texValue);
    if (num > 0 && num < 8761)
    {
        String::Vector split;
        Data::Study::Current::Get()->calendar.text.hours[num - 1].split(split, "-");
        auto labelText = wxString("(") << wxStringFromUTF8(split[1]) << ")";
        endHourLabel->SetLabel(labelText);
        pCBuilder->setCalendarEnd(num);
        pCBuilder->saveCBuilderToFile();
    }
    else
    {
        endHourLabel->SetLabel(wxT("???"));
    }

    endHourSizer->Layout();
    pFlexSizer->Layout();
}

} // namespace Window
} // namespace Antares
