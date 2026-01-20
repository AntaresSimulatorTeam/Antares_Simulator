// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENTS_BUTTON_BUTTON_HXX__
#define __ANTARES_TOOLBOX_COMPONENTS_BUTTON_BUTTON_HXX__

namespace Antares::Component
{
template<class T>
Button::Button(wxWindow* parent,
               const wxString& caption,
               const char* bitmap,
               T* object,
               void (T::*method)(void*)):
    Panel(parent),
    pCaption(caption),
    pUserData(NULL),
    pIcon(NULL),
    pClickBehavior(clkDefault),
    pRecommendedWidth(0),
    pMiddleWidth(0),
    pSelected(false),
    pPushed(false),
    pAutoToggle(false),
    pBold(false),
    pHover(true)
{
    // callback
    using MethodType = void (T::*)(void*);
    if (object)
    {
        pOnClick.bind(const_cast<T*>(object), reinterpret_cast<MethodType>(method));
    }

    SetBackgroundStyle(wxBG_STYLE_CUSTOM); // Required by both GTK and Windows
    SetBackgroundColour(parent->GetBackgroundColour());
    loadIconFromResource(bitmap);
    precalculateCoordinates();
}

template<class T>
inline void Button::onPopupMenu(const T* object, void (T::*method)(Button&, wxMenu&, void*))
{
    if (object)
    {
        using MethodType = void (T::*)(Button&, wxMenu&, void*);
        pOnPopup.bind(const_cast<T*>(object), reinterpret_cast<MethodType>(method));
    }
    else
    {
        pOnPopup.clear();
    }
}

inline void Button::onPopupMenu(const OnPopupMenu& popup)
{
    pOnPopup = popup;
}

template<class T>
inline void Button::onClick(const T* object, void (T::*method)(void*))
{
    if (object)
    {
        using MethodType = void (T::*)(void*);
        pOnClick.bind(const_cast<T*>(object), reinterpret_cast<MethodType>(method));
    }
    else
    {
        pOnClick.clear();
    }
}

inline void Button::disconnectClickEvent()
{
    pOnClick.clear();
}

inline const wxString& Button::caption() const
{
    return pCaption;
}

inline void* Button::userdata() const
{
    return pUserData;
}

inline void Button::userdata(void* v)
{
    pUserData = v;
}

inline bool Button::dropDown() const
{
    return (pClickBehavior == clkDropdown);
}

inline void Button::dropDown(bool rhs)
{
    pClickBehavior = (rhs) ? clkDropdown : clkDefault;
    precalculateCoordinates();
    Refresh();
}

inline bool Button::menu() const
{
    return (pClickBehavior == clkDropdown);
}

inline void Button::menu(bool rhs)
{
    pClickBehavior = (rhs) ? clkMenu : clkDefault;
    precalculateCoordinates();
    Refresh();
}

inline bool Button::pushed() const
{
    return pPushed;
}

inline void Button::pushed(bool v)
{
    if (v != pPushed)
    {
        pPushed = v;
        Refresh();
    }
}

inline bool Button::autoToggle() const
{
    return pAutoToggle;
}

inline void Button::autoToggle(bool v)
{
    pAutoToggle = v;
}

inline void Button::image(const char* filename)
{
    loadIconFromResource(filename);
    precalculateCoordinates();
}

inline void Button::pushedColor(uint r, uint g, uint b)
{
    pColorOverridePushed.Set((unsigned char)r, (unsigned char)g, (unsigned char)b);
}

inline void Button::pushedColor(const wxColour& color)
{
    pColorOverridePushed = color;
}

inline void Button::pushedColor(const wxWindow* wnd)
{
    assert(wnd && "invalid window");
    if (wnd)
    {
        pColorOverridePushed = wnd->GetBackgroundColour();
    }
}

inline bool Button::bold() const
{
    return pBold;
}

inline void Button::bold(bool value)
{
    pBold = value;
}

inline bool Button::enabled() const
{
    return IsEnabled();
}

inline void Button::enabled(bool value)
{
    Enable(value);
}

inline bool Button::hover() const
{
    return pHover;
}

inline void Button::hover(bool value)
{
    pHover = value;
}

} // namespace Antares::Component

#endif // __ANTARES_TOOLBOX_COMPONENTS_BUTTON_BUTTON_HXX__
