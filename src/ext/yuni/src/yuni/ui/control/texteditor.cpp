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
#include "texteditor.h"
#include <locale>

namespace Yuni
{
namespace UI
{
namespace Control
{


	void TextEditor::draw(DrawingSurface::Ptr& surface, float xOffset, float yOffset) const
	{
		if (!pVisible)
			return;

		Point2D<float> pos(pPosition.x + xOffset, pPosition.y + yOffset);

		// Draw background
		surface->drawFilledRectangle(pBackColor, pBackColor, pos.x, pos.y, pSize.x, pSize.y, 0.0f);

		surface->beginRectangleClipping(pos.x, pos.y, pSize.x, pSize.y);

		// Draw the text
		float pixelLineHeight = pLineHeight(pConversion);
		float x = pos.x + pHorizMargin;
		float y = pos.y + pVertMargin;
		const uint lastVisibleLine = YToLine(pSize.y - pVertMargin);
		// Loop on lines of text
		uint lineNb = pScrollPos.y;
		AnyString text(pText, cursorToByte(pScrollPos));
		text.words("\n", [&](AnyString& line) -> bool
		{
			if (lineNb++ > lastVisibleLine)
				return false;
			// TODO : x scroll offset ?
			if (!line.empty())
			{
				// Crop trailing `\r` (CR) if necessary
				if ('\r' == line[line.length() - 1])
					line.truncate(line.length() - 1);
				// Ignore empty lines (test a second time to catch single "\r" lines)
				if (!line.empty())
				{
					// y is the baseline position, not the top
					surface->drawText(line, pFont, pColor, x, y - pixelLineHeight / 2, pTabWidth, true);
				}
			}
			y += pixelLineHeight;
			return true;
		}, true);

		// Draw the cursor
		if (pCursorPos.y >= pScrollPos.y && pCursorPos.y <= lastVisibleLine)
		{
			float cx = columnToX(pCursorPos.x);
			float cy = lineToY(pCursorPos.y);
			surface->drawLine(pColor, cx, cy, cx, cy + pixelLineHeight, 1.0f);
		}

		// Draw line and column numbers
		surface->drawText(String(pCursorPos.y + 1) << ':' << pCursorPos.x, pFont, pColor, pSize.x - 50, pSize.y - 15);

		surface->endClipping();
		pModified = false;
	}


	void TextEditor::cursorMoveLeft(uint offset)
	{
		uint index = cursorToByte(pCursorPos);
		if (0 == index)
			return;
		AnyString part(pText, 0, index + 1);
		auto it = part.utf8begin();
		it += index - offset;
		byteToCursor(pCursorPos, index + it.offset());
		invalidate();
	}

	void TextEditor::cursorMoveRight(uint offset)
	{
		uint index = cursorToByte(pCursorPos);
		AnyString part(pText, index);
		auto it = part.utf8begin();
		it += offset;
		byteToCursor(pCursorPos, index + it.offset());
		invalidate();
	}

	void TextEditor::cursorMoveUp(uint offset)
	{
		uint end = cursorToByte(pCursorPos);
		if (0 == end)
			return;

		AnyString part(pText, 0, end);
		for (uint i = end - 1; i > 0 && offset > 0; --i)
		{
			if ('\n' == part[i])
			{
				end = i;
				--offset;
			}
		}
		uint start = part.rfind('\n');
		if (start >= part.size())
			start = 0;
		else
			++start;
		byteToCursor(pCursorPos, Math::Min(start + pCursorPos.x, end));
		invalidate();
	}

	void TextEditor::cursorMoveDown(uint offset)
	{
		uint start = cursorToByte(pCursorPos);
		AnyString part(pText, start);

		uint lineStart = 0u;
		for (uint i = 0; i < part.size() && offset > 0; ++i)
		{
			if ('\n' == part[i])
			{
				lineStart = i + 1;
				--offset;
			}
		}
		uint absLineStart = start + lineStart;
		byteToCursor(pCursorPos, Math::Min(absLineStart + pCursorPos.x, currentLine(absLineStart).size()));
		invalidate();
	}


	void TextEditor::cursorBeginningOfLine()
	{
		pCursorPos.x = 0;
		invalidate();
	}


	void TextEditor::cursorEndOfLine()
	{
		uint index = cursorToByte(pCursorPos);
		AnyString part(pText, index);
		uint end = part.find('\n');
		AnyString cut(part, 0, end);
		pCursorPos.x += columnCount(cut);
		invalidate();
	}


	void TextEditor::cursorBeginningOfText()
	{
		pCursorPos.reset();
		invalidate();
	}


	void TextEditor::cursorEndOfText()
	{
		byteToCursor(pCursorPos, pText.size());
		invalidate();
	}


	uint TextEditor::XToColumn(float x) const
	{
		if (x - pHorizMargin <= 0)
			return 0u;
		// TODO : I need the real advance of the clicked text to find the proper column value
		return (uint)Math::Round((x - pHorizMargin) / (pFont->size() / 1.25f)); // For now, use a not-quite-random ratio
	}

	uint TextEditor::YToLine(float y) const
	{
		if (y - pVertMargin <= 0)
			return pScrollPos.y;
		return uint((y - pVertMargin) / pLineHeight(pConversion)) + pScrollPos.y;
	}

	float TextEditor::columnToX(uint col) const
	{
		// TODO : I need the real advance of the clicked text to find the proper column value
		return (float)col * ((float)pFont->size() / 1.25f) + (float)pHorizMargin; // For now, use a not-quite-random ratio
	}

	float TextEditor::lineToY(uint line) const
	{
		return (float)(line - pScrollPos.y) * pLineHeight(pConversion) + (float)pVertMargin;
	}


	EventPropagation TextEditor::keyDown(Input::Key key)
	{
		switch (key)
		{
			case Input::Left:
				cursorMoveLeft();
				break;
			case Input::Right:
				cursorMoveRight();
				break;
			case Input::Up:
				cursorMoveUp();
				break;
			case Input::Down:
				cursorMoveDown();
				break;
			case Input::Home:
				cursorBeginningOfLine();
				break;
			case Input::End:
				cursorEndOfLine();
				break;
			case Input::PageUp:
				scroll(displayedLineCount());
				break;
			case Input::PageDown:
				scroll(-displayedLineCount());
				break;
			// Delete
			case Input::Delete:
			{
				pText.erase(cursorToByte(pCursorPos), 1);
				invalidate();
				break;
			}
			default:
				break;
		}
		return epStop;
	}


	EventPropagation TextEditor::charInput(const AnyString& str)
	{
		switch (str[0])
		{
			// Backspace
			case 0x08:
				for (uint i = 0; i < str.size(); ++i)
				{
					// Cannot use backspace when at beginning of file
					if (0 == pCursorPos.y && 1 == pCursorPos.x)
						return epStop;
					// When at beginning of line but not on first line, move up
					if (0 == pCursorPos.y && pCursorPos.x > 1)
						cursorPos(pCursorPos.x - 1, columnCount(pCursorPos.x - 1));
					else
						cursorPos(pCursorPos.x, pCursorPos.y - 1);
					// Erase
					pText.erase(cursorToByte(pCursorPos), 1);
				}
				invalidate();
				break;
			// Space
			case ' ':
				pText.insert(cursorToByte(pCursorPos), str);
				pCursorPos.x += str.size();
				invalidate();
				break;
			// Tab
			case '\t':
				pText.insert(cursorToByte(pCursorPos), str);
				cursorPos(pCursorPos.x, pCursorPos.y + str.size() * pTabWidth);
				invalidate();
				break;
			// Carriage Return
			case '\r':
			// New Line / Line Feed
			case '\n':
				for (uint i = 0; i < str.size(); ++i)
					pText.insert(cursorToByte(pCursorPos), '\n');
				pCursorPos.y += str.size();
				invalidate();
				break;
			// Normal displayable characters
			default:
				// Normal ASCII
				if ((uint8)str[0] < 0x80)
				{
					// Non-displayable characters are ignored
					std::locale loc;
					if (!std::isgraph(str[0], loc))
						break;
				}
				pText.insert(cursorToByte(pCursorPos), str);

				// Advance the cursor
				pCursorPos.x += str.utf8size();
				invalidate();
				break;
		}
		return epStop;
	}


	EventPropagation TextEditor::mouseDown(Input::IMouse::Button btn, float x, float y)
	{
		if (btn == Input::IMouse::ButtonLeft)
		{
			cursorPos(XToColumn(x), YToLine(y));
			pDragPos(pCursorPos);
			pDragging = true;
			invalidate();
		}
		return epStop;
	}


	EventPropagation TextEditor::mouseUp(Input::IMouse::Button btn, float x, float y)
	{
		if (btn == Input::IMouse::ButtonLeft)
		{
			pDragPos(XToColumn(x), YToLine(y));
			pDragging = false;
			invalidate();
		}
		return epStop;
	}


	EventPropagation TextEditor::mouseMove(float x, float y)
	{
		if (pDragging)
		{
			pDragPos(XToColumn(x), YToLine(y));
			invalidate();
		}
		return epContinue;
	}


	EventPropagation TextEditor::mouseScroll(float delta, float, float)
	{
		scroll(delta);
		return epStop;
	}


	void TextEditor::scroll(float nbLines)
	{
		uint oldTopLine = pScrollPos.y;
		float newLineNb = ((float)pScrollPos.y > nbLines) ? (float)pScrollPos.y - nbLines : 0.0f;
		float maxLineNb = (float)lineCount();
		pScrollPos.y = (uint)Math::Min(maxLineNb - displayedLineCount(), newLineNb);
		if (oldTopLine != pScrollPos.y)
			invalidate();
	}


	uint TextEditor::cursorToByte(uint columnNb, uint lineNb) const
	{
		uint index = 0u;

		pText.words("\n", [&](const AnyString& line) -> bool
		{
			if (lineNb > 0)
			{
				--lineNb;
				index += line.size() + 1 /*\n*/;
				return true;
			}
			else
			{
				if (columnNb > 0)
				{
					const AnyString part(pText, index);
					auto end = part.utf8end();
					for (auto it = part.utf8begin(); it != end && columnNb-- > 0; ++it)
					{
						if ((*it) == (uint)'\t')
							columnNb -= (pTabWidth - 1);
						index += it->size();
					}
				}
				return false;
			}
		}, true);
		return Math::Min(pText.size(), index);
	}


	void TextEditor::byteToCursor(Point2D<uint>& cursor, uint byte) const
	{
		if (byte > 0)
		{
			AnyString part(pText, 0, Math::Min(byte, pText.size()));
			assert(part.utf8valid() and "invalid UTF8 string part");

			cursor.y = part.countChar('\n');
			if (cursor.y > 0)
			{
				auto pos = part.rfind('\n');
				if (pos < part.size())
				{
					++pos;
					AnyString lastline(part, pos);
					assert(lastline.utf8valid() and "invalid UTF8 sub string");
					cursor.x = columnCount(lastline);
				}
				else
					cursor.x = 0;
			}
			else
				cursor.x = columnCount(part);
		}
		else
			cursor.reset();
	}



} // namespace Control
} // namespace UI
} // namespace Yuni
