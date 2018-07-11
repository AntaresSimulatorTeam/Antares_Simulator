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

#include "../../core/system/windows.hdr.h"
#include "mouse.h"

namespace Yuni
{
namespace Input
{


	//! Mouse input management under Windows
	class WinMouse : public IMouse
	{
	public:
		//! Constructor
		WinMouse()
		{
			POINT p;
			::GetCursorPos(&p);
			pPos(p.x, p.y);
			pLastPos(pPos);
			pDragPos(pPos);
		}

		//! Virtual destructor
		virtual ~WinMouse() {}

		virtual void showCursor() { ::ShowCursor(true); }

		virtual void hideCursor() { ::ShowCursor(false); }

		virtual void showCursor(bool visible) { ::ShowCursor(visible); }

	}; // class WinMouse



} // namespace Input
} // namespace Yuni
