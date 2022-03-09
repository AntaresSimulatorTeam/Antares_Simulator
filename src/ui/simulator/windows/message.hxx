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
#ifndef __ANTARES_APPLICATION_MESSAGE_HXX__
#define __ANTARES_APPLICATION_MESSAGE_HXX__

namespace Antares
{
namespace Window
{
inline void Message::add(DefaultButtonType btn, bool defaultButton, int space)
{
    switch (btn)
    {
    case btnCancel:
        add(wxT("Cancel"), btn, defaultButton, space);
        break;
    case btnOk:
        add(wxT("  Ok  "), btn, defaultButton, space);
        break;
    case btnYes:
        add(wxT("  Yes  "), btn, defaultButton, space);
        break;
    case btnNo:
        add(wxT("  No  "), btn, defaultButton, space);
        break;
    case btnRetry:
        add(wxT("  Retry  "), btn, defaultButton, space);
        break;
    case btnSaveChanges:
        add(wxT("Save changes"), btn, defaultButton, space);
        break;
    case btnQuitWithoutSaving:
        add(wxT("Quit without saving"), btn, defaultButton, space);
        break;
    case btnDiscard:
        add(wxT("Discard"), btn, defaultButton, space);
        break;
    case btnSaveAs:
        add(wxT("Save as..."), btn, defaultButton, space);
        break;
    case btnUpgrade:
        add(wxT("Upgrade"), btn, defaultButton, space);
        break;
    case btnContinue:
        add(wxT("Continue"), btn, defaultButton, space);
        break;
    case btnViewResults:
        add(wxT("View results"), btn, defaultButton, space);
        break;
    case btnQuit:
        add(wxT("Quit the program"), btn, defaultButton, space);
        break;
    case btnStartID:
        break;
    }
}

inline void Message::add(Component::Spotlight::IItem::Ptr item)
{
    pItemList.push_back(item);
}

inline Message& Message::operator+=(Component::Spotlight::IItem::Ptr item)
{
    pItemList.push_back(item);
    return *this;
}

inline void Message::recommendedWidth(uint w)
{
    pRecommendedWidth = w;
}

} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_MESSAGE_HXX__
