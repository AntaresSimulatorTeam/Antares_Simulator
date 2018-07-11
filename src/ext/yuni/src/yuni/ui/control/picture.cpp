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
# include "picture.h"

namespace Yuni
{
namespace UI
{
namespace Control
{


	void Picture::draw(DrawingSurface::Ptr& surface, float xOffset, float yOffset) const
	{
		if (!pVisible)
			return;

		Point2D<float> pos(pPosition.x + xOffset, pPosition.y + yOffset);

		surface->beginRectangleClipping(pos.x, pos.y, pSize.x, pSize.y);
		// Manual full-background drawing (FIXME : this is not optimal)
		surface->drawFilledRectangle(pFillColor, pFillColor, pos.x, pos.y,
			pSize.x, pSize.y, 0);
		// Then try drawing the image
		if (!(!pImage))
			surface->drawImage(pImage, pos.x, pos.y, pSize.x, pSize.y, pFillColor, pDisplay,
				pOffsetX, pOffsetY, pOpacity);
		surface->endClipping();
		pModified = false;
	}



} // namespace Control
} // namespace UI
} // namespace Yuni
