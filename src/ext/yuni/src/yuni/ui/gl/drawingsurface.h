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
#ifndef __YUNI_UI_DRAWINGSURFACE_H__
# define __YUNI_UI_DRAWINGSURFACE_H__

# include "../../yuni.h"
# include "../../core/string.h"
# include "../../core/color/rgba.h"
# include "../../core/point2D.h"
# include <cassert>
# include "../ftfont.h"
# include "texture.h"
# include "../displaymode.h"

namespace Yuni
{
namespace UI
{

	//! Forward declaration
	class DrawingSurfaceImpl;


	/*!
	** \brief A rectangle surface for 2D drawing
	*/
	class DrawingSurface
	{
	public:
		//! Smart pointer
		typedef SmartPtr<DrawingSurface>  Ptr;


	public:
		//! Constructor to an empty transparent surface
		DrawingSurface(float width, float height);

		//! Destructor
		~DrawingSurface();

		//! Begin modifications on the surface
		void begin();

		//! Apply modifications on the surface
		void commit();

		//! Discard modifications on the surface
		void rollback();

		//! Clear the surface
		void clear();

		//! Resize the surface
		void resize(float width, float height);

		//! Draw text from a given coordinate
		void drawText(const String& text, const FTFont::Ptr& font, const Color::RGBA<float>& color,
			float x, float y, uint tabWidth = 4u, bool drawOnBaseline = false);

		//! Draw text from a given coordinate with a given background color
		void drawTextOnColor(const String& text, const FTFont::Ptr& font,
			const Color::RGBA<float>& color, const Color::RGBA<float>& backColor, float x, float y,
			uint tabWidth = 4u);

		//! Draw text centered in a rectangle
		void drawTextInRect(const String& text, const FTFont::Ptr& font,
			const Color::RGBA<float>& color, float x, float y, float width, float height,
			uint tabWidth = 4u);

		//! Draw text centered in a rectangle with a given background color
		void drawTextOnColorInRect(const String& text, const FTFont::Ptr& font,
			const Color::RGBA<float>& color, const Color::RGBA<float>& backColor,
			float x, float y, float width, float height, uint tabWidth = 4u);

		//! Draw an anti-aliased line on the surface
		void drawLine(const Color::RGBA<float>& color, float startX, float startY,
			float endX, float endY, float lineWidth);

		//! Draw a line anti-aliased with a given background color
		void drawLine(const Color::RGBA<float>& color, const Color::RGBA<float>& bgColor,
			float startX, float startY, float endX, float endY, float lineWidth);

		//! Draw an empty rectangle (borders only)
		void drawRectangle(const Color::RGBA<float>& frontColor,
			const Color::RGBA<float>& backColor, float x, float y, float width, float height,
			float lineWidth);

		//! Draw a rectangle filled with a background color on the surface
		void drawFilledRectangle(const Color::RGBA<float>& frontColor,
			const Color::RGBA<float>& backColor, float x, float y, float width, float height,
			float lineWidth);

		/*!
		** \brief Draw a rectangular image
		**
		** \warning `imageOpacity` is used only for the image, the alpha from `fillColor` is used for out of bounds opacity
		*/
		void drawImage(const Gfx3D::Texture::Ptr& texture, float x, float y,
			float width, float height, const Color::RGBA<float>& fillColor,
			DisplayMode dispMode = dmNone,
			float offsetX = 0, float offsetY = 0, float imageOpacity = 1.0f);

		//! Fill with a color. Use clipping to control the filled area
		void fill(const Color::RGBA<float>& color);

		//! Activate clipping inside a rectangle
		void beginRectangleClipping(float x, float y, float width, float height);

		//! Disable last clipping, must be called in pair with begin*Clipping
		void endClipping();

		//! Get the texture containing the rendering of this surface
		const Gfx3D::Texture::Ptr& texture() const;

		//! Get the current width of the surface
		float width() const;

		//! Get the current height of the surface
		float height() const;

	private:
		//! Update the texture according to the changes in the surface
		void updateTexture();

	private:
		DrawingSurfaceImpl* pImpl;

	}; // class DrawingSurface


} // namespace UI
} // namespace Yuni

#endif // __YUNI_UI_DRAWINGSURFACE_H__
