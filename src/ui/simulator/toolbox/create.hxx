// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_CREATE_HXX__
#define __ANTARES_TOOLBOX_CREATE_HXX__

#include <yuni/core/bind.h>

namespace Antares::Private::Component
{
class CustomWxButton: public wxButton
{
public:
    /*!
    ** \brief Constructor
    */
    CustomWxButton(wxWindow* parent, const wxString& title);

    //! Destructor
    virtual ~CustomWxButton()
    {
    }

public:
    //!
    Yuni::Bind<void()> onUserClick;

private:
    void evtOnUserClick(wxCommandEvent&);

}; // class CustomWxButton

} // namespace Antares::Private::Component

namespace Antares::Component
{
template<class T, class StringT, class UserDataT>
wxButton* CreateButton(wxWindow* parent,
                       const StringT& caption,
                       T* object,
                       void (T::*method)(UserDataT),
                       UserDataT userdata)
{
    // type alias
    using ButtonType = ::Antares::Private::Component::CustomWxButton;
    // Title of the button
    wxString title;
    title << wxT("   ") << caption << wxT("   ");

    // Creation
    auto* button = new ButtonType(parent, title);

    // Event
    if (object)
    {
        using MemberType = void (T::*)(UserDataT);
        button->onUserClick.bind(const_cast<T*>(object),
                                 reinterpret_cast<MemberType>(method),
                                 userdata);
    }
    return button;
}

} // namespace Antares::Component

#endif // __ANTARES_TOOLBOX_CREATE_HXX__
