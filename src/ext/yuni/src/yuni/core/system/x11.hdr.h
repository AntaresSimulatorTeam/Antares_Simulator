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
#include "../../yuni.h"
#include <cstring>
#include <GL/glx.h>
#include <X11/extensions/Xrender.h>

// The define None from the X11 headers overlaps with Yuni::None...
# ifdef None
#	undef None
# endif

# define YUNI_X11LIB_NONE  0L

# define _NET_WM_STATE_REMOVE	0 // Unset a state
# define _NET_WM_STATE_ADD		1 // Set a state
# define _NET_WM_STATE_TOGGLE	2 // Toggle a state


namespace // anonymous
{

	/*!
	** \brief Set a state on a window using extended window manager hints (EMWH)
	**
	** \param display X Display
	** \param window Window to set a state on
	** \param operation Can be _NET_WM_STATE_ADD, _NET_WM_STATE_REMOVE or _NET_WM_STATE_TOGGLE
	** \param state State to modify
	*/
	static void netwm_set_state(Display *display, Window window, int operation, Atom state)
	{
		XEvent e;

		Atom _NET_WM_STATE = XInternAtom(display, "_NET_WM_STATE", false);

		memset(&e, 0, sizeof(e));
		e.xclient.type = ClientMessage;
		e.xclient.message_type = _NET_WM_STATE;
		e.xclient.display = display;
		e.xclient.window = window;
		e.xclient.format = 32;
		e.xclient.data.l[0] = operation;
		e.xclient.data.l[1] = state;
		XSendEvent(display, DefaultRootWindow(display), false, SubstructureRedirectMask, &e);
	}


} // namespace anonymous

