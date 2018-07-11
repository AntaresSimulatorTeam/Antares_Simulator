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
# include "button.h"

namespace Yuni
{
namespace UI
{
namespace Control
{


	void Button::draw(DrawingSurface::Ptr& surface, float xOffset, float yOffset) const
	{
		if (!pVisible)
			return;

		auto themeptr = Theme::Current();
		auto& theme = *themeptr;
		Point2D<float> pos(pPosition.x + xOffset, pPosition.y + yOffset);

		surface->beginRectangleClipping(pos.x, pos.y, pSize.x, pSize.y);
		// Manual full-background drawing (FIXME : this is not optimal)
		surface->drawFilledRectangle(theme.borderColor, pBackColor, pos.x, pos.y,
			pSize.x, pSize.y, theme.borderWidth);
		// Try drawing images on top
		if (pBeingClicked && !(!pImageClicking))
			surface->drawImage(pImageClicking, pos.x, pos.y, pSize.x, pSize.y, pBackColor,
				pDisplay, pOffsetX, pOffsetY, pOpacity);
		else if (!(!pImage))
			surface->drawImage(pImage, pos.x, pos.y, pSize.x, pSize.y, pBackColor, pDisplay,
				pOffsetX, pOffsetY, pOpacity);
		// Draw the text if any
		if (!pText.empty())
			surface->drawTextOnColorInRect(pText, theme.font, pTextColor, pBackColor,
				pos.x, pos.y, pSize.x, pSize.y);
		surface->endClipping();
		pModified = false;
	}



} // namespace Control
} // namespace UI
} // namespace Yuni
