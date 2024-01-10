/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
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
*/
#ifndef __ANTARES_TOOLBOX_COMPONENT_NOTEBOOK_HXX__
#define __ANTARES_TOOLBOX_COMPONENT_NOTEBOOK_HXX__

namespace Antares
{
namespace Component
{
inline Notebook& Notebook::Page::notebook()
{
    return pNotebook;
}

inline const Notebook& Notebook::Page::notebook() const
{
    return pNotebook;
}

inline const wxString& Notebook::Page::caption() const
{
    return pCaption;
}

inline const wxString& Notebook::Page::name() const
{
    return pName;
}

inline const wxString& Notebook::caption() const
{
    return pCaption;
}

inline Notebook::Tabs::~Tabs()
{
}

inline void Notebook::addSeparator()
{
    new Page(*this, NULL, wxEmptyString);
}

inline void Notebook::Page::name(const wxString& s)
{
    pName = s;
}

inline void Notebook::Page::visible(const bool v)
{
    pVisible = v;
    pNotebook.Refresh();
}

inline Notebook::Page* Notebook::add(wxWindow* ctnrl, const wxString& caption)
{
    return new Page(*this, ctnrl, caption);
}

inline Notebook::Page* Notebook::add(wxWindow* ctnrl, const wxString& name, const wxString& caption)
{
    return new Page(*this, ctnrl, name, caption);
}

inline bool Notebook::alwaysDisplayTabs() const
{
    return pAlwaysDisplayTab;
}

inline void Notebook::alwaysDisplayTabs(const bool v)
{
    pAlwaysDisplayTab = v;
}

inline bool Notebook::tabsVisible() const
{
    return pTabsVisible;
}

inline void Notebook::tabsVisible(bool v)
{
    pTabsVisible = v;
}

inline bool Notebook::displayTitle() const
{
    return pDisplayTitle;
}

inline void Notebook::displayTitle(const bool v)
{
    pDisplayTitle = v;
}

inline Notebook::Orientation Notebook::orientation() const
{
    return pOrientation;
}

inline wxWindow* Notebook::titlePanelControl() const
{
    return pTabs;
}

inline bool Notebook::HasMultiplePages() const
{
    return true;
}

} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_NOTEBOOK_HXX__
