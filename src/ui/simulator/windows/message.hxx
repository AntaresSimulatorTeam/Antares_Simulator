/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
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
