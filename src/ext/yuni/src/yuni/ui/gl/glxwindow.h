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
#ifndef __YUNI_UI_GLXWINDOW_H__
# define __YUNI_UI_GLXWINDOW_H__

#include <yuni/yuni.h>
#include <yuni/private/graphics/opengl/glew/glew.h>
#include <yuni/private/graphics/opengl/glew/glxew.h>
#include "glwindow.h"

namespace Yuni
{
namespace UI
{


	/*!
	** \brief Specific implementation of a window for OpenGL rendering under MS Windows
	*/
	class GLXWindow: public GLWindow
	{
	public:
		//! Constructor
		GLXWindow(const AnyString& title, uint width, uint height, uint bitDepth, bool fullScreen) :
			GLWindow(title, width, height, bitDepth, fullScreen)
		{}

		//! Virtual destructor
		virtual ~GLXWindow() {}

		//! Initialize the window
		virtual bool initialize() override;

		//! Main event loop
		virtual bool loop();

		//! Kill the window, release everything
		virtual void kill() override;

		//! Minimize the window
		virtual void minimize() override;

		//! Minimize the window
		virtual void maximize() override;

		//! Minimize the window
		virtual void restore() override;

		//! Swap front and back buffers (OS-specific)
		virtual void swapBuffers() override;

		//! Refresh and swap buffers with a single polymorphic call
		virtual void refreshAndSwap() override;

		//! Set window icon
		virtual void icon(const AnyString& path) override;

		//! Set window title
		virtual void title(const AnyString& path) override;

		//! Enable / Disable vertical sync
		virtual void vsync(bool enable) override;
		//! Current status of the vertical sync
		virtual bool vsync() const override;

		//! Does the window have Full Screen AntiAliasing / MultiSampling ?
		virtual bool antiAliasing() const override;
		/*!
		** \brief Should Full Screen AntiAliasing / MultiSampling be enabled ?
		**
		** Changing this value may kill and re-create the window.
		*/
		virtual void antiAliasing(bool enable) override;

		//! Enable / Disable full screen
		virtual void fullScreen(bool enable) override;
		//! Is the window full screen ?
		virtual bool fullScreen() const override;

	private:
		Display* pDisplay;
		int pScreen;
		Window pWindow;
		GLXContext pContext;
		XSetWindowAttributes pAttr;
		XWindowAttributes pWndAttr;

	}; // GLXWindow


} // UI
} // Yuni

#include "glxwindow.hxx"

#endif // __YUNI_UI_GLXWINDOW_H__
