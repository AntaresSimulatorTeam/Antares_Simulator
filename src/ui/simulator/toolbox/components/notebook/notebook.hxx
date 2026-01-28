// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_NOTEBOOK_HXX__
#define __ANTARES_TOOLBOX_COMPONENT_NOTEBOOK_HXX__

namespace Antares::Component
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

} // namespace Antares::Component

#endif // __ANTARES_TOOLBOX_COMPONENT_NOTEBOOK_HXX__
