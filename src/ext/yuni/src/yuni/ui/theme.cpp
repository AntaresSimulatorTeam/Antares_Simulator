/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
# include "theme.h"

namespace Yuni
{
namespace UI
{

	Theme::Ptr Theme::sDefault = new Theme();

	Theme::Ptr Theme::sCurrent = nullptr;

	Theme::Theme():
		font(new FTFont("data/fonts/OpenSans-Light.ttf", 12)),
		monoFont(new FTFont("data/fonts/DejaVuSansMono.ttf", 10)),
		textColor(0.0f, 0.0f, 0.0f), // Opaque Black
 		windowColor(0.9882f, 0.9882f, 0.9882f), // (0xfcfcfc)
		buttonColor(0.2863f, 0.5451f, 0.651f), // (73, 139, 166)
		buttonColorDisabled(0.2863f, 0.5451f, 0.651f), // (103, 120, 139)
		menuColor(0.2863f, 0.5451f, 0.651f), // (73, 139, 166)
		menuColorDisabled(0.2863f, 0.5451f, 0.651f), // (103, 120, 139)
		borderColor(0.1882f, 0.3490f, 0.451f), // (48, 89, 115)
		borderColorDisabled(0.1882f, 0.3490f, 0.451f), // (102, 100, 126)
		panelColor(0.0f, 0.0f, 0.0f, 0.0f), // Full transparent
		margin(10),
		borderWidth(2.5f),
		menuHeight(20),
		checkBoxSize(15)
	{}


	Theme::Theme(const Theme& other):
		font(other.font),
		monoFont(other.monoFont),
		textColor(other.textColor),
 		windowColor(other.windowColor),
		buttonColor(other.buttonColor),
		buttonColorDisabled(other.buttonColorDisabled),
		menuColor(other.menuColor),
		menuColorDisabled(other.menuColorDisabled),
		borderColor(other.borderColor),
		borderColorDisabled(other.borderColorDisabled),
		panelColor(other.panelColor),
		margin(other.margin),
		borderWidth(other.borderWidth),
		menuHeight(other.menuHeight),
		checkBoxSize(other.checkBoxSize)
	{}


	void Theme::SwitchTo(const Theme::Ptr& newTheme)
	{
		if (!(!newTheme))
			sCurrent = newTheme;
	}


	void Theme::SwitchToDefault()
	{
		sCurrent = sDefault;
	}



} // namespace UI
} // namespace Yuni
