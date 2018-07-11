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
# include "window.h"
# include "../theme.h"

namespace Yuni
{
namespace UI
{
namespace Control
{


	void Window::draw(DrawingSurface::Ptr& surface, float xOffset, float yOffset) const
	{
		if (!pVisible)
			return;

		auto themeptr = Theme::Current();
		auto& theme = *themeptr;
		Point2D<float> pos(pPosition.x + xOffset, pPosition.y + yOffset);

		surface->beginRectangleClipping(pos.x, pos.y, pSize.x, pSize.y);
		surface->drawFilledRectangle(theme.borderColor, theme.windowColor,
			pos.x, pos.y, pSize.x, pSize.y, theme.borderWidth);
		// Draw the children
		drawChildren(surface, pos);
		surface->endClipping();
		pModified = false;
	}



} // namespace Control
} // namespace UI
} // namespace Yuni
