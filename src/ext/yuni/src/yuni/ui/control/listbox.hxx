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

namespace Yuni
{
namespace UI
{
namespace Control
{


	template<class ContentT>
	void ListBox<ContentT>::draw(DrawingSurface::Ptr& surface, float xOffset, float yOffset) const
	{
		if (!pVisible)
			return;

		Point2D<float> pos(pPosition.x + xOffset, pPosition.y + yOffset);

		// Draw background
		surface->drawFilledRectangle(pBackColor, pBackColor, pos.x, pos.y, pSize.x, pSize.y, 0.0f);

		surface->beginRectangleClipping(pos.x + pHorizMargin, pos.y + pVertMargin,
			pSize.x - 2 * pHorizMargin, pSize.y - 2 * pVertMargin);

		// Draw the text
		float pixelLineHeight = pLineHeight(pConversion);
		float x = pos.x + pHorizMargin;
		float y = pos.y + pVertMargin;
		uint lineNb = 0u;
		// Loop on elements
		for (const auto& element : pElements)
		{
			if (lineNb++ < pTopLineNb)
				continue;
			// Ignore empty labels
			if (!element.label.empty())
			{
				if (pIndex == lineNb - 1)
					// Currently selected -> invert colors
					surface->drawTextOnColor(element.label, pFont, pBackColor, pColor, x, y);
				else
					surface->drawText(element.label, pFont, pColor, x, y);
			}
			y += pixelLineHeight;
			// Stop if we are outside the rectangle (for optim)
			if (y >= pSize.y)
				break;
		}

		surface->endClipping();
		pModified = false;
	}


	template<class Content>
	EventPropagation ListBox<Content>::mouseScroll(float delta, float, float)
	{
		uint oldTopLine = pTopLineNb;
		float newLineNb = (float)pTopLineNb - delta;
		float maxLineNb = (float)pElements.size();
		float displayedLineCount = pSize.y / pLineHeight(pConversion);
		pTopLineNb = (uint)Math::Max(0.0f, Math::Min(maxLineNb - displayedLineCount, newLineNb));
		if (oldTopLine != pTopLineNb)
			invalidate();
		return epStop;
	}


	template<class Content>
	EventPropagation ListBox<Content>::mouseDown(Input::IMouse::Button btn, float, float y)
	{
		if (btn == Input::IMouse::ButtonLeft)
		{
			// Store selected index, wait mouseUp to confirm selection
			pClickedIndex = pTopLineNb + Math::Floor((y - pPosition.y) / pLineHeight(pConversion));
			if (pClickedIndex < 0 || (uint)pClickedIndex >= pElements.size())
				pClickedIndex = -1;
		}
		return epStop;
	}


	template<class Content>
	EventPropagation ListBox<Content>::mouseUp(Input::IMouse::Button btn, float, float y)
	{
		if (btn == Input::IMouse::ButtonLeft && pClickedIndex > -1)
		{
			// Store selected index, wait mouseUp to confirm selection
			uint newIndex = pTopLineNb + Math::Floor((y - pPosition.y) / pLineHeight(pConversion));
			if (newIndex == (uint)pClickedIndex)
			{
				pIndex = newIndex;
				onSelectionChange(this, pIndex);
				invalidate();
			}
			pClickedIndex = -1;
		}
		return epStop;
	}



} // namespace Control
} // namespace UI
} // namespace Yuni
