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
#ifndef __YUNI_UI_THEME_H__
# define __YUNI_UI_THEME_H__

# include "../yuni.h"
# include "../core/color/rgba.h"
# include "../core/smartptr.h"
# include "ftfont.h"


namespace Yuni
{
namespace UI
{


	//! The UI theme describes all colors, margins, fonts, ... for displaying UI
	class Theme
	{
	public:
		//! Smart pointer
		typedef SmartPtr<Theme>  Ptr;

	public:
		//! Get the current theme
		static Theme::Ptr Current();
		//! Get the default theme
		static Theme::Ptr Default();
		//! Switch to another theme
		static void SwitchTo(const Theme::Ptr& newTheme);
		//! Switch back to default theme
		static void SwitchToDefault();

	public:
		//! Empty constructor
		Theme();

		//! Copy constructor
		Theme(const Theme& other);

		//! Font
		FTFont::Ptr font;
		FTFont::Ptr monoFont;

		//! Color for text
		Color::RGBA<float> textColor;
		//! Color for window background
		Color::RGBA<float> windowColor;
		//! Color for button background
		Color::RGBA<float> buttonColor;
		Color::RGBA<float> buttonColorDisabled;
		//! Color for window menus
		Color::RGBA<float> menuColor;
		Color::RGBA<float> menuColorDisabled;
		//! Color for control borders
		Color::RGBA<float> borderColor;
		Color::RGBA<float> borderColorDisabled;
		//! Color for panel background
		Color::RGBA<float> panelColor;
		//! Margin for automatic placement of child controls (in pixels)
		float margin;
		//! Width of control borders (in pixels)
		float borderWidth;
		//! Height of window menus (in pixels)
		float menuHeight;
		//! Checkbox size (both width and height, checkboxes are squares)
		float checkBoxSize;

	private:
		static Theme::Ptr sCurrent;
		static Theme::Ptr sDefault;
		static FTFont::Ptr sDefaultFont;
		static FTFont::Ptr sDefaultMonoFont;

	}; // class Theme



} // namespace UI
} // namespace Yuni

# include "theme.hxx"

#endif // __YUNI_UI_THEME_H__
