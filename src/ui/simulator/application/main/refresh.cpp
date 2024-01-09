/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
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
**
** SPDX-License-Identifier: MPL-2.0
*/

#include "main.h"
#include "../study.h"
#include "../menus.h"
#include <antares/date/date.h>
#include <wx/menuitem.h>
#include "../../toolbox/components/refresh.h"
#include "../../../common/lock.h"
#include <yuni/datetime/timestamp.h>
#include "internal-ids.h"

using namespace Yuni;

namespace Antares
{
namespace Forms
{
struct CompareByStudyMode final
{
    bool operator()(const Data::StudyMode a, const Data::StudyMode b) const
    {
        return b < a;
    }
};

struct CompareByTimestamp final
{
    bool operator()(const int64_t a, const int64_t b) const
    {
        return b < a;
    }
};

template<class S>
static inline bool TimestampToString(S& out, int64_t timestamp)
{
    return DateTime::TimestampToString(out, "%A,%d %B %Y at %H:%M", timestamp);
}

template<class S>
static inline bool OutputTimestampToString(S& out, int64_t timestamp)
{
    if (DateTime::TimestampToString(out, "%d %b %Y (%a) %H:%M", timestamp))
    {
        out.toUpper();
        return true;
    }
    return false;
}

void ApplWnd::refreshMenuInput()
{
    if (not CurrentStudyIsValid() or IsGUIAboutToQuit())
    {
        pMenuInputCreation->SetItemLabel(wxT("No study"));
        pMenuInputLastSaved->SetItemLabel(wxT("Last save: never"));
        return;
    }

    auto& study = *GetCurrentStudy();

    String nowstr;
    if (not TimestampToString(nowstr, study.header.dateCreated))
        pMenuInputCreation->SetItemLabel(wxT("Created: unknown"));
    else
        pMenuInputCreation->SetItemLabel(wxString()
                                         << wxT("Created: ") << wxStringFromUTF8(nowstr));

    wxString s;
    s << wxT("Last saved: ");
    if (study.folder.empty())
        s << wxT("never");
    else
    {
        if (not TimestampToString(nowstr, study.header.dateLastSave))
            s << wxT("unknown");
        else
            s << wxStringFromUTF8(nowstr);
    }
    s << wxT("   "); // extra spaces for beauty
    pMenuInputLastSaved->SetItemLabel(s);
}

void ApplWnd::refreshMenuOutput()
{
    if (not pMenuOutput or not Forms::ApplWnd::Instance() or IsGUIAboutToQuit())
        return;

    // Cleanup
    Menu::Clear(*pMenuOutput);

    // NOTE : in some rare cases, it may happen that two simulations have the
    // same timestamp
    using StudyModeT = Data::StudyMode;
    using TemporalMap = std::map<int64_t, Data::Output::List, CompareByTimestamp>;
    using Map = std::map<StudyModeT, TemporalMap, CompareByStudyMode>;

    // Getting the list of all available outputs
    const Data::Output::List& list = ListOfOutputsForTheCurrentStudy;
    // The last study mode
    Data::StudyMode lastMode = Data::stdmUnknown;

    // Informations about the outputs
    Map map;
    TemporalMap orderByTime;
    uint lastTimestamp = 0;
    wxMenuItem* item;

    if (!list.empty())
    {
        const Data::Output::List::const_iterator end = list.end();
        for (Data::Output::List::const_iterator i = list.begin(); i != end; ++i)
        {
            orderByTime[(*i)->timestamp].push_back(*i);
            map[(*i)->mode][(*i)->timestamp].push_back(*i);
            if ((*i)->timestamp > lastTimestamp)
            {
                lastTimestamp = (*i)->timestamp;
                latestOutput = *i;
            }
        }
    }

    // Header
    if (list.empty())
    {
        item = Menu::CreateItem(pMenuOutput,
                                wxID_ANY,
                                wxT("No simulation results available"),
                                "images/16x16/empty.png",
                                wxEmptyString);
        item->Enable(false);
        // aborting. Nothing to do
        return;
    }

    // temporary buffer for converting a date
    String nowstr;
    enum
    {
        maxInMenu = 30,
    };

    uint total = 0;
    uint more = 0;

    for (auto mapB = orderByTime.cbegin(); mapB != orderByTime.cend(); ++mapB)
        total += (uint)mapB->second.size();

    if (!total)
        return;

    wxString howManyResults;
    switch (total)
    {
    case 1:
        howManyResults << wxT("1 result");
        break;
    default:
        if (total > maxInMenu)
            howManyResults << maxInMenu << wxT(" results  (out of ") << total << wxT(")");
        else
            howManyResults << total << wxT(" results");
        break;
    }

    item = Menu::CreateItem(
      pMenuOutput, wxID_ANY, howManyResults, "images/16x16/empty.png", wxEmptyString);
    item->Enable(false);

    for (auto mapB = orderByTime.cbegin(); mapB != orderByTime.cend(); ++mapB)
    {
        bool shouldStop = false;
        for (auto i = mapB->second.begin(); i != mapB->second.end(); ++i)
        {
            if (not OutputTimestampToString(nowstr, (*i)->timestamp))
                continue;

            // The output
            wxString s;
            s << wxStringFromUTF8(nowstr);

            if (not(*i)->name.empty())
                s << wxT(" : ") << wxStringFromUTF8((*i)->name);

            if (System::unix)
                s << wxT("  ");

            auto* it = Menu::CreateItem(
              pMenuOutput,
              wxID_ANY,
              s,
#ifndef YUNI_OS_WINDOWS
              (!more ? "images/16x16/minibullet_sel.png" : "images/16x16/minibullet.png"),
#else
              (((*i)->mode == Data::stdmEconomy) ? "images/misc/economy.png"
                                                 : "images/misc/adequacy.png"),
#endif
              s,
              wxITEM_NORMAL);

            (*i)->viewMenuID = it->GetId();
            Forms::ApplWnd::Instance()->Connect(
              it->GetId(),
              wxEVT_COMMAND_MENU_SELECTED,
              wxCommandEventHandler(Forms::ApplWnd::evtOnViewOutput),
              nullptr,
              Forms::ApplWnd::Instance());

            if (++more >= maxInMenu)
            {
                if (total > more)
                {
                    shouldStop = true;
                    break;
                }
            }
        }
        if (shouldStop)
            break;
    }

    pMenuOutput->AppendSeparator();

    if (System::windows)
        item = pMenuOutput->AppendSubMenu(new wxMenu(), wxT("Open in Windows Explorer..."));
    else
        item = pMenuOutput->AppendSubMenu(new wxMenu(), wxT("Open in file explorer..."));

    // The current total number of item for the current category
    total = 0;
    more = 0;
    wxMenu* menu = item->GetSubMenu();
    wxWindow* parentForm = Forms::ApplWnd::Instance();

    for (auto mapA = map.begin(); mapA != map.end(); ++mapA)
    {
        for (auto mapB = mapA->second.begin(); mapB != mapA->second.end(); ++mapB)
        {
            for (auto i = mapB->second.cbegin(); i != mapB->second.cend(); ++i)
            {
                // Separating into groups
                if (lastMode != (*i)->mode)
                {
                    if (more)
                    {
                        // AppendTooManyItems(menu, more);
                        more = 0;
                    }
                    // if (lastMode != Data::stdmUnknown)
                    //	Menu::CreateEmptyItem(menu);

                    lastMode = (*i)->mode;
                    if (lastMode == Data::stdmUnknown)
                        lastMode = Data::stdmEconomy;
                    Menu::CreateGroupItem(menu,
                                          wxStringFromUTF8(StudyModeToCString(lastMode)),
                                          "images/16x16/empty.png");
                    total = 0;
                }

                if (++total > maxInMenu) // arbitrary value
                {
                    ++more;
                }
                else
                {
                    if (not OutputTimestampToString(nowstr, (*i)->timestamp))
                        continue;

                    // The output
                    wxString s;
                    s << wxStringFromUTF8(nowstr);
                    if (not(*i)->name.empty())
                        s << wxT(", ") << wxStringFromUTF8((*i)->name);
                    if (System::unix)
                        s << wxT("  ");

                    auto* it = Menu::CreateItem(menu,
                                                wxID_ANY,
                                                s,
#ifndef YUNI_OS_WINDOWS
                                                "images/16x16/minibullet.png",
#else
                                                (((*i)->mode == Data::stdmEconomy)
                                                   ? "images/misc/economy.png"
                                                   : "images/misc/adequacy.png"),
#endif
                                                wxString(wxT("Open the folder "))
                                                  << wxStringFromUTF8((*i)->path));

                    (*i)->menuID = it->GetId();
                    parentForm->Connect(
                      it->GetId(),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(Forms::ApplWnd::evtOnOpenOutputInExplorer),
                      NULL,
                      parentForm);
                }
            }
        }
    }
}

void ApplWnd::refreshMenuOptions(Data::Study::Ptr study)
{
    if (not CurrentStudyIsValid() or IsGUIAboutToQuit())
        return;

    // Disabling the Configure menu's scenario builder item after loading a study
    // when building mode is not Custom.

    auto* menu = GetMenuBar();
    auto* sc_builder_menu_item
      = menu->FindItem(Antares::Forms::mnIDOptionConfigureMCScenarioBuilder);
    if (not sc_builder_menu_item)
        return;

    if (not study->parameters.useCustomScenario)
        sc_builder_menu_item->Enable(false);
}

void ApplWnd::forceRefresh()
{
    if (not IsGUIAboutToQuit())
    {
        auto* evtHandler = GetEventHandler();
        if (evtHandler)
        {
            wxSizeEvent e(GetClientSize());
            evtHandler->ProcessEvent(e);
        }
        RefreshAllControls(this);
    }
}

void ApplWnd::forceFocus()
{
    // Set the focus to the main form
    SetFocus();

    // We must assign the focus to the control displayed by the main notebook
    // as well, otherwise shortcuts may not work properly for the map
    // The method `forceRefresh` will re-assign the focus to its child
    pNotebook->forceRefresh();

    // Force the refresh of the main form
    forceRefresh();
}

void ApplWnd::delayForceFocus()
{
    Dispatcher::GUI::Post(this, &ApplWnd::forceFocus, 50 /*ms*/);
}

} // namespace Forms
} // namespace Antares
