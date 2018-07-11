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
#ifndef __YUNI_UI_CONTROL_BUTTON_H__
# define __YUNI_UI_CONTROL_BUTTON_H__

# include "../../yuni.h"
# include "control.h"
# include "../font.h"
# include "../displaymode.h"
# include "../theme.h"

namespace Yuni
{
namespace UI
{
namespace Control
{


	//! A button is a clickable control that triggers an event
	class Button: public IControl
	{
	public:
		//! Smart pointer
		typedef Ancestor::SmartPtrType<Button>::Ptr  Ptr;
		//! Type of bind for the button callback
		typedef Yuni::Bind<EventPropagation (IControl*)>  Callback;

	public:
		//! Callback for button click
		Callback onClick;

	public:
		Button():
			pBeingClicked(false),
			pOffsetX(0),
			pOffsetY(0),
			pTextColor(Theme::Current()->textColor),
			pBackColor(Theme::Current()->buttonColor),
			pFillColor(0.0f, 0.0f, 0.0f, 1.0f),
			pDisplay(dmNone)
		{}

		Button(float x, float y, float width, float height):
			IControl(x, y, width, height),
			pBeingClicked(false),
			pOffsetX(0),
			pOffsetY(0),
			pTextColor(Theme::Current()->textColor),
			pBackColor(Theme::Current()->buttonColor),
			pFillColor(0.0f, 0.0f, 0.0f, 1.0f),
			pDisplay(dmNone)
		{}

		Button(const Point2D<float>& position, const Point2D<float>& size):
			IControl(position, size),
			pBeingClicked(false),
			pOffsetX(0),
			pOffsetY(0),
			pTextColor(Theme::Current()->textColor),
			pBackColor(Theme::Current()->buttonColor),
			pFillColor(0.0f, 0.0f, 0.0f, 1.0f),
			pDisplay(dmNone)
		{}

		//! Virtual destructor
		virtual ~Button() {}

		const String& text() const { return pText; }
		void text(const AnyString& text) { pText = text; invalidate(); }

		//! Launch a click event
		void click() { onClick(this); }

		//! Draw the button on the surface
		virtual void draw(DrawingSurface::Ptr& surface, float xOffset, float yOffset) const override;

		//! Image shown on the button
		void image(const Gfx3D::Texture::Ptr& image) { pImage = image; invalidate(); }

		//! Image displayed while clicking (same as standard image if `null`)
		void imageClicking(const Gfx3D::Texture::Ptr& image) { pImageClicking = image; invalidate(); }

		//! Image displayed while hovering (same as standard image if `null`)
		void imageHovering(const Gfx3D::Texture::Ptr& image) { pImageHovering = image; invalidate(); }

		//! On mouse button down
		virtual EventPropagation mouseDown(Input::IMouse::Button btn, float, float) override
		{
			if (btn != Input::IMouse::ButtonLeft)
				return epStop;

			pBeingClicked = true;
			if (!(!pImageClicking))
				invalidate();
			return epStop;
		}

		//! On mouse button up
		virtual EventPropagation mouseUp(Input::IMouse::Button btn, float, float) override
		{
			if (btn != Input::IMouse::ButtonLeft)
				return epStop;

			if (pBeingClicked)
				onClick(this);
			pBeingClicked = false;
			invalidate();
			return epStop;
		}

		//! Get the text color
		const Color::RGBA<float>& textColor() const { return pTextColor; }
		//! Set the text color
		void textColor(const Color::RGBA<float>& color) { pTextColor = color; invalidate(); }

		//! Get the back color
		const Color::RGBA<float>& backColor() const { return pBackColor; }
		//! Set the back color
		void backColor(const Color::RGBA<float>& color) { pBackColor = color; invalidate(); }

		//! Get the fill color
		const Color::RGBA<float>& fillColor() const { return pFillColor; }
		//! Set the fill color
		void fillColor(const Color::RGBA<float>& color) { pFillColor = color; invalidate(); }

		//! Current display mode
		DisplayMode display() const { return pDisplay; }
		//! Set display mode to Fit
		void fit() { pDisplay = dmFit; invalidate(); }
		//! Set display mode to Fill
		void fill() { pDisplay = dmFill; invalidate(); }
		//! Set display mode to Center
		void center() { pDisplay = dmCenter; invalidate(); }
		//! Set display mode to Stretch
		void stretch() { pDisplay = dmStretch; invalidate(); }
		//! Set display mode to Offset and set offset values
		void offset(float x, float y)
		{
			pDisplay = dmOffset;
			pOffsetX = x;
			pOffsetY = y;
			invalidate();
		}
		//! Get current offset (might not be used if display mode is not podOffset)
		Point2D<float> offset() const { return Point2D<float>(pOffsetX, pOffsetY); }

	private:
		String pText;

		String pHoverText;

		Gfx3D::Texture::Ptr pImage;

		Gfx3D::Texture::Ptr pImageClicking;

		Gfx3D::Texture::Ptr pImageHovering;

		bool pBeingClicked;

		//! Offset of the rectangle over the image in X (only used in Offset display mode)
		float pOffsetX;

		//! Offset of the rectangle over the image in Y (only used in Offset display mode)
		float pOffsetY;

		//! Text color: Use the theme color by default
		Color::RGBA<float> pTextColor;

		//! Background color : Use the theme color by default
		Color::RGBA<float> pBackColor;

		//! Fill color for when a part of the overlay is empty
		Color::RGBA<float> pFillColor;

		//! Type of display of the image inside the rectangle
		DisplayMode pDisplay;

	}; // class Button



} // namespace Control
} // namespace UI
} // namespace Yuni

#endif // __YUNI_UI_CONTROL_BUTTON_H__
