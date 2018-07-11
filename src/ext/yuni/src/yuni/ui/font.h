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
#pragma once
#include "../yuni.h"
#include "../core/smartptr.h"
#include "../core/string.h"
#include "../core/color/rgba.h"
#include "gl/texture.h"



namespace Yuni
{
namespace UI
{

	/*!
	 ** \brief Factory for fonts
	 */
	class Font
	{
	public:
		//! Smart pointer
		typedef SmartPtr<Font>  Ptr;

	public:
		//! Constructor
		Font(const AnyString& name, float size, bool bold = false, bool italic = false, bool underline = false, bool strikeout = false)
			: pID(0)
			, pName(name)
			, pSize(size > 0.0f ? size : 16.0f)
			, pBold(bold)
			, pItalic(italic)
			, pUnderline(underline)
			, pStrikeout(strikeout)
		{}

		//! Virtual destructor
		virtual ~Font() {}

		/*!
		** \brief Draw the given text using this font to the given texture
		** \note Expect the texture to be resized and cleared
		*/
		virtual void draw(const AnyString&, Gfx3D::Texture::Ptr&, bool antiAliased = true, bool useKerning = true, uint tabWidth = 4u) const = 0;

		/*!
		** \brief Measure a text drawn with this font
		** \see TextMeasurements
		*/
		virtual void measure(const AnyString&, uint& width, uint& height, int& ascent, int& descent,
			bool useKerning = true, uint tabWidth = 4u) const = 0;

		//! Is the font properly loaded and valid ?
		virtual bool valid() const { return true; }

		//! Get the font ID
		virtual uint id() const { return pID; }

		//! Font name
		const String& name() const { return pName; }

		//! Font absolute size (in points)
		float size() const { return pSize; }

		//! Font weight
		bool bold() const { return pBold; }

		//! Font is italic
		bool italic() const { return pItalic; }

		//! Font is underlined
		bool underline() const { return pUnderline; }

		//! Font is striked out
		bool strikeout() const { return pStrikeout; }


	protected:
		//! ID. !! This might end up only be useful for the GL implementation
		uint pID;
		//! Name of the font
		String pName;
		//! Size of the font (in points)
		float pSize;
		//! Is the font bold ?
		bool pBold;
		//! Is the font italic ?
		bool pItalic;
		//! Is the font underlined ?
		bool pUnderline;
		//! Is the font striked out ?
		bool pStrikeout;

	}; // class Font





} // namespace UI
} // namespace Yuni
