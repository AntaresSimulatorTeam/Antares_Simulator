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
#ifndef __YUNI_UI_CONTROL_TEXTEDITOR_H__
# define __YUNI_UI_CONTROL_TEXTEDITOR_H__

# include "../../yuni.h"
# include "../../core/color/rgb.h"
# include "../../core/color/rgba.h"
# include "../../core/math.h"
# include "../../core/functional/view.h"
# include "../../io/file.h"
# include "control.h"
# include "../displaymode.h"
# include "../dimension.h"
# include "../font.h"
# include "../theme.h"
# include <deque>

namespace Yuni
{
namespace UI
{
namespace Control
{


	//! A text is an multi-line area for text edition
	class TextEditor: public IControl
	{
	public:
		//! Smart pointer
		typedef Ancestor::SmartPtrType<TextEditor>::Ptr  Ptr;

	public:
		TextEditor(float x, float y, float maxWidth, float maxHeight):
			IControl(x, y, maxWidth, maxHeight),
			pText(),
			pCursorPos(0u, 0u),
			pScrollPos(0u, 0u),
			pDragPos(0u, 0u),
			pFont(Theme::Current()->monoFont),
			// White by default
			pColor(Theme::Current()->textColor),
			pBackColor(Theme::Current()->windowColor),
			pAntiAliased(true),
			pTabWidth(4u),
			pLineHeight(144_pcp), // 1.44 ratio
			pHorizMargin(15),
			pVertMargin(10)
		{
			// We use percent parent on the font size (height) for line height calculation
			pConversion.unitPerPercentParent = pFont->size() * 96.0f / 72.0f / 100.0f;
		}

		TextEditor(const Point2D<float>& position, const Point2D<float>& maxSize):
			IControl(position, maxSize),
			pText(),
			pCursorPos(0u, 0u),
			pScrollPos(0u, 0u),
			pDragPos(0u, 0u),
			pFont(Theme::Current()->monoFont),
			pColor(Theme::Current()->textColor),
			pBackColor(Theme::Current()->windowColor),
			pAntiAliased(true),
			pTabWidth(4u),
			pLineHeight(144_pcp), // 1.44 ratio
			pHorizMargin(15),
			pVertMargin(10)
		{
			// We use percent parent on the font size (height) for line height calculation
			pConversion.unitPerPercentParent = pFont->size() * 96.0f / 72.0f / 100.0f;
		}

		//! Virtual destructor
		virtual ~TextEditor() {}

		//! Draw the panel
		virtual void draw(DrawingSurface::Ptr& surface, float xOffset, float yOffset) const override;

		void clear()
		{
			invalidate();
			pCursorPos.reset();
			pScrollPos.reset();
			pDragPos.reset();
			pText.clear();
		}

		//! Load content from a file
		bool loadFromFile(const AnyString& filePath)
		{
			clear();
			if (IO::errNone != IO::File::LoadFromFile(pText, filePath))
			{
				pText << "Error loading file !";
				return false;
			}
			return true;
		}

		//! Get the text
		const Clob& text() const { return pText; }
		void text(const AnyString& newText)
		{
			invalidate();
			pCursorPos.reset();
			pScrollPos.reset();
			pDragPos.reset();
			pText = newText;
		}

		//! Modify the font used
		void font(const UI::FTFont::Ptr& font)
		{
			if (pFont != font)
			{
				pFont = font;
				pConversion.unitPerPercentParent = pFont->size() * 96.0f / 72.0f / 100.0f;
				invalidate();
			}
		}
		//! Get the font
		const UI::FTFont::Ptr& font() const { return pFont; }

		//! Modify the color used for text
		void color(float r, float g, float b) { pColor.assign(r, g, b); invalidate(); }
		void color(float r, float g, float b, float a) { pColor.assign(r, g, b, a); invalidate(); }
		void color(const Color::RGB<float>& color) { pColor = color; invalidate(); }
		void color(const Color::RGBA<float>& color) { pColor = color; invalidate(); }
		//! Modify the color used for background
		void backColor(float r, float g, float b) { pBackColor.assign(r, g, b); invalidate(); }
		void backColor(float r, float g, float b, float a) { pBackColor.assign(r, g, b, a); invalidate(); }
		void backColor(const Color::RGB<float>& color) { pBackColor = color; invalidate(); }
		void backColor(const Color::RGBA<float>& color) { pBackColor = color; invalidate(); }

		const Point2D<uint>& cursorPos() const { return pCursorPos; }
		void cursorPos(uint column, uint line)
		{
			pCursorPos.x = Math::Min(column, columnCount(currentLine(column, line)));
			pCursorPos.y = Math::Min(line, lineCount() - 1);
			invalidate();
		}
		void cursorPos(Point2D<uint> cursor)
		{
			pCursorPos.x = Math::Min(cursor.x, columnCount(currentLine(cursor)));
			pCursorPos.y = Math::Min(cursor.y, lineCount() - 1);
			invalidate();
		}

		//! Is the text anti-aliased ?
		bool antiAliased() const { return pAntiAliased; }
		//! Set text anti-aliasing
		void antiAliased(bool newValue) { pAntiAliased = newValue; invalidate(); }

		uint tabWidth() const { return pTabWidth; }
		void tabWidth(uint nbChars) { pTabWidth = nbChars; invalidate(); }

		//! Get line height
		const Dimension& lineHeight() const { return pLineHeight; }
		//! Get line height in pixels
		float lineHeightPixels() const { return pLineHeight(pConversion); }
		//! Set line height
		void lineHeight(const Dimension& newValue) { pLineHeight = newValue; }
		//! Set line height in pixels
		void lineHeightPixels(float newValue) { pLineHeight.reset(newValue, Dimension::uPixel); }

		virtual EventPropagation keyDown(Input::Key key) override;
		virtual EventPropagation charInput(const AnyString& str) override;
		virtual EventPropagation mouseDown(Input::IMouse::Button btn, float x, float y) override;
		virtual EventPropagation mouseUp(Input::IMouse::Button btn, float x, float y) override;
		virtual EventPropagation mouseMove(float x, float y) override;
		virtual EventPropagation mouseScroll(float delta, float x, float y) override;

	private:
		//! Cursor manipulation
		void cursorMoveLeft(uint offset = 1u);
		void cursorMoveRight(uint offset = 1u);
		void cursorMoveUp(uint offset = 1u);
		void cursorMoveDown(uint offset = 1u);
		void cursorBeginningOfLine();
		void cursorEndOfLine();
		void cursorBeginningOfText();
		void cursorEndOfText();

		//! Scroll by a number of lines, negative scrolls down, positive scrolls up
		void scroll(float nbLines);

		//! Number of lines of text displayed on screen
		float displayedLineCount() const
		{
			return (pSize.y - pVertMargin) / pLineHeight(pConversion);
		}

		//! Total number of lines in the text
		uint lineCount() const { return pText.countChar('\n') + 1; }

		//! Number of UTF-8 chars displayed on this line, tabs converted to spaces
		uint columnCount(uint lineNb) const
		{
			return columnCount(currentLine(0, lineNb));
		}

		//! Number of UTF-8 chars displayed on this line, tabs converted to spaces
		uint columnCount(const AnyString& line) const
		{
			return columnCount(line.utf8begin(), line.utf8end());
		}

		//! Number of UTF-8 chars displayed on this line, tabs converted to spaces
		template<class BeginT, class EndT>
		uint columnCount(const BeginT& begin, const EndT& end) const
		{
			return makeView(begin, end)
				.map([&](const AnyString::Char& c) -> uint
				{
					return c == '\t' ? pTabWidth : 1u;
				})
				.sum();
		}

		//! Text line currently containing the cursor
		AnyString currentLine(const Point2D<uint>& cursor) const
		{
			return currentLine(cursor.x, cursor.y);
		}

		//! Text line currently containing the cursor
		AnyString currentLine(uint /*columnNb*/, uint lineNb) const
		{
			AnyString finalLine;
			pText.words("\n", [&](const AnyString& line) -> bool
			{
				if (lineNb > 0)
				{
					--lineNb;
					return true;
				}
				else
				{
					finalLine = line;
					return false;
				}
			}, true);
			return finalLine;
		}

		//! Text line currently containing the index
		AnyString currentLine(uint byteIndex) const
		{
			auto start = pText.rfind('\n', byteIndex - 1) + 1;
			if (start >= pText.size())
				start = 0;
			AnyString part(pText, start);
			auto end = part.find('\n');
			return AnyString(part, 0, end >= part.size() ? part.size() - 1 : end);
		}

		//! Convert an X coordinate in pixels to the corresponding text column
		uint XToColumn(float x) const;

		//! Convert a Y coordinate in pixels to the corresponding text line
		uint YToLine(float y) const;

		//! Convert a column number on screen to the corresponding X coordinate in pixels
		float columnToX(uint col) const;

		//! Convert a line number on screen to the corresponding Y coordinate in pixels
		float lineToY(uint line) const;


		//! Get the index in the text that corresponds to a given cursor position
		uint cursorToByte(const Point2D<uint>& cursor) const
		{
			return cursorToByte(cursor.x, cursor.y);
		}

		//! Get the index in the text that corresponds to a given cursor position
		uint cursorToByte(uint columnNb, uint lineNb) const;

		//! Get the visual cursor position that corresponds to a given byte index
		void byteToCursor(Point2D<uint>& cursor, uint byte) const;

	private:
		//! Text to display
		Clob pText;

		//! Position of the edition cursor (column,line)
		Point2D<uint> pCursorPos;

		//! Right-most displayed column, and top-most displayed line (for scrolling)
		Point2D<uint> pScrollPos;

		/*!
		** \brief Position of the dragging cursor (column,line)
		**
		** This is the same as pCursorPos when not dragging and no text is selected
		** This is the opposite end of the selection if dragging / some text is selected
		*/
		Point2D<uint> pDragPos;

		//! Currently dragging ?
		bool pDragging;


		//! Font to use
		FTFont::Ptr pFont;

		//! Text color
		Color::RGBA<float> pColor;

		//! Background color
		Color::RGBA<float> pBackColor;

		//! Anti-alias the text ?
		bool pAntiAliased;

		//! Tab width (in number of spaces)
		uint pTabWidth;

		//! Height of a line in % of the font pixel size
		Dimension pLineHeight;

		//! Horizontal margin (pixels)
		float pHorizMargin;

		//! Vertical margin (pixels)
		float pVertMargin;

		//! Conversion data for font and text units
		ConversionData pConversion;

	}; // class TextEditor



} // namespace Control
} // namespace UI
} // namespace Yuni

#endif // __YUNI_UI_CONTROL_TEXTEDITOR_H__
