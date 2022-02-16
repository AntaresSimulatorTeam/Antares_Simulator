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

#include <yuni/yuni.h>
#include <antares/logs.h>
#include "menus.h"
#include <wx/image.h>
#include "../toolbox/resources.h"
#include "recentfiles.h"
#include "main.h"
#include "main/internal-ids.h"

using namespace Yuni;

namespace Antares
{
//! Temporary string for bitmap path location
static String gUIbmpLocation;

static void InternalRebuildRecentFiles(wxMenu* it,
                                       const RecentFiles::ListPtr& lst,
                                       bool addOpenMenu = true)
{
    if (!it)
        return;

    // Keeping a reference on the submenu
    wxMenu* subMenu = it;
    // Remove all previous entries
    Menu::Clear(*subMenu);

    if (addOpenMenu)
    {
        // Adding the item in the menu
        Menu::CreateItem(subMenu,
                         Forms::mnIDOpen,
                         wxT("&Open from a folder...   \tCtrl+O"),
                         "images/16x16/open.png",
                         wxEmptyString,
                         wxITEM_NORMAL,
                         true);

        // A tiny separator
        subMenu->AppendSeparator();
    }

    if (not lst->empty())
    {
        // Show path of the studies ?
        const bool showPaths = RecentFiles::ShowPathInMenu();

        int indx = 1;
        wxString mnCaption;
        RecentFiles::List::const_iterator end = lst->end();
        for (RecentFiles::List::const_iterator i = lst->begin(); i != end; ++i)
        {
            mnCaption.clear();
            if (!i->second.IsEmpty())
                mnCaption << i->second;
            if (showPaths)
                mnCaption << wxT("  -  ") << i->first;

            // Adding the item in the menu
            Menu::CreateItem(subMenu,
                             Forms::mnIDOpenRecents_0 + (!indx ? 10 : indx) - 1,
#ifndef YUNI_OS_WINDOWS
                             mnCaption,
                             "images/16x16/minibullet.png",
#else
                             mnCaption,
                             "images/misc/study.png",
#endif
                             wxString() << wxT("Open the recent study `") << i->second << wxT("`"));

            if (++indx > 9)
                indx = 0;
        }

        // Settings
        subMenu->AppendSeparator();
        // Path to the study
        wxMenuItem* i = Menu::CreateItem(subMenu,
                                         Forms::mnIDOpenRecents_ToggleShowPaths,
                                         wxT("Show the path of the studies"),
                                         nullptr,
                                         wxEmptyString,
                                         wxITEM_CHECK);
        i->Check(showPaths);

        Menu::CreateItem(subMenu,
                         Forms::mnIDOpenRecents_ClearHistory,
                         wxT("Clear the history"),
                         nullptr,
                         wxEmptyString);
    }
    else
    {
        auto* menuitem = Menu::CreateItem(
          subMenu, wxID_ANY, wxT("(No recent files available)"), nullptr, wxEmptyString);
        menuitem->Enable(false);
    }
    it->UpdateUI();
}

void Menu::CreateEmptyItem(wxMenu* parent)
{
    auto* m = new wxMenuItem(parent, wxID_ANY, wxT(" "), wxEmptyString, wxITEM_NORMAL);
    parent->Append(m);
    m->Enable(false);
}

void Menu::CreateGroupItem(wxMenu* parent, const wxString& caption, const char* bitmap)
{
    auto* m = new wxMenuItem(parent, wxID_ANY, caption, wxEmptyString, wxITEM_NORMAL);
    if (bitmap)
    {
        if (Resources::FindFile(gUIbmpLocation, bitmap))
        {
            wxImage img(wxStringFromUTF8(gUIbmpLocation));
#ifdef YUNI_OS_WINDOWS
            m->SetMarginWidth(img.GetWidth());
#endif
            m->SetBitmap(wxBitmap(img));
        }
    }
    parent->Append(m);
    m->Enable(false);
}

wxMenuItem* Menu::CreateItem(wxMenu* parent,
                             const int id,
                             const wxString& caption,
                             const char* bitmap,
                             const wxString& help,
                             wxItemKind kind,
                             const bool bold)
{
    assert(parent && "invalid parent handle");

    wxString newcaption;
    newcaption << wxT(' ') << caption;
#ifndef YUNI_OS_WINDOWS
    newcaption << wxT("  ");
#endif

    auto* m = (help.empty())
                ? new wxMenuItem(parent, id, newcaption, wxEmptyString, kind)
                : new wxMenuItem(parent, id, newcaption, wxString(wxT("  ")) << help, kind);

#ifdef YUNI_OS_WINDOWS
    if (bold)
    {
        wxFont font = m->GetFont();
        font.SetWeight(wxFONTWEIGHT_BOLD);
        m->SetFont(font);
    }
#else
    (void)bold;
#endif

    if (bitmap && *bitmap != '\0')
    {
        if (Resources::FindFile(gUIbmpLocation, bitmap))
        {
            wxImage img(wxStringFromUTF8(gUIbmpLocation));
#ifdef YUNI_OS_WINDOWS
            m->SetMarginWidth(img.GetWidth());
#endif
            m->SetBitmap(wxBitmap(img));
        }
    }
    parent->Append(m);

    return m;
}

wxMenuItem* Menu::CreateItemWithSubMenu(wxMenu* parent,
                                        const int id,
                                        const wxString& caption,
                                        const char* bitmap,
                                        const wxString& help,
                                        const bool bold)
{
    assert(parent && "invalid parent handle");

    wxString newcaption;
    newcaption << wxT(' ') << caption;
#ifndef YUNI_OS_WINDOWS
    newcaption << wxT("  ");
#endif

    auto* m = (help.empty()) ? new wxMenuItem(parent, id, newcaption, wxEmptyString, wxITEM_NORMAL)
                             : new wxMenuItem(
                               parent, id, newcaption, wxString(wxT("  ")) << help, wxITEM_NORMAL);

#ifdef YUNI_OS_WINDOWS
    if (bold)
    {
        wxFont font = m->GetFont();
        font.SetWeight(wxFONTWEIGHT_BOLD);
        m->SetFont(font);
    }
#else
    (void)bold;
#endif

    if (bitmap && *bitmap != '\0')
    {
        if (Resources::FindFile(gUIbmpLocation, bitmap))
        {
            wxImage img(wxStringFromUTF8(gUIbmpLocation));
#ifdef YUNI_OS_WINDOWS
            m->SetMarginWidth(img.GetWidth());
#endif
            m->SetBitmap(wxBitmap(img));
        }
    }

    m->SetSubMenu(new wxMenu());
    parent->Append(m);

    return m;
}

wxMenuItem* Menu::CreateItem(wxMenu* parent,
                             const int id,
                             const wxString& caption,
                             const wxBitmap& bitmap,
                             const wxString& help,
                             wxItemKind kind,
                             const bool bold)
{
    assert(parent && "invalid parent handle");

    wxString newcaption;
    newcaption << wxT(' ') << caption;
#ifndef YUNI_OS_WINDOWS
    newcaption << wxT("  ");
#endif

    auto* m = (help.empty())
                ? new wxMenuItem(parent, id, newcaption, wxEmptyString, kind)
                : new wxMenuItem(parent, id, newcaption, wxString(wxT("  ")) << help, kind);

#ifdef YUNI_OS_WINDOWS
    if (bold)
    {
        wxFont font = m->GetFont();
        font.SetWeight(wxFONTWEIGHT_BOLD);
        m->SetFont(font);
    }
#else
    (void)bold;
#endif

#ifdef YUNI_OS_WINDOWS
        m->SetMarginWidth(bitmap.GetWidth());
#endif
        m->SetBitmap(bitmap);

    parent->Append(m);

    return m;
}

void Menu::Clear(wxMenu& menu)
{
    if (menu.GetMenuItemCount() > 0)
    {
        wxMenuItemList list = menu.GetMenuItems();
        auto end = list.end();
        for (auto i = list.begin(); i != end; ++i)
            menu.Destroy(*i);
    }
}

wxMenuItem* Menu::AddRecentFile(wxMenuItem* it, const wxString& title, const wxString& path)
{
    const RecentFiles::ListPtr lst = RecentFiles::AddAndGet(path, title);
    InternalRebuildRecentFiles(it->GetSubMenu(), lst, false);
    it->Enable(!lst->empty());
    return it;
}

wxMenuItem* Menu::RebuildRecentFiles(wxMenuItem* it, bool addOpenMenu)
{
    const RecentFiles::ListPtr lst = RecentFiles::Get();
    InternalRebuildRecentFiles(it->GetSubMenu(), lst, addOpenMenu);
    it->Enable(!lst->empty());
    return it;
}

wxMenu* Menu::RebuildRecentFiles(wxMenu* it, bool addOpenMenu)
{
    const RecentFiles::ListPtr lst = RecentFiles::Get();
    InternalRebuildRecentFiles(it, lst, addOpenMenu);
    return it;
}

} // namespace Antares
