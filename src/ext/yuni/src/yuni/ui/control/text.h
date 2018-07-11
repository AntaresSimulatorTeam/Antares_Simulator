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
#ifndef __YUNI_UI_CONTROL_TEXT_H__
# define __YUNI_UI_CONTROL_TEXT_H__

# include "../../yuni.h"
# include "../../core/color/rgb.h"
# include "../../core/color/rgba.h"
# include "control.h"
# include "../displaymode.h"
# include "../font.h"
# include "../theme.h"

namespace Yuni
{
namespace UI
{
namespace Control
{


	//! A text is a control that displays text on the parent surface
	class Text: public IControl
	{
	public:
		//! Smart pointer
		typedef Ancestor::SmartPtrType<Text>::Ptr  Ptr;

	public:
		Text(float x, float y, float maxWidth, float maxHeight):
			IControl(x, y, maxWidth, maxHeight),
			pText(),
			pFont(Theme::Current()->font),
			// White by default
			pColor(Theme::Current()->textColor),
			pAntiAliased(true)
		{}

		Text(const Point2D<float>& position, const Point2D<float>& maxSize):
			IControl(position, maxSize),
			pText(),
			pFont(Theme::Current()->font),
			pColor(Theme::Current()->textColor),
			pAntiAliased(true)
		{}

		//! Virtual destructor
		virtual ~Text() {}

		//! Draw the panel
		virtual void draw(DrawingSurface::Ptr& surface, float xOffset, float yOffset) const override;

		//! Clear the text
		String& clear() { invalidate(); return pText.clear(); }

		//! Get the text
		String& text() { invalidate(); return pText; }
		const String& text() const { return pText; }

		//! Modify the font used
		void font(const UI::FTFont::Ptr& font) { if (pFont != font) { pFont = font; invalidate(); } }
		//! Get the font
		const UI::FTFont::Ptr& font() const { return pFont; }

		//! Modify the color used
		void color(float r, float g, float b) { pColor.assign(r, g, b); invalidate(); }
		void color(float r, float g, float b, float a) { pColor.assign(r, g, b, a); invalidate(); }
		void color(const Color::RGB<float>& color) { pColor = color; invalidate(); }
		void color(const Color::RGBA<float>& color) { pColor = color; invalidate(); }

		//! Is the text anti-aliased ?
		bool antiAliased() const { return pAntiAliased; }
		//! Set text anti-aliasing
		void antiAliased(bool newValue) { pAntiAliased = newValue; invalidate(); }

	private:
		//! Text to display
		String pText;

		//! Font to use
		FTFont::Ptr pFont;

		//! Text color
		Color::RGBA<float> pColor;

		//! Anti-alias the text ?
		bool pAntiAliased;

	}; // class Text



} // namespace Control
} // namespace UI
} // namespace Yuni

#endif // __YUNI_UI_CONTROL_TEXT_H__
