/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
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
#ifndef __ANTARES_TOOLBOX_CREATE_HXX__
#define __ANTARES_TOOLBOX_CREATE_HXX__

#include <yuni/core/bind.h>

namespace Antares
{
namespace Private
{
namespace Component
{
class CustomWxButton : public wxButton
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

} // namespace Component
} // namespace Private
} // namespace Antares

namespace Antares
{
namespace Component
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
        button->onUserClick.bind(
          const_cast<T*>(object), reinterpret_cast<MemberType>(method), userdata);
    }
    return button;
}

} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_CREATE_HXX__
