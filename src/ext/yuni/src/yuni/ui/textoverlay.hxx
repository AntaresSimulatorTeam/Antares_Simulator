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
#ifndef __YUNI_UI_TEXTOVERLAY_HXX__
# define __YUNI_UI_TEXTOVERLAY_HXX__

# include "theme.h"


namespace Yuni
{
namespace UI
{


	inline TextOverlay::TextOverlay():
		pText(),
		pPrevText(),
		pTexture(nullptr),
		pX(0),
		pY(0),
		pAscent(0),
		pFont(Theme::Current()->font),
		// White by default
		pColor(1.0f, 1.0f, 1.0f, 1.0f),
		pMaterial(new Gfx3D::MaterialSurfaceColored("TextColor", 1.0f, 1.0f, 1.0f)),
		pAntiAliased(true),
		pTabWidth(4u),
		pModified(true),
		pDrawOnBaseline(false)
	{
	}


	inline TextOverlay::TextOverlay(const FTFont::Ptr& font):
		pText(),
		pPrevText(),
		pTexture(nullptr),
		pX(0),
		pY(0),
		pAscent(0),
		pFont(font),
		// White by default
		pColor(1.0f, 1.0f, 1.0f, 1.0f),
		pMaterial(new Gfx3D::MaterialSurfaceColored("TextColor", 1.0f, 1.0f, 1.0f)),
		pAntiAliased(true),
		pTabWidth(4u),
		pModified(true),
		pDrawOnBaseline(false)
	{
	}


	inline TextOverlay::TextOverlay(const AnyString& text, float x, float y, const Color::RGBA<float>& color):
		pText(text),
		pPrevText(),
		pTexture(nullptr),
		pX(x),
		pY(y),
		pAscent(0),
		pFont(Theme::Current()->font),
		pColor(color),
		pMaterial(new Gfx3D::MaterialSurfaceColored("TextColor", color)),
		pAntiAliased(true),
		pTabWidth(4u),
		pModified(true),
		pDrawOnBaseline(false)
	{
	}


	inline TextOverlay::~TextOverlay()
	{
	}



	inline TextOverlay& TextOverlay::operator = (const AnyString& newText)
	{
		pPrevText = pText;
		pText = newText;
		return *this;
	}

	inline String& TextOverlay::operator << (const AnyString& newText)
	{
		pPrevText = pText;
		return pText << newText;
	}


	inline String& TextOverlay::clear()
	{
		pPrevText = pText;
		return pText.clear();
	}


	inline String& TextOverlay::text()
	{
		pPrevText = pText;
		return pText;
	}

	inline const String& TextOverlay::text() const
	{
		return pText;
	}


	inline void TextOverlay::font(const UI::FTFont::Ptr& font)
	{
		if (font != pFont)
		{
			pModified = true;
			pFont = font;
		}
	}

	inline const UI::FTFont::Ptr& TextOverlay::font() const
	{
		return pFont;
	}


	inline float TextOverlay::width() const
	{
		return nullptr != pTexture ? (float)pTexture->width() : 0.0f;
	}


	inline float TextOverlay::height() const
	{
		return nullptr != pTexture ? (float)pTexture->height() : 0.0f;
	}


	inline void TextOverlay::color(float r, float g, float b)
	{
		pModified = true;
		pColor.assign(r, g, b);
		pMaterial = new Gfx3D::MaterialSurfaceColored("TextColor", r, g, b);
	}

	inline void TextOverlay::color(float r, float g, float b, float a)
	{
		pModified = true;
		pColor.assign(r, g, b, a);
		pMaterial = new Gfx3D::MaterialSurfaceColored("TextColor", r, g, b, a);
	}

	inline void TextOverlay::color(const Color::RGB<float>& color)
	{
		pModified = true;
		pColor = color;
		pMaterial = new Gfx3D::MaterialSurfaceColored("TextColor", color);
	}

	inline void TextOverlay::color(const Color::RGBA<float>& color)
	{
		pModified = true;
		pColor = color;
		pMaterial = new Gfx3D::MaterialSurfaceColored("TextColor", color);
	}


	inline bool TextOverlay::antiAliased() const
	{
		return pAntiAliased;
	}

	inline void TextOverlay::antiAliased(bool newValue)
	{
		pModified = true;
		pAntiAliased = newValue;
	}


	inline void TextOverlay::texture(const Gfx3D::Texture::Ptr& texture)
	{
		pMaterial = new Gfx3D::MaterialSurfaceTextured(texture);
	}


	inline void TextOverlay::move(float x, float y)
	{
		pX = x;
		pY = y;
	}

	inline float TextOverlay::x() const
	{
		return pX;
	}

	inline float TextOverlay::y() const
	{
		return pY;
	}



} // namespace UI
} // namespace Yuni

#endif // __YUNI_UI_TEXTOVERLAY_HXX__
