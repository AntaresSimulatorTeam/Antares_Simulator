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

# include "../../yuni.h"
# include "glwindow.h"
# include "CMacsTypes.h"
//# include "../input/winmouse.h"
# include "../input/key.h"

namespace Yuni
{
namespace UI
{


	/*!
	** \brief Specific implementation of a window for OpenGL rendering under OS X / Cocoa
	*/
	class NSGLWindow: public GLWindow
	{
	public:
		//! List of local windows, by id
		typedef std::map<id, NSGLWindow*> WindowList;

	public:
		//! Constructor
		NSGLWindow(const AnyString& title, uint width, uint height, uint bitDepth, bool fullScreen):
			GLWindow(title, width, height, bitDepth, fullScreen)
		{
			//pMouse = new Input::OSXMouse();
		}
		//! Destructor
		~NSGLWindow();

		//! Initialize the window
		virtual bool initialize() override;

		//! Main event loop
		virtual bool loop() override;

		//! Kill the window, release everything
		virtual void kill() override;

		//! Minimize the window
		virtual void minimize() override { /*if (wsMinimized != pState) ::ShowWindow(pHWnd, SW_MINIMIZE);*/ }

		//! Minimize the window
		virtual void maximize() override { /*if (wsMaximized != pState) ::ShowWindow(pHWnd, SW_MAXIMIZE);*/ }

		//! Restore the window when minimized
		virtual void restore() override { /*if (wsNormal != pState) ::ShowWindow(pHWnd, SW_RESTORE);*/ }

		//! Force a window resize event
		virtual void resize(uint width, uint height) override {}

		//! Swap front and back buffers (OS-specific)
		virtual void swapBuffers() override { ::glFlush(); }

		//! Refresh and swap buffers with a single polymorphic call
		virtual void refreshAndSwap() override { onRefresh(); ::glFlush(); }

		//! Set window icon
		virtual void icon(const AnyString& path) override {}

		//! Set window title
		virtual void title(const AnyString& path) override {}

		//! Enable / Disable vertical sync (OS-specific)
		virtual void vsync(bool enable) override {}
		//! Current status of the vertical sync (OS-specific)
		virtual bool vsync() const override { return true; }

		/*!
		** \brief Should Full Screen AntiAliasing / MultiSampling be enabled ?
		**
		** Changing this value may kill and re-create the window.
		*/
	    virtual void multiSampling(MultiSampling::Type samplingType) override {}

		//! Enable / Disable full screen
		virtual void fullScreen(bool enable) override {}
		//! Is the window full screen ?
		virtual bool fullScreen() const override { return false; }

	protected:
		//! Find a window given its handle
		static NSGLWindow* FindWindow(id windowId);

		//! Register a window with its handle as a key
		static void RegisterWindow(id windowId, NSGLWindow* window);

		//! Unregister the window, happens when closing it
		static void UnregisterWindow(id windowId);

		// Event callbacks
		static void OnDrawRect(id self, SEL _cmd, CMRect rect);
		static void OnMouseDown(id self, SEL _cmd, void* event);
		static void OnWindowClose();

	private:
		//! Convert a Windows VK_* code to the equivalent Yuni code
		//		static Yuni::Input::Key ConvertInputKey(WPARAM keyCode);

		//! Enable full screen mode, use primary monitor with desktop resolution
		bool enableFullScreen();

		//! Disable full screen mode
		bool disableFullScreen();

		//! Window initialization
		bool initWindow();

		//! Only resize the internal data, this supposes the window is actually already resized
		void internalResize(uint width, uint height);

	protected:
		//! Static list of registered GDI windows
		static WindowList sWindowList;

	private:
		id pID;

		Logs::Logger<> logs;

	}; // class NSGLWindow






} // namespace UI
} // namespace Yuni
