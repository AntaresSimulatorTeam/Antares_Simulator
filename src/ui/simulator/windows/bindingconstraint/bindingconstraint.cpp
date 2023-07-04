/*
** Copyright 2007-2023 RTE
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

#include "bindingconstraint.h"

#include "../../toolbox/validator.h"

#include <ui/common/component/panel.h>
#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/components/button.h"
#include "../../toolbox/components/datagrid/gridhelper.h"
#include "../../toolbox/components/datagrid/renderer/bindingconstraint/weights.h"
#include "../../toolbox/components/datagrid/renderer/bindingconstraint/offsets.h"
#include "../../toolbox/components/datagrid/renderer/bindingconstraint/data.h"
#include "../../toolbox/resources.h"
#include "../../toolbox/create.h"
#include "../../windows/message.h"

#include "edit.h"
#include "../../application/main.h"
#include "../../application/wait.h"
#include "antares/study/ui-runtimeinfos.h"

using namespace Yuni;

namespace Antares
{
namespace Window
{
BEGIN_EVENT_TABLE(BindingConstraint, wxPanel)
END_EVENT_TABLE()

class BindingConstraintWeightsPanel final
 : public Component::Panel,
   public Yuni::IEventObserver<BindingConstraintWeightsPanel>
{
public:
    BindingConstraintWeightsPanel(wxWindow* parent) : Component::Panel(parent)
    {
        auto* sizer = new wxBoxSizer(wxVERTICAL);

        auto* notebook = new Component::Notebook(this, Component::Notebook::orTop);
        notebook->theme(Component::Notebook::themeLight);

        linkWeightsGrid = new Component::Datagrid::Component(
          notebook,
          new Component::Datagrid::Renderer::BindingConstraint::LinkWeights(notebook),
          wxEmptyString,
          true,
          true,
          false,
          false,
          true);
        pPageLinkWeights = notebook->add(linkWeightsGrid, wxT("links"), wxT("  Links  "));

        clusterWeightsGrid = new Component::Datagrid::Component(
          notebook,
          new Component::Datagrid::Renderer::BindingConstraint::ClusterWeights(notebook),
          wxEmptyString,
          true,
          true,
          false,
          false,
          true);
        pPageclusterWeights
          = notebook->add(clusterWeightsGrid, wxT("clusters"), wxT("  Clusters  "));

        // Post
        notebook->select(wxT("links"));
        sizer->Add(notebook, 1, wxALL | wxEXPAND);
        SetSizer(sizer);
        // pPageLinkWeights->select();
    }

    virtual ~BindingConstraintWeightsPanel()
    {
        destroyBoundEvents();
    }

    void forceRefresh()
    {
        assert(clusterWeightsGrid && "Invalid pointer to clusterWeightsGrid");
        assert(linkWeightsGrid && "Invalid pointer to linkWeightsGrid");
        assert(GetSizer() && "Invalid sizer");

        if (Data::Study::Current::Valid())
        {
            linkWeightsGrid->InvalidateBestSize();
            linkWeightsGrid->forceRefresh();
            clusterWeightsGrid->InvalidateBestSize();
            clusterWeightsGrid->forceRefresh();
            Data::UIRuntimeInfo& uiinfo = *(Data::Study::Current::Get()->uiinfo);
            wxString s;
            uint c;

            c = uiinfo.visibleLinksCount(Data::Study::Current::Get()->activeLayerID);
            s << wxT("  Links  (") << c << wxT(") ");
            pPageLinkWeights->caption(s);

            c = uiinfo.visibleClustersCount(Data::Study::Current::Get()->activeLayerID);
            s.clear();
            s << wxT("  Thermal Clusters  (") << c << wxT(") ");
            pPageclusterWeights->caption(s);

            GetSizer()->Layout();
        }
    }

    void selectDefaultPage()
    {
        // Select the first page
        pPageLinkWeights->select(true);
    }

public:
    Component::Datagrid::Component* linkWeightsGrid;
    Component::Datagrid::Component* clusterWeightsGrid;

private:
    Component::Notebook::Page* pPageLinkWeights;
    Component::Notebook::Page* pPageclusterWeights;

}; // class BindingConstraintWeightsPanel

class BindingConstraintOffsetsPanel final
 : public Component::Panel,
   public Yuni::IEventObserver<BindingConstraintOffsetsPanel>
{
public:
    BindingConstraintOffsetsPanel(wxWindow* parent) : Component::Panel(parent)
    {
        auto* sizer = new wxBoxSizer(wxVERTICAL);

        auto* notebook = new Component::Notebook(this, Component::Notebook::orTop);
        notebook->theme(Component::Notebook::themeLight);

        linkOffsetsGrid = new Component::Datagrid::Component(
          notebook,
          new Component::Datagrid::Renderer::BindingConstraint::LinkOffsets(notebook),
          wxEmptyString,
          true,
          true,
          false,
          false,
          true);
        pPageLinkOffsets = notebook->add(linkOffsetsGrid, wxT("links"), wxT("  Links  "));

        clusterOffsetsGrid = new Component::Datagrid::Component(
          notebook,
          new Component::Datagrid::Renderer::BindingConstraint::ClusterOffsets(notebook),
          wxEmptyString,
          true,
          true,
          false,
          false,
          true);
        pPageclusterOffsets
          = notebook->add(clusterOffsetsGrid, wxT("clusters"), wxT("  Clusters  "));

        // Post
        notebook->select(wxT("links"));
        sizer->Add(notebook, 1, wxALL | wxEXPAND);
        SetSizer(sizer);
        // pPageLinkWeights->select();
    }

    virtual ~BindingConstraintOffsetsPanel()
    {
        destroyBoundEvents();
    }

    void forceRefresh()
    {
        assert(clusterOffsetsGrid && "Invalid pointer to clusterOffsetsGrid");
        assert(linkOffsetsGrid && "Invalid pointer to linkOffsetsGrid");
        assert(GetSizer() && "Invalid sizer");

        if (Data::Study::Current::Valid())
        {
            linkOffsetsGrid->InvalidateBestSize();
            linkOffsetsGrid->forceRefresh();
            clusterOffsetsGrid->InvalidateBestSize();
            clusterOffsetsGrid->forceRefresh();
            Data::UIRuntimeInfo& uiinfo = *(Data::Study::Current::Get()->uiinfo);
            wxString s;
            uint c;

            c = uiinfo.visibleLinksCount(Data::Study::Current::Get()->activeLayerID);
            s << wxT("  Links  (") << c << wxT(") ");
            pPageLinkOffsets->caption(s);

            c = uiinfo.visibleClustersCount(Data::Study::Current::Get()->activeLayerID);
            s.clear();
            s << wxT("  Thermal Clusters  (") << c << wxT(") ");
            pPageclusterOffsets->caption(s);

            GetSizer()->Layout();
        }
    }

    void selectDefaultPage()
    {
        // Select the first page
        pPageLinkOffsets->select(true);
    }

public:
    Component::Datagrid::Component* linkOffsetsGrid;
    Component::Datagrid::Component* clusterOffsetsGrid;

private:
    Component::Notebook::Page* pPageLinkOffsets;
    Component::Notebook::Page* pPageclusterOffsets;

}; // class BindingConstraintWeightsPanel

BindingConstraint::BindingConstraint(wxWindow* parent) :
 Panel(parent),
 pPageList(nullptr),
 pPageWeights(nullptr),
 pPageOffsets(nullptr),
 pPageEqual(nullptr),
 pPageLess(nullptr),
 pPageGreater(nullptr),
 // pGridWeights(nullptr),
 pAllConstraints(nullptr),
 pSelected(nullptr),
 pWeightsPanel(nullptr)
{
    // Main sizer
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    auto* n = new Component::Notebook(this, Component::Notebook::orTop);
    n->caption(wxT("Binding constraints"));

    // List of all binding constraints
    {
        wxPanel* p = new wxPanel(n, wxID_ANY);
        p->SetSizer(new wxBoxSizer(wxVERTICAL));
        wxSizer* h = new wxBoxSizer(wxHORIZONTAL);

        h->AddSpacer(10);

        Component::Button* btn;

        // Create a new thermal constraint
        btn = new Component::Button(p,
                                    wxT("Create a constraint"),
                                    "images/16x16/constraint_plus.png",
                                    this,
                                    &BindingConstraint::onAdd);
        h->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxALL);

        // Delete
        btn = new Component::Button(p,
                                    wxT("Delete"),
                                    "images/16x16/constraint_minus.png",
                                    this,
                                    &BindingConstraint::onDelete);
        h->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxALL);

        // vertical line
        Component::AddVerticalSeparator(p, h);

        // Edit
        btn = new Component::Button(
          p, wxT("Edit"), "images/16x16/constraint_edit.png", this, &BindingConstraint::onEdit);
        h->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxALL);

        // vertical line
        Component::AddVerticalSeparator(p, h);

        // vertical line
        Component::AddVerticalSeparator(p, h);

        // Delete  All
        btn = new Component::Button(p,
                                    wxT("Delete all"),
                                    "images/16x16/delete_all_BC.png",
                                    this,
                                    &BindingConstraint::onDeleteAll);
        h->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxALL);

        p->GetSizer()->Add(h, 0, wxALL | wxEXPAND, 2);

        pAllConstraints = new Toolbox::InputSelector::BindingConstraint(p);
        p->GetSizer()->Add(pAllConstraints, 1, wxALL | wxEXPAND);
        pPageList = n->add(p, wxT("list"), wxT("  Summary  "));
    }

    // Weights
    {
        pWeightsPanel = new BindingConstraintWeightsPanel(n);
        pPageWeights = n->add(pWeightsPanel, wxT("weights"), wxT("  Weights  "));
    }

    // Offsets
    {
        pOffsetsPanel = new BindingConstraintOffsetsPanel(n);
        pPageOffsets = n->add(pOffsetsPanel, wxT("offsets"), wxT("  Offsets  "));
    }

    // Second membre
    {
        auto unavailablePanel = new Component::Panel(n);
        n->add(unavailablePanel, wxT("antares-web"), wxT("  Please use Antares Web to edit BC data  "));
    }
    

    // Post
    n->select(wxT("list"));
    sizer->Add(n, 1, wxALL | wxEXPAND);

    // Events
    n->onPageChanged.connect(this, &BindingConstraint::onPageChanged);

    // Update the layout
    SetSizer(sizer);
    GetSizer()->Layout();

    pAllConstraints->onBindingConstraintChanged.connect(this,
                                                        &BindingConstraint::onSelectedConstraint);
    pAllConstraints->onBindingConstraintDblClick.connect(this,
                                                         &BindingConstraint::onDblClickConstraint);
}

BindingConstraint::~BindingConstraint()
{
    pSelected = nullptr;
    destroyBoundEvents();

    // To avoid corrupt vtable in some rare cases / misuses
    // (when children try to access to this object for example),
    // we should destroy all children as soon as possible.
    auto* sizer = GetSizer();
    if (sizer)
        sizer->Clear(true);
}

void BindingConstraint::onPageChanged(Component::Notebook::Page& page)
{
    InvalidateBestSize();
    Refresh();

    if (&page == pPageList)
    {
        logs.info() << "notebook binding constraints: page changed: all constraints";
        pAllConstraints->InvalidateBestSize();
        pAllConstraints->updateRowContent();
        pAllConstraints->Refresh();
        return;
    }
    if (&page == pPageWeights)
    {
        logs.info() << "notebook binding constraints: page changed: weights";

        pWeightsPanel->forceRefresh();

        return;
    }
    if (&page == pPageOffsets)
    {
        logs.info() << "notebook binding constraints: page changed: offsets";

        pOffsetsPanel->forceRefresh();
        return;
    }
}

void BindingConstraint::onAddFromMouse(wxMouseEvent& evt)
{
    onAdd(nullptr);
    evt.Skip();
}

void BindingConstraint::onDeleteFromMouse(wxMouseEvent& evt)
{
    onDelete(nullptr);
    evt.Skip();
}

void BindingConstraint::onEditFromMouse(wxMouseEvent& evt)
{
    onEdit(nullptr);
    evt.Skip();
}

void BindingConstraint::onAdd(void*)
{
    auto studyptr = Data::Study::Current::Get();
    if (not studyptr)
        return;
    auto& study = *studyptr;

    // To avoid some useless refresh (and flickering in the same time)
    // We will keep the current number of binding constraints and we
    // will compare it after the deletion of the window to make a refresh
    // if required.
    uint oldCount = study.bindingConstraints.size();

    // The Window
    auto* editor = new BindingConstraintInfoEditor(Forms::ApplWnd::Instance(), nullptr);
    editor->ShowModal();
    editor->Destroy();
    WIP::Locker wip;
    // Refresh only if required
    if (oldCount != study.bindingConstraints.size())
    {
        InvalidateBestSize();
        pAllConstraints->update();
        pAllConstraints->InvalidateBestSize();
        pAllConstraints->updateRowContent();
        Dispatcher::GUI::Refresh(pAllConstraints);

        MarkTheStudyAsModified();
        Dispatcher::GUI::Refresh(this);
    }
}

void BindingConstraint::onDelete(void*)
{
    if (not pSelected)
        return;
    auto studyptr = Data::Study::Current::Get();
    if (not studyptr)
        return;
    auto& study = *studyptr;

    auto& mainFrm = *Forms::ApplWnd::Instance();

    Window::Message message(&mainFrm,
                            wxT("Binding constraint"),
                            wxT("Delete a binding constraint"),
                            wxString()
                              << wxT("Do you really want to delete the binding constraint '")
                              << wxStringFromUTF8(pSelected->name()) << wxT("' ?"));
    message.add(Window::Message::btnYes);
    message.add(Window::Message::btnCancel, true);
    if (message.showModal() == Window::Message::btnYes)
    {
        logs.info() << "deleting the constraint " << pSelected->name();
        WIP::Locker wip;
        study.bindingConstraints.remove(pSelected);
        study.uiinfo->reloadBindingConstraints();
        pSelected = nullptr;

        InvalidateBestSize();
        pAllConstraints->update();
        pAllConstraints->InvalidateBestSize();
        pAllConstraints->updateRowContent();
        OnStudyConstraintDelete(nullptr);
        Dispatcher::GUI::Refresh(pAllConstraints);

        MarkTheStudyAsModified();
        Dispatcher::GUI::Refresh(this);
    }
}

void BindingConstraint::onEdit(void*)
{
    if (not pSelected)
        return;
    auto* editor = new BindingConstraintInfoEditor(Forms::ApplWnd::Instance(), pSelected);
    editor->ShowModal();
    editor->Destroy();
    pAllConstraints->updateRowContent();
}

void BindingConstraint::onDeleteAll(void*)
{
    auto studyptr = Data::Study::Current::Get();
    if (not studyptr)
        return;
    auto& study = *studyptr;

    auto& mainFrm = *Forms::ApplWnd::Instance();

    Window::Message message(
      &mainFrm,
      wxT("Binding constraint"),
      wxT("Delete all binding constraints"),
      wxString() << wxT("Do you really want to delete all the binding constraints ") << wxT("' ?"));
    message.add(Window::Message::btnYes);
    message.add(Window::Message::btnCancel, true);
    if (message.showModal() == Window::Message::btnYes)
    {
        logs.info() << "deleting the constraints ";
        WIP::Locker wip;

        auto BC = study.bindingConstraints.begin();
        int BCListSize = study.bindingConstraints.size();
        for (int i = 0; i < BCListSize; i++)
        {
            if ((*BC)->hasAllWeightedLinksOnLayer(study.activeLayerID)
                && (*BC)->hasAllWeightedClustersOnLayer(study.activeLayerID))
                study.bindingConstraints.remove(BC->get());
            else
                ++BC;
        }

        study.uiinfo->reloadBindingConstraints();
        pSelected = nullptr;

        InvalidateBestSize();
        pAllConstraints->update();
        pAllConstraints->InvalidateBestSize();
        pAllConstraints->updateRowContent();
        OnStudyConstraintDelete(nullptr);
        Dispatcher::GUI::Refresh(pAllConstraints);

        MarkTheStudyAsModified();
        Dispatcher::GUI::Refresh(this);
    }
}

void BindingConstraint::onSelectedConstraint(Data::BindingConstraint* constraint)
{
    pSelected = (Data::Study::Current::Valid() ? constraint : nullptr);
}

void BindingConstraint::onDblClickConstraint(Data::BindingConstraint* constraint)
{
    pSelected = (Data::Study::Current::Valid() ? constraint : nullptr);
    auto* editor = new BindingConstraintInfoEditor(Forms::ApplWnd::Instance(), pSelected);
    editor->ShowModal();
    editor->Destroy();
    pAllConstraints->updateRowContent();
}

void BindingConstraint::selectDefaultPage()
{
    if (pPageList)
        pPageList->select();
}

} // namespace Window
} // namespace Antares
