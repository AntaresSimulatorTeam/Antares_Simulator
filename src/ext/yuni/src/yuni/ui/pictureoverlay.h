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
#ifndef __YUNI_UI_PICTUREOVERLAY_H__
# define __YUNI_UI_PICTUREOVERLAY_H__

# include "../yuni.h"
# include "../core/smartptr.h"
# include <vector>
# include "gl/shaderprogram.h"
# include "gl/texture.h"
# include "displaymode.h"

namespace Yuni
{
namespace UI
{

	/*!
	** \brief A picture overlay is a 2D picture meant to be displayed on screen over a 3D view
	*/
	class PictureOverlay
	{
	public:
		//! Smart pointer
		typedef SmartPtr<PictureOverlay>  Ptr;
		//! Vector
		typedef std::vector<Ptr>  Vector;

	public:
		//! Empty constructor
		PictureOverlay();

		//! Constructor. Passing 0 as a dimension indicates to use the value from the texture.
		PictureOverlay(const Gfx3D::Texture::Ptr& texture, int x, int y, uint width = 0, uint height = 0);

		//! Destructor
		~PictureOverlay();

		//! Draw the picture
		void draw(const Gfx3D::ShaderProgram::Ptr& shader) const;

		//! Modify the picture data
		void texture(const Gfx3D::Texture::Ptr& texture);
		//! Get the picture data
		const Gfx3D::Texture::Ptr& texture() const;

		//! Move the picture to another position
		void move(int x, int y);

		//! Get the X coordinate
		int x() const;
		//! Get the Y coordinate
		int y() const;

		//! Set the size of the picture
		void resize(uint width, uint height);

		//! Scale both dimensions by a factor
		void scale(float factor);

		//! Width of the picture (in pixels)
		uint width() const;
		//! Height of the picture (in pixels)
		uint height() const;

		//! Enable visibility
		void show() { pVisible = true; }
		//! Disable visibility
		void hide() { pVisible = false; }
		//! Set visibility
		void show(bool visible) { pVisible = visible; }
		//! Get visibility
		bool visible() const { return pVisible; }

		//! Get the fill color
		const Color::RGBA<float>& fillColor() const { return pFillColor; }
		//! Set the fill color
		void fillColor(const Color::RGBA<float>& color) { pFillColor = color; }

		//! Current display mode
		DisplayMode display() const { return pDisplay; }
		//! Set display mode to Fit
		void fit() { pDisplay = dmFit; }
		//! Set display mode to Fill
		void fill() { pDisplay = dmFill; }
		//! Set display mode to Center
		void center() { pDisplay = dmCenter; }
		//! Set display mode to Stretch
		void stretch() { pDisplay = dmStretch; }
		//! Set display mode to Offset and set offset values
		void offset(int x, int y) { pDisplay = dmOffset; pOffsetX = x; pOffsetY = y; }
		//! Get current offset (might not be used if display mode is not podOffset)
		Point2D<int> offset() const { return Point2D<int>(pOffsetX, pOffsetY); }

	private:
		//! Picture to display
		Gfx3D::Texture::Ptr pTexture;

		/*!
		** \brief Is it hidden or visible ?
		**
		** \note This has nothing to do with being occluded, this is a mechanism to hide the overlay
		*/
		bool pVisible;

		//! X coordinate for the left of the picture (in pixels, left = 0, right = ScreenWidth)
		int pX;

		//! Y coordinate for the bottom of the picture (in pixels, top = 0, bottom = ScreenHeight)
		int pY;

		//! Width of the picture (in pixels)
		uint pWidth;

		//! Height of the picture (in pixels)
		uint pHeight;

		//! Offset of the overlay over the image in X (only used in Offset display)
		int pOffsetX;

		//! Offset of the overlay over the image in Y (only used in Offset display)
		int pOffsetY;

		//! Fill color for when a part of the overlay is empty
		Color::RGBA<float> pFillColor;

		//! Type of display of the image inside the overlay
		DisplayMode pDisplay;
	};


} // namespace UI
} // namespace Yuni

# include "pictureoverlay.hxx"

#endif // __YUNI_UI_PICTUREOVERLAY_H__
