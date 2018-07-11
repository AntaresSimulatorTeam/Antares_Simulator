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
#ifndef __YUNI_UI_WGLWINDOW_H__
# define __YUNI_UI_WGLWINDOW_H__

# include <windows.h>
# include <map>
# include "glwindow.h"
# include "../input/winmouse.h"
# include "../input/key.h"

namespace Yuni
{
namespace UI
{


	/*!
	** \brief Specific implementation of a window for OpenGL rendering under MS Windows
	*/
	class WGLWindow: public GLWindow
	{
	public:
		//! List of local GDI windows, by HWND
		typedef std::map<HWND, WGLWindow*> WindowList;

	public:
		//! Constructor
		WGLWindow(const AnyString& title, uint width, uint height, uint bitDepth, bool fullScreen):
			GLWindow(title, width, height, bitDepth, fullScreen),
			pHasMSAASupport(false)
		{
			pMouse = new Input::WinMouse();
		}
		//! Destructor
		~WGLWindow();

		//! Initialize the window
		virtual bool initialize() override;

		//! Main event loop
		virtual bool loop() override;

		//! Kill the window, release everything
		virtual void kill() override;

		//! Minimize the window
		virtual void minimize() override { if (wsMinimized != pState) ::ShowWindow(pHWnd, SW_MINIMIZE); }

		//! Minimize the window
		virtual void maximize() override { if (wsMaximized != pState) ::ShowWindow(pHWnd, SW_MAXIMIZE); }

		//! Restore the window when minimized
		virtual void restore() override { if (wsNormal != pState) ::ShowWindow(pHWnd, SW_RESTORE); }

		//! Force a window resize event
		virtual void resize(uint width, uint height) override;

		//! Swap front and back buffers (OS-specific)
		virtual void swapBuffers() override { ::SwapBuffers(pHDC); }

		//! Refresh and swap buffers with a single polymorphic call
		virtual void refreshAndSwap() override { onRefresh(); ::SwapBuffers(pHDC); }

		//! Set window icon
		virtual void icon(const AnyString& path) override;

		//! Set window title
		virtual void title(const AnyString& path) override;

		//! Enable / Disable vertical sync (OS-specific)
		virtual void vsync(bool enable) override;
		//! Current status of the vertical sync (OS-specific)
		virtual bool vsync() const override;

		/*!
		** \brief Should Full Screen AntiAliasing / MultiSampling be enabled ?
		**
		** Changing this value may kill and re-create the window.
		*/
		virtual void multiSampling(MultiSampling::Type samplingType) override;

		//! Enable / Disable full screen
		virtual void fullScreen(bool enable) override;
		//! Is the window full screen ?
		virtual bool fullScreen() const override;

	private:
		/*!
		** \brief Callback method for windows events
		**
		** \param hWnd Handle for this window
		** \param uMsg Message
		** \param wParam Additional Message Information
		** \param lParam Additional Message Information
		*/
		static LRESULT CALLBACK MessageCallback(HWND handle, UINT uMsg, WPARAM wParam, LPARAM lParam);

		//! Find a window given its handle
		static WGLWindow* FindWindow(HWND handle);

		//! Register a window with its handle as a key
		static void RegisterWindow(HWND handle, WGLWindow* window);

		//! Unregister the window, happens when closing it
		static void UnregisterWindow(HWND handle);

		//! Current number of registered windows
		static uint WindowCount();

		//! Convert a Windows VK_* code to the equivalent Yuni code
		static Yuni::Input::Key ConvertInputKey(WPARAM keyCode);

		//! Enable full screen mode, use primary monitor with desktop resolution
		bool enableFullScreen();

		//! Disable full screen mode
		bool disableFullScreen();

		//! Window initialization
		bool initWindow();

		//! Close and release all the necessary stuff to reinitialize the window
		void closeWindowForReinit();

		//! Initialize the pixel format with default values
		bool initDefaultPixelFormat(const PIXELFORMATDESCRIPTOR& pfd);

		//! Initialize the pixel format for multi-sampling
		bool initMultisamplePixelFormat(const PIXELFORMATDESCRIPTOR& pfd, bool set);

		//! Only resize the internal data, this supposes the window is actually already resized
		void internalResize(uint width, uint height);

	protected:
		//! Static list of registered GDI windows
		static WindowList sWindowList;

		// # ifdef YUNI_OS_WINDOWS
		friend LRESULT CALLBACK MessageCallback(HWND handle, UINT uMsg, WPARAM wParam, LPARAM lParam);
		// # endif

	private:
		HINSTANCE pHInstance;
		HWND pHWnd;
		HDC pHDC;
		HGLRC pHRC;

		long pPrevStyle;
		long pPrevExStyle;
		bool pPrevMaximized;
		RECT pPrevRect;

		//! True if the window has already been created with a MultiSampling pixel format.
		bool pHasMSAASupport;

		Logs::Logger<> logs;

	}; // class WGLWindow






} // namespace UI
} // namespace Yuni

#endif // __YUNI_UI_WGLWINDOW_H__
