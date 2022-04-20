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

#include "panel.h"
#include <antares/date.h>
#include <wx/statline.h>
#include <antares/study/scenario-builder/rules.h>
#include <antares/study/scenario-builder/sets.h>
#include "../../application/study.h"
#include <wx/sizer.h>
#include "../../toolbox/components/captionpanel.h"
#include "../../toolbox/create.h"
#include "../../application/menus.h"
#include "../../windows/message.h"
#include <wx/textdlg.h>

using namespace Yuni;

#define DEFAULT_RULESET_NAME "New Ruleset"

namespace Antares
{
namespace Window
{
namespace ScenarioBuilder
{
namespace // anonymous
{
inline wxString RulesetCaption(const Data::ScenarioBuilder::Rules::Ptr& rules,
                               bool selected = false)
{
    wxString str = wxStringFromUTF8(rules->name());
    str << wxT("   "); // for beauty
    if (selected)
        str << wxT("(active)   ");
    return str;
}

inline const char* RulesetImage(bool selected = false)
{
    return (selected) ? "images/16x16/arrow_right.png" : "images/16x16/empty.png";
}

} // anonymous namespace

Panel::Panel(wxWindow* parent) : Antares::Component::Panel(parent)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Title
    {
        wxWindow* cp = new Component::CaptionPanel(this, wxT("Scenario Builder"));
        sizer->Add(cp, 0, wxALL | wxEXPAND);
    }

    Component::Panel* sub = new Component::Panel(this);
    wxBoxSizer* hz = new wxBoxSizer(wxHORIZONTAL);
    sub->SetSizer(hz);
    hz->AddSpacer(10);

    Component::Button* btn;

    btn = new Component::Button(sub, wxT("Building rules"), "images/16x16/open.png");
    btn->menu(true);
    btn->onPopupMenu(this, &Panel::onFileMenu);
    hz->Add(btn, 0, wxALL | wxEXPAND);
    pBtnFile = btn;

    Component::AddVerticalSeparator(sub, hz);

    hz->Add(Component::CreateLabel(sub, wxT("   Active :"), true),
            0,
            wxRIGHT | wxALIGN_CENTER_VERTICAL,
            4);
    btn = new Component::Button(sub, wxT("loading..."), "images/16x16/bullet.png");
    btn->menu(true);
    btn->onPopupMenu(this, &Panel::onActiveMenu);
    hz->Add(btn, 0, wxALL | wxEXPAND);
    pBtnActive = btn;

    sizer->Add(sub, 0, wxALL | wxEXPAND);
    sizer->Add(new wxStaticLine(this, wxID_ANY), 0, wxALL | wxEXPAND, 2);
    sizer->Layout();
    SetSizer(sizer);

    // There is no need to update at the creation. The controls are not visible
    // Dispatcher::GUI::Post(this, &Panel::update, 20);

    // External events
    OnStudyClosed.connect(this, &Panel::onStudyClosed);
    OnStudyChanged.connect(this, &Panel::onStudyChanged);
    OnStudyScenarioBuilderDataAreLoaded.connect(this, &Panel::scenarioBuilderDataAreLoaded);
}

Panel::~Panel()
{
    updateRules(nullptr);
    destroyBoundEvents();

    // To avoid corrupt vtable in some rare cases / misuses
    // (when children try to access to this object for example),
    // we should destroy all children as soon as possible.
    wxSizer* sizer = GetSizer();
    if (sizer)
        sizer->Clear(true);
}

void Panel::scenarioBuilderDataAreLoaded()
{
    pRules = nullptr;
    pActiveList.clear();
    pDeleteList.clear();
    pRenameList.clear();
    pBtnActive->caption(wxT("(loading...)"));
    pBtnFile->caption(wxT("(loading...)"));
    Refresh();
    Dispatcher::GUI::Post(this, &Panel::update, 20);
}

void Panel::update()
{
    if (not Data::Study::Current::Valid()) // should never happen here
        return;
    auto& study = *Data::Study::Current::Get();
    if (!&study or !study.scenarioRules)
    {
        // This may happen sometimes, especially at the creation of the component
        // for example (but this has been disabled)
        pBtnActive->caption(wxT("(none)"));
        pBtnFile->caption(wxT("No building rules"));
        return;
    }

    Data::ScenarioBuilder::Sets& sets = *study.scenarioRules;

    // Checking if our pointer to a ruleset is not deprecated
    if (!(!pRules))
    {
        Data::RulesScenarioName id = pRules->name();
        id.toLower();
        if (!sets.exists(id))
            pRules = nullptr;
    }

    // If the current ruleset is not defined, we will have to find the default one
    if (!pRules)
    {
        if (study.scenarioRules)
        {
            // We will try to find the ruleset by default that is active...
            Data::RulesScenarioName& active = study.parameters.activeRulesScenario;
            pRules = sets.find(active);
            if (!pRules and !sets.empty())
            {
                // ... otherwise we will take the first one
                Data::ScenarioBuilder::Sets::iterator i = sets.begin();
                pRules = i->second;
                active = i->first;
                MarkTheStudyAsModified();
            }
        }
    }

    // Refresh
    // Updating the caption
    if (!pRules)
        pBtnActive->caption(wxT("(none)"));
    else
        pBtnActive->caption(wxStringFromUTF8(pRules->name()));

    // Building rules
    if (sets.size() > 1)
        pBtnFile->caption(wxString() << sets.size() << wxT(" building rules"));
    else
        pBtnFile->caption(wxT("Building rules"));

    // Force the refresh
    Dispatcher::GUI::Refresh(this);
    // Broadcast the new
    updateRules(pRules);
}

void Panel::onStudyClosed()
{
    pRules = nullptr;
    updateRules(nullptr);
    pActiveList.clear();
    pDeleteList.clear();
    pRenameList.clear();
}

void Panel::onStudyChanged(Data::Study& study)
{
    pActiveList.clear();
    pDeleteList.clear();
    pRenameList.clear();

    if (!study.scenarioRules)
        pRules = nullptr;

    Dispatcher::GUI::Post(this, &Panel::update, 20);
}

void Panel::onFileMenu(Antares::Component::Button&, wxMenu& menu, void*)
{
    // New
    wxMenuItem* itNew = Menu::CreateItem(&menu,
                                         wxID_ANY,
                                         wxT("New ruleset     "),
                                         "images/16x16/new.png",
                                         wxEmptyString,
                                         wxITEM_NORMAL);
    menu.Connect(itNew->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Panel::onFileNew),
                 nullptr,
                 this);

    if (not Data::Study::Current::Valid())
    {
        logs.error() << "[scenario-builder] new ruleset: The data are not loaded";
        return;
    }

    auto& study = *Data::Study::Current::Get();
    if (!study.scenarioRules)
    {
        logs.error() << "[scenario-builder] new ruleset: The data are not loaded";
        return;
    }

    auto& sets = *study.scenarioRules;

    if (sets.empty())
        return;

    const uint rulesetCount = sets.size();

    wxString s;

    // Rename
    if (rulesetCount and !(!pRules))
    {
        s << wxT("Rename  '") << wxStringFromUTF8(pRules->name()) << wxT("'...    ");
        wxMenuItem* itRename = Menu::CreateItem(
          &menu, wxNewId(), s, "images/16x16/rename.png", wxEmptyString, wxITEM_NORMAL);
        menu.Connect(itRename->GetId(),
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(Panel::onFileRename),
                     nullptr,
                     this);
        pRenameList[itRename->GetId()] = pRules;
    }

    // Delete
    if (rulesetCount > 1 and !(!pRules))
    {
        s.clear();
        s << wxT("Delete  '") << wxStringFromUTF8(pRules->name()) << wxT("'...    ");
        wxMenuItem* itDelete = Menu::CreateItem(
          &menu, wxNewId(), s, "images/16x16/cancel.png", wxEmptyString, wxITEM_NORMAL);
        menu.Connect(itDelete->GetId(),
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(Panel::onFileDelete),
                     nullptr,
                     this);
        pDeleteList[itDelete->GetId()] = pRules;

        menu.AppendSeparator();
    }

    // Rename
    // wxMenu* renameSub = nullptr;
    // if (rulesetCount > 1)
    //{
    //	renameSub = new wxMenu();
    //	menu.AppendSubMenu(renameSub, wxT("Rename..."));
    //}

    // Delete
    // wxMenu* deleteSub = nullptr;
    // if (rulesetCount > 1)
    //{
    //	deleteSub = new wxMenu();
    //	menu.AppendSubMenu(deleteSub, wxT("Delete..."));
    //}

    if (rulesetCount > 1)
    {
        Data::ScenarioBuilder::Sets::iterator end = sets.end();
        wxMenuItem* it;

        it = Menu::CreateItem(&menu,
                              wxID_ANY,
                              wxT("Rename..."),
                              "images/16x16/rename.png",
                              wxEmptyString,
                              wxITEM_NORMAL);
        it->Enable(false);

        // Rename first
        for (Data::ScenarioBuilder::Sets::iterator i = sets.begin(); i != end; ++i)
        {
            // alias to the current item
            const Data::ScenarioBuilder::Rules::Ptr& rules = i->second;

            // Delete
            s.clear();
            s << wxT("Rename  '") << wxStringFromUTF8(rules->name()) << wxT("'...    ");
            wxMenuItem* itRename
              = Menu::CreateItem(&menu, wxNewId(), s, nullptr, wxEmptyString, wxITEM_NORMAL);
            menu.Connect(itRename->GetId(),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(Panel::onFileRename),
                         nullptr,
                         this);
            pRenameList[itRename->GetId()] = rules;
        }

        it = Menu::CreateItem(&menu,
                              wxID_ANY,
                              wxT("Delete..."),
                              "images/16x16/cancel.png",
                              wxEmptyString,
                              wxITEM_NORMAL);
        it->Enable(false);

        for (Data::ScenarioBuilder::Sets::iterator i = sets.begin(); i != end; ++i)
        {
            // alias to the current item
            const Data::ScenarioBuilder::Rules::Ptr& rules = i->second;

            // const bool selected = !(!pRules) and (pRules == rules);
            // const char* const img = RulesetImage(selected);

            // const wxString& str = RulesetCaption(rules, selected);

            // Delete
            s.clear();
            s << wxT("Delete  '") << wxStringFromUTF8(rules->name()) << wxT("'...    ");
            wxMenuItem* itDelete
              = Menu::CreateItem(&menu, wxNewId(), s, nullptr, wxEmptyString, wxITEM_NORMAL);
            menu.Connect(itDelete->GetId(),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(Panel::onFileDelete),
                         nullptr,
                         this);
            pDeleteList[itDelete->GetId()] = rules;

            // This code does not work on Windows :
            // (the ID retrieved are invalid)
            //
            // wxMenuItem* itSubDelete = Menu::CreateItem(deleteSub, wxNewId(), str, img,
            // wxEmptyString, 	wxITEM_NORMAL); deleteSub->Connect(itSubDelete->GetId(),
            // wxEVT_COMMAND_MENU_SELECTED, 	wxCommandEventHandler(Panel::onFileDelete), nullptr,
            // this);

            // Rename
            // wxMenuItem* itSubRename = Menu::CreateItem(renameSub, wxNewId(), str, img,
            // wxEmptyString, 	wxITEM_NORMAL); renameSub->Connect(itSubRename->GetId(),
            // wxEVT_COMMAND_MENU_SELECTED, 	wxCommandEventHandler(Panel::onFileRename), nullptr,
            // this);

            // pRenameList[itSubRename->GetId()] = rules;
            // pDeleteList[itSubDelete->GetId()] = rules;
        }
    }
}

void Panel::onActiveMenu(Antares::Component::Button&, wxMenu& menu, void*)
{
    if (not Data::Study::Current::Valid())
    {
        logs.error() << "[scenario-builder] new ruleset: The data are not loaded";
        return;
    }

    auto& study = *Data::Study::Current::Get();
    if (!study.scenarioRules)
    {
        logs.error() << "[scenario-builder] new ruleset: The data are not loaded";
        return;
    }

    Data::ScenarioBuilder::Sets& sets = *study.scenarioRules;

    if (sets.empty())
    {
        wxMenuItem* it = Menu::CreateItem(&menu,
                                          wxID_ANY,
                                          wxT("(No ruleset available)  "),
                                          "images/16x16/empty.png",
                                          wxEmptyString,
                                          wxITEM_NORMAL);
        it->Enable(false);
        return;
    }

    Data::ScenarioBuilder::Sets::iterator end = sets.end();
    Data::ScenarioBuilder::Sets::iterator i = sets.begin();
    for (; i != end; ++i)
    {
        // alias to the current item
        const Data::ScenarioBuilder::Rules::Ptr& rules = i->second;

        const bool selected = (pRules == rules);
        const char* const img = RulesetImage(selected);

        // Caption
        const wxString& str = RulesetCaption(rules, selected);

        // New
        wxMenuItem* it
          = Menu::CreateItem(&menu, wxID_ANY, str, img, wxEmptyString, wxITEM_NORMAL, selected);
        // Mapping
        pActiveList[it->GetId()] = rules;

        menu.Connect(it->GetId(),
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(Panel::onActiveRuleset),
                     nullptr,
                     this);
    }
}

void Panel::onFileNew(wxCommandEvent&)
{
    if (not Data::Study::Current::Valid())
    {
        logs.error() << "[scenario-builder] new ruleset: The data are not loaded";
        return;
    }

    auto& study = *Data::Study::Current::Get();
    if (!study.scenarioRules)
    {
        logs.error() << "[scenario-builder] new ruleset: The data are not loaded";
        return;
    }

    Data::ScenarioBuilder::Sets& sets = *study.scenarioRules;
    Data::RulesScenarioName& active = study.parameters.activeRulesScenario;

    // Finding a new name
    Data::RulesScenarioName newname = DEFAULT_RULESET_NAME;
    Data::RulesScenarioName id = newname;
    id.toLower();
    if (sets.exists(id))
    {
        Data::RulesScenarioName tmp;
        uint indx = 1;
        do
        {
            ++indx;
            tmp.clear() << newname << ' ' << indx;
            id = tmp;
            id.toLower();
            if (!sets.exists(id))
            {
                newname = tmp;
                break;
            }
        } while (true);
    }

    logs.info() << "[scenario-builder] Creating a new ruleset '" << newname << "'";
    pRules = sets.createNew(newname);
    if (!(!pRules))
    {
        active = pRules->name();
        active.toLower();
        MarkTheStudyAsModified();
    }

    Dispatcher::GUI::Post(this, &Panel::update, 20);
}

void Panel::onFileDelete(wxCommandEvent& evt)
{
    Data::ScenarioBuilder::Rules::Ptr rules = pDeleteList[evt.GetId()];
    if (!rules)
        return;

    if (not Data::Study::Current::Valid())
    {
        logs.error() << "[scenario-builder] delete: The data are not loaded";
        return;
    }

    auto& study = *Data::Study::Current::Get();
    if (!study.scenarioRules)
    {
        logs.error() << "[scenario-builder] delete: The data are not loaded";
        return;
    }

    Data::ScenarioBuilder::Sets& sets = *study.scenarioRules;
    if (sets.size() == 1)
        return;

    Data::RulesScenarioName& active = study.parameters.activeRulesScenario;

    Data::RulesScenarioName id = rules->name();
    id.toLower();

    {
        Window::Message message(
          this,
          wxT("Delete a ruleset"),
          wxEmptyString,
          wxString() << wxT("Do you really want to delete ") << wxStringFromUTF8(rules->name()),
          "images/misc/book.png");
        message.add(Window::Message::btnYes);
        message.add(Window::Message::btnCancel, true);
        if (message.showModal() != Window::Message::btnYes)
            return;
    }

    logs.info() << "[scenario-builder] deleting '" << rules->name() << "'";
    sets.remove(id);

    if (rules == pRules)
    {
        active.clear();
        pRules = nullptr;
    }

    MarkTheStudyAsModified();
    Dispatcher::GUI::Post(this, &Panel::update, 20);
}

void Panel::onFileRename(wxCommandEvent& evt)
{
    Data::ScenarioBuilder::Rules::Ptr rules = pRenameList[evt.GetId()];
    if (!rules)
        return;

    if (not Data::Study::Current::Valid())
    {
        logs.error() << "[scenario-builder] new ruleset: The data are not loaded";
        return;
    }

    auto& study = *Data::Study::Current::Get();
    if (!study.scenarioRules)
    {
        logs.error() << "[scenario-builder] new ruleset: The data are not loaded";
        return;
    }

    Data::ScenarioBuilder::Sets& sets = *study.scenarioRules;
    Data::RulesScenarioName& active = study.parameters.activeRulesScenario;

    Data::RulesScenarioName id = rules->name();
    id.toLower();

    Data::RulesScenarioName newname;
    do
    {
        wxTextEntryDialog dialog(this,
                                 wxT("Please enter the new name of the ruleset :"),
                                 wxT("Name"),
                                 wxStringFromUTF8(rules->name()),
                                 wxOK | wxCANCEL);

        if (dialog.ShowModal() != wxID_OK)
            return;
        wxStringToString(dialog.GetValue(), newname);
        newname.trim(" \t\r\n");
        if (!newname)
        {
            Window::Message message(
              this,
              wxT("Rename"),
              wxEmptyString,
              wxT("Impossible to rename the ruleset. The new name must not be empty."),
              "images/misc/book.png");
            message.add(Window::Message::btnContinue, true);
            message.showModal();
            continue;
        }
        if (newname.contains('[') or newname.contains(']') or newname.contains('$')
            or newname.contains('#') or newname.contains(','))
        {
            Window::Message message(this,
                                    wxT("Rename"),
                                    wxEmptyString,
                                    wxT("Impossible to rename the ruleset.\n\nThe name must not "
                                        "contain the following characters : , $ # [ ]"),
                                    "images/misc/book.png");
            message.add(Window::Message::btnContinue, true);
            message.showModal();
            continue;
        }

        Data::RulesScenarioName newid = newname;
        newid.toLower();
        if (newid == id)
            return;
        if (sets.exists(newid))
        {
            Window::Message message(this,
                                    wxT("Rename"),
                                    wxEmptyString,
                                    wxT("The name is already used."),
                                    "images/misc/book.png");
            message.add(Window::Message::btnContinue, true);
            message.showModal();
            continue;
        }
        break;
    } while (true);

    logs.info() << "[scenario-builder] renaming '" << rules->name() << "' into '" << newname << "'";
    Data::ScenarioBuilder::Rules::Ptr newrules = sets.rename(id, newname);
    if (!newrules)
    {
        Window::Message message(this,
                                wxT("Rename"),
                                wxEmptyString,
                                wxT("Impossible to rename the ruleset"),
                                "images/misc/book.png");
        message.add(Window::Message::btnContinue, true);
        message.showModal();
        return;
    }

    if (newrules == pRules)
        active = newrules->name();

    MarkTheStudyAsModified();
    Dispatcher::GUI::Post(this, &Panel::update, 20);
}

void Panel::onActiveRuleset(wxCommandEvent& evt)
{
    Data::ScenarioBuilder::Rules::Ptr rules = pActiveList[evt.GetId()];
    if (!rules)
        return;

    if (not Data::Study::Current::Valid())
    {
        logs.error() << "[scenario-builder] active: The data are not loaded";
        return;
    }

    auto& study = *Data::Study::Current::Get();
    if (!study.scenarioRules)
    {
        logs.error() << "[scenario-builder] active: The data are not loaded";
        return;
    }

    Data::RulesScenarioName& active = study.parameters.activeRulesScenario;
    active = rules->name();
    active.toLower();
    pRules = rules;
    MarkTheStudyAsModified();
    Dispatcher::GUI::Post(this, &Panel::update, 20);
}

} // namespace ScenarioBuilder
} // namespace Window
} // namespace Antares
