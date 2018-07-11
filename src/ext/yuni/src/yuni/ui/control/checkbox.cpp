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
#include "checkbox.h"
#include "../theme.h"

namespace Yuni
{
namespace UI
{
namespace Control
{


	void CheckBox::draw(DrawingSurface::Ptr& surface, float xOffset, float yOffset) const
	{
		if (!pVisible)
			return;

		auto themeptr = Theme::Current();
		auto& theme = *themeptr;
		Point2D<float> pos(pPosition.x + xOffset, pPosition.y + yOffset);

		surface->beginRectangleClipping(pos.x, pos.y, theme.checkBoxSize, theme.checkBoxSize);

		// Draw the check box
		surface->drawFilledRectangle(theme.borderColor, theme.buttonColor,
			pos.x, pos.y, theme.checkBoxSize, theme.checkBoxSize, theme.borderWidth);

		// Draw the cross if the box is checked
		if (pChecked)
		{
			surface->drawLine(theme.borderColor, theme.buttonColor, pos.x, pos.y,
				pos.x + theme.checkBoxSize, pos.y + theme.checkBoxSize, theme.borderWidth);
			surface->drawLine(theme.borderColor, theme.buttonColor, pos.x + theme.checkBoxSize,
				pos.y, pos.x, pos.y + theme.checkBoxSize, theme.borderWidth);
		}
		surface->endClipping();

		// Draw the label
		surface->drawText(pText, theme.font, theme.textColor,
			pos.x + theme.checkBoxSize + theme.margin, pos.y);
		pModified = false;
	}




} // namespace Control
} // namespace UI
} // namespace Yuni
