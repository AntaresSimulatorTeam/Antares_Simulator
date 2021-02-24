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
template<class T, class StringT>
wxButton* CreateButton(wxWindow* parent,
                       const StringT& caption,
                       T* object,
                       void (T::*method)(void*),
                       void* userdata)
{
    // type alias
    typedef ::Antares::Private::Component::CustomWxButton ButtonType;
    // Title of the button
    wxString title;
    title << wxT("   ") << caption << wxT("   ");

    // Creation
    auto* button = new ButtonType(parent, title);

    // Event
    if (object)
    {
        typedef void (T::*MemberType)(void*);
        button->onUserClick.bind(
          const_cast<T*>(object), reinterpret_cast<MemberType>(method), userdata);
    }
    return button;
}

} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_CREATE_HXX__
