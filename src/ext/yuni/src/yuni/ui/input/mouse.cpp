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
#include "mouse.h"

namespace Yuni
{
namespace Input
{

	void IMouse::doMove(int x, int y)
	{
		pLastPos(pPos);
		pPos(x, y);
		onMove(x, y);
	}

	void IMouse::doDown(Button btn)
	{
		pButtonClicked[btn] = true;
		pDragPos(pPos);
		onButtonDown(btn, pPos.x, pPos.y);
	}

	void IMouse::doUp(Button btn)
	{
		pButtonClicked[btn] = false;
		onButtonUp(btn, pPos.x, pPos.y);
	}

	void IMouse::doDblClick(Button btn)
	{
		onDblClick(btn, pPos.x, pPos.y);
	}

	void IMouse::doScroll(float delta)
	{
		onScroll(delta);
	}

	void IMouse::doHover(int x, int y)
	{
		onHover(x, y);
	}

	void IMouse::doLeave()
	{
		onLeave();
	}


} // namespace Input
} // namespace Yuni
