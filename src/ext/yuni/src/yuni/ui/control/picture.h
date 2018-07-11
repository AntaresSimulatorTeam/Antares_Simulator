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
#ifndef __YUNI_UI_CONTROL_PICTURE_H__
# define __YUNI_UI_CONTROL_PICTURE_H__

# include "../../yuni.h"
# include "control.h"


namespace Yuni
{
namespace UI
{
namespace Control
{


	//! A picture is a control that displays an image in a quad
	class Picture: public IControl
	{
	public:
		//! Smart pointer
		typedef Ancestor::SmartPtrType<Picture>::Ptr  Ptr;

	public:
		Picture():
			pOffsetX(0.0f),
			pOffsetY(0.0f),
			pFillColor(0.0f, 0.0f, 0.0f, 0.0f),
			pDisplay(dmStretch)
		{}

		Picture(float x, float y, float width, float height):
			IControl(x, y, width, height),
			pOffsetX(0.0f),
			pOffsetY(0.0f),
			pFillColor(0.0f, 0.0f, 0.0f, 0.0f),
			pDisplay(dmStretch)
		{}

		Picture(const Point2D<float>& position, const Point2D<float>& size):
			IControl(position, size),
			pOffsetX(0.0f),
			pOffsetY(0.0f),
			pFillColor(0.0f, 0.0f, 0.0f, 0.0f),
			pDisplay(dmStretch)
		{}

		//! Virtual destructor
		virtual ~Picture() {}

		//! Draw the picture
		virtual void draw(DrawingSurface::Ptr& surface, float xOffset, float yOffset) const override;

		//! Image shown on the button
		void image(const Gfx3D::Texture::Ptr& image) { pImage = image; invalidate(); }

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
		//! Texture containing the image
		Gfx3D::Texture::Ptr pImage;

		//! Offset of the overlay over the image in X (only used in Offset display)
		float pOffsetX;

		//! Offset of the overlay over the image in Y (only used in Offset display)
		float pOffsetY;

		//! Fill color for when a part of the overlay is empty
		Color::RGBA<float> pFillColor;

		//! Type of display of the image inside the overlay
		DisplayMode pDisplay;

	}; // class Picture



} // namespace Control
} // namespace UI
} // namespace Yuni

#endif // __YUNI_UI_CONTROL_PICTURE_H__
