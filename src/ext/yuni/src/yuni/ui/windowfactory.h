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

#include "../yuni.h"
#include "../core/string.h"

namespace Yuni
{
namespace UI
{


	//! Forward declaration
	class RenderWindow;


	//! Factory for creating platform-specific windows
	class WindowFactory
	{
	public:
		static RenderWindow* CreateGLWindow(const AnyString& title, uint width,
			uint height, uint bitDepth, bool fullScreen);

	private:
		WindowFactory() {}
		WindowFactory(const WindowFactory&) {}

	}; // class WindowFactory



} // UI
} // Yuni
