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
#ifndef __YUNI_UI_TEXTOVERLAY_H__
# define __YUNI_UI_TEXTOVERLAY_H__

# include "../yuni.h"
# include "../core/smartptr.h"
# include "../core/string.h"
# include "../core/color/rgb.h"
# include "../core/color/rgba.h"
# include <vector>
# include "ftfont.h"
# include "pictureoverlay.h"
# include "gl/materialsurface.h"
# include "gl/shaderprogram.h"
# include "gl/texture.h"

namespace Yuni
{
namespace UI
{

	/*!
	** \brief A text overlay is a 2D line of text that is displayed on screen on top of the 3D view
	*/
	class TextOverlay
	{
	public:
		//! Smart pointer
		typedef SmartPtr<TextOverlay>  Ptr;
		//! Vector
		typedef std::vector<Ptr>  Vector;

	public:
		//! Empty constructor
		TextOverlay();

		//! Constructor
		TextOverlay(const FTFont::Ptr& font);

		//! Constructor
		TextOverlay(const AnyString& text, float x, float y, const Color::RGBA<float>& color);

		//! Destructor
		~TextOverlay();

		//! Replace the text in this overlay
		TextOverlay& operator = (const AnyString& newText);

		//! Concatenate more text in this overlay
		String& operator << (const AnyString& newText);

		//! Update the internal representation
		void update();

		//! Draw the text using the given shader
		void draw(const Gfx3D::ShaderProgram::Ptr& shader) const;
		void draw(const Gfx3D::ShaderProgram::Ptr& shader, const Color::RGB<float>& backColor) const;		void draw(const Gfx3D::ShaderProgram::Ptr& shader, const Color::RGBA<float>& backColor) const;

		//! Clear the text
		String& clear();

		//! Get the text
		String& text();
		const String& text() const;

		//! Modify the font used
		void font(const UI::FTFont::Ptr& font);
		//! Get the font
		const UI::FTFont::Ptr& font() const;

		//! Get the material
		const Gfx3D::MaterialSurface::Ptr& material() const;

		//! Current width of the internal texture
		float width() const;

		//! Current height of the internal texture
		float height() const;

		//! Text ascent (Y bearing = distance from the top of the text to the baseline)
		int ascent() const { return pAscent; }

		//! Modify the color used
		void color(float r, float g, float b);
		void color(float r, float g, float b, float a);
		void color(const Color::RGB<float>& color);
		void color(const Color::RGBA<float>& color);

		//! Apply a texture to the text
		void texture(const Gfx3D::Texture::Ptr& texture);

		//! Is the text anti-aliased ?
		bool antiAliased() const;
		//! Set text anti-aliasing
		void antiAliased(bool newValue);

		//! Number of space characters written for one tab character
		uint tabWidth() const { return pTabWidth; }
		void tabWidth(uint nbChars) { pTabWidth = nbChars; }

		//! Use the Y coordinate as the text baseline rather than the top of the text ?
		bool drawOnBaseline() const { return pDrawOnBaseline; }
		void drawOnBaseline(bool onBaseline) { pDrawOnBaseline = onBaseline; }

		//! Move the text to another position
		void move(float x, float y);

		//! Get the X coordinate
		float x() const;
		//! Get the Y coordinate
		float y() const;

		//! Convert to a new picture overlay (e.g. for resizing)
		PictureOverlay::Ptr toPicture() const;

	private:
		//! Text to display
		String pText;

		//! Store previous text to avoid resizing and updating texture for naught
		String pPrevText;

		//! Rendered texture
		Gfx3D::Texture::Ptr pTexture;

		//! X coordinate for the left of the text (in pixels, 0 -> ScreenWidth = left -> right)
		float pX;

		//! Y coordinate for the bottom of the text (in pixels, 0 -> ScreenHeight = top -> bottom)
		float pY;

		//! Text ascent (Y bearing = distance from the top of the text to the baseline)
		int pAscent;

		//! Font to use
		FTFont::Ptr pFont;

		//! Text color
		Color::RGBA<float> pColor;

		//! Material
		Gfx3D::MaterialSurface::Ptr pMaterial;

		//! Anti-alias the text ?
		bool pAntiAliased;

		//! Tab width
		uint pTabWidth;

		//! Track modification
		bool pModified;

		//! Use the Y coordinate as the text baseline rather than the top of the text
		bool pDrawOnBaseline;
	};


} // namespace UI
} // namespace Yuni

# include "textoverlay.hxx"

#endif // __YUNI_UI_TEXTOVERLAY_H__
