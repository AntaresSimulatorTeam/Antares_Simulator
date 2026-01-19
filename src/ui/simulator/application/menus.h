// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_FORMS_MENU_H__
#define __ANTARES_FORMS_MENU_H__

#include <yuni/yuni.h>
#include "../toolbox/wx-wrapper.h"
#include <wx/menu.h>
#include <wx/menuitem.h>

class wxBitmap;

namespace Antares
{
class Menu
{
public:
    static void Clear(wxMenu& it);

    static void CreateEmptyItem(wxMenu* parent);

    static void CreateGroupItem(wxMenu* parent,
                                const wxString& caption,
                                const char* bitmap = "images/16x16/empty.png");

    static wxMenuItem* CreateItem(wxMenu* parent,
                                  const int id,
                                  const wxString& caption,
                                  const char* bitmap = NULL,
                                  const wxString& help = wxEmptyString,
                                  wxItemKind kind = wxITEM_NORMAL,
                                  const bool bold = false);

    static wxMenuItem* CreateItem(wxMenu* parent,
                                  const int id,
                                  const wxString& caption,
                                  const wxBitmap& bitmap,
                                  const wxString& help = wxEmptyString,
                                  wxItemKind kind = wxITEM_NORMAL,
                                  const bool bold = false);

    static wxMenuItem* CreateItemWithSubMenu(wxMenu* parent,
                                             const int id,
                                             const wxString& caption,
                                             const char* bitmap,
                                             const wxString& help = wxEmptyString,
                                             const bool bold = false);

    /*!
    ** \brief Rebuild the menu item about recents opened files
    **
    ** \param it An existing menuitem to fill
    ** \return Always it
    */
    static wxMenuItem* RebuildRecentFiles(wxMenuItem* it, bool addOpenMenu = false);
    static wxMenu* RebuildRecentFiles(wxMenu* it, bool addOpenMenu = false);

    static wxMenuItem* AddRecentFile(wxMenuItem* it, const wxString& title, const wxString& path);
};

} // namespace Antares

#endif // __ANTARES_FORMS_MENU_H__
