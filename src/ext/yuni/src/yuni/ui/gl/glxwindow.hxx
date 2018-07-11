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
#ifndef __YUNI_UI_GLXWINDOW_HXX__
# define __YUNI_UI_GLXWINDOW_HXX__

namespace Yuni
{
namespace UI
{


	inline void GLXWindow::swapBuffers()
	{
		//XNextEvent(pDisplay, &xev);
		::XGetWindowAttributes(pDisplay, pWindow, &pWndAttr);
		::glXSwapBuffers(pDisplay, pWindow);
	}

	inline void GLXWindow::refreshAndSwap()
	{
		pRefreshFunc();
		::XGetWindowAttributes(pDisplay, pWindow, &pWndAttr);
		::glXSwapBuffers(pDisplay, pWindow);
	}



} // UI
} // Yuni

#endif // __YUNI_UI_GLXWINDOW_HXX__
