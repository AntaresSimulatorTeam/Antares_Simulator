// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_MESSAGE_HXX__
#define __ANTARES_APPLICATION_MESSAGE_HXX__

namespace Antares::Window
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

} // namespace Antares::Window

#endif // __ANTARES_APPLICATION_MESSAGE_HXX__
