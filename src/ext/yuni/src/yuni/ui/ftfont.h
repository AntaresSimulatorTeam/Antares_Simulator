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
#ifndef __YUNI_UI_FTFONT_H__
# define __YUNI_UI_FTFONT_H__

# include "../yuni.h"
# include "font.h"
# include "gl/texture.h"

namespace Yuni
{
namespace UI
{


	//! Forward declaration
	struct FTFontImpl;


	/*!
	** \brief Freetype font implementation
	*/
	class FTFont: public Font
	{
	public:
		//! Smart pointer
		typedef SmartPtr<FTFont>  Ptr;

	public:
		//! Constructor
		FTFont(const AnyString& name, float size, bool bold = false, bool italic = false, bool underline = false, bool strikeout = false);

		//! Virtual destructor
		virtual ~FTFont();

		/*!
		** \brief Draw the given text using this font to the given texture
		** \note Expect the texture to be resized and cleared
		*/
		virtual void draw(const AnyString& text, Gfx3D::Texture::Ptr& texture,
			bool antiAliased = true, bool useKerning = true, uint tabWidth = 4u) const override;

		/*!
		** \brief Measure a text drawn with this font
		** \see TextMeasurements
		*/
		virtual void measure(const AnyString& text, uint& width, uint& height, int& ascent, int& descent,
			bool useKerning = true, uint tabWidth = 4u) const override;
		//! Initialize the font
		bool initialize();

		//! Reset the font cache
		void reset();

		//! Is the font valid ?
		bool valid() const;

	private:
		//! PImpl pattern (to avoid freetype dependencies in the header)
		FTFontImpl* pImpl;

	}; // class FTFont



} // namespace UI
} // namespace Yuni


#endif // __YUNI_UI_FTFONT_H__
