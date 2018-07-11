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
#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/core/string/wstring.h>
#include <yuni/private/graphics/opengl/glew/glew.h>
#include <yuni/private/graphics/opengl/glew/wglew.h>
#include <yuni/io/directory.h>
#include <yuni/io/filename-manipulation.h>
#include <iostream>
#include <windowsx.h> // for GET_X_LPARAM / GET_Y_LPARAM

#include "../windowfactory.h"
#include "wglwindow.h"

namespace Yuni
{
namespace UI
{

	namespace // anonymous
	{

		// Windows virtual key codes
		// source : http://www.kbdedit.com/manual/low_level_vk_list.html
		const Input::Key KeyConversions[] =
		{
			Input::Key::Invalid,	// 00 	Windows key codes start at 01
			Input::Key::Invalid,	// 01 	Left mouse button
			Input::Key::Invalid,	// 02 	Right mouse button
			Input::Key::Cancel, 	// 03 	Control-break processing
			Input::Key::Invalid,	// 04 	Middle mouse button on a three-button mouse
			Input::Key::Invalid,	// 05-07 	Undefined
			Input::Key::Invalid,
			Input::Key::Invalid,
			Input::Key::Backspace, 	// 08 	BACKSPACE key
			Input::Key::Tab,		// 09 	TAB key
			Input::Key::Invalid,	// 0A-0B 	Undefined
			Input::Key::Invalid,
			Input::Key::Clear,		// 0C 	CLEAR key
			Input::Key::Enter,		// 0D 	ENTER key
			Input::Key::Invalid,	// 0E-0F 	Undefined
			Input::Key::Invalid,
			Input::Key::ShiftL, 	// 10 	SHIFT key
			Input::Key::ControlL, 	// 11 	CTRL key
			Input::Key::AltL,	 	// 12 	ALT key
			Input::Key::Pause,		// 13 	PAUSE key
			Input::Key::CapsLock, 	// 14 	CAPS LOCK key
			Input::Key::Invalid,	// 15-19 	Reserved for Kanji systems
			Input::Key::Invalid,
			Input::Key::Invalid,
			Input::Key::Invalid,
			Input::Key::Invalid,
			Input::Key::Invalid, 	// 1A 	Undefined
			Input::Key::Escape, 	// 1B 	ESC key
			Input::Key::Invalid,	// 1C-1F 	Reserved for Kanji systems
			Input::Key::Invalid,
			Input::Key::Invalid,
			Input::Key::Invalid,
			Input::Key::Space,		// 20 	SPACEBAR
			Input::Key::PageUp,		// 21 	PAGE UP key
			Input::Key::PageDown, 	// 22 	PAGE DOWN key
			Input::Key::End,		// 23 	END key
			Input::Key::Home,		// 24 	HOME key
			Input::Key::Left,		// 25 	LEFT ARROW key
			Input::Key::Up,			// 26 	UP ARROW key
			Input::Key::Right,	 	// 27 	RIGHT ARROW key
			Input::Key::Down,	 	// 28 	DOWN ARROW key
			Input::Key::Select, 	// 29 	SELECT key
			Input::Key::Invalid,	// 2A 	Specific to original equipment manufacturer
			Input::Key::Execute, 	// 2B 	EXECUTE key
			Input::Key::PrintScreen,// 2C 	PRINT SCREEN key
			Input::Key::Insert, 	// 2D 	INS key
			Input::Key::Delete, 	// 2E 	DEL key
			Input::Key::Help,		// 2F 	HELP key
			Input::Key::N0,			// 30	Digit 0
			Input::Key::N1,			// 31	Digit 1
			Input::Key::N2,			// 32	Digit 2
			Input::Key::N3,			// 33	Digit 3
			Input::Key::N4,			// 34	Digit 4
			Input::Key::N5,			// 35	Digit 5
			Input::Key::N6,			// 36	Digit 6
			Input::Key::N7,			// 37	Digit 7
			Input::Key::N8,			// 38	Digit 8
			Input::Key::N9,			// 39	Digit 9
			Input::Key::Invalid,	// 3A-40 	Undefined
			Input::Key::Invalid,
			Input::Key::Invalid,
			Input::Key::Invalid,
			Input::Key::Invalid,
			Input::Key::Invalid,
			Input::Key::Invalid,
			Input::Key::A,			// 41	Letter A (corresponds to ascii)
			Input::Key::B,
			Input::Key::C,
			Input::Key::D,
			Input::Key::E,
			Input::Key::F,
			Input::Key::G,
			Input::Key::H,
			Input::Key::I,
			Input::Key::J,
			Input::Key::K,
			Input::Key::L,
			Input::Key::M,
			Input::Key::N,
			Input::Key::O,
			Input::Key::P,
			Input::Key::Q,
			Input::Key::R,
			Input::Key::S,
			Input::Key::T,
			Input::Key::U,
			Input::Key::V,
			Input::Key::W,
			Input::Key::X,
			Input::Key::Y,
			Input::Key::Z,			// 5A	Letter Z (corresponds to ascii)
			Input::Key::MetaL,		// 5B 	Left Windows key on a Microsoft Natural Keyboard
			Input::Key::MetaR,		// 5C 	Right Windows key on a Microsoft Natural Keyboard
			Input::Key::Invalid, 	// 5D 	Applications key on a Microsoft Natural Keyboard
			Input::Key::Invalid,	// 5E-5F 	Undefined
			Input::Key::Invalid,
			Input::Key::KP0,	 	// 60 	Numeric keypad 0 key
			Input::Key::KP1,		// 61 	Numeric keypad 1 key
			Input::Key::KP2,	 	// 62 	Numeric keypad 2 key
			Input::Key::KP3,		// 63 	Numeric keypad 3 key
			Input::Key::KP4,		// 64 	Numeric keypad 4 key
			Input::Key::KP5,		// 65 	Numeric keypad 5 key
			Input::Key::KP6,		// 66 	Numeric keypad 6 key
			Input::Key::KP7,		// 67 	Numeric keypad 7 key
			Input::Key::KP8,		// 68 	Numeric keypad 8 key
			Input::Key::KP9,		// 69 	Numeric keypad 9 key
			Input::Key::KPAsterisk, // 6A 	Multiply key
			Input::Key::KPPlus, 	// 6B 	Add key
			Input::Key::KPEnter,	// 6C 	Separator key
			Input::Key::KPMinus, 	// 6D 	Subtract key
			Input::Key::KPDelete, 	// 6E 	Decimal key
			Input::Key::KPSlash, 	// 6F 	Divide key
			Input::Key::F1,			// 70 	F1 key
			Input::Key::F2,			// 71 	F2 key
			Input::Key::F3,			// 72 	F3 key
			Input::Key::F4,			// 73 	F4 key
			Input::Key::F5,			// 74 	F5 key
			Input::Key::F6,			// 75 	F6 key
			Input::Key::F7,			// 76 	F7 key
			Input::Key::F8,			// 77 	F8 key
			Input::Key::F9,			// 78 	F9 key
			Input::Key::F10,		// 79 	F10 key
			Input::Key::F11,		// 7A 	F11 key
			Input::Key::F12,		// 7B 	F12 key
			Input::Key::F13,		// 7C 	F13 key
			Input::Key::F14,		// 7D 	F14 key
			Input::Key::F15,		// 7E 	F15 key
			Input::Key::F16,		// 7F 	F16 key
			Input::Key::F17,		// 80 	F17 key
			Input::Key::F18,		// 81 	F18 key
			Input::Key::F19,		// 82 	F19 key
			Input::Key::F20,		// 83 	F20 key
			Input::Key::F21,		// 84 	F21 key
			Input::Key::F22,		// 85 	F22 key
			Input::Key::F23,		// 86 	F23 key
			Input::Key::F24,		// 87 	F24 key
			Input::Key::Invalid,	// 88-8F 	Unassigned
			Input::Key::Invalid,
			Input::Key::Invalid,
			Input::Key::Invalid,
			Input::Key::Invalid,
			Input::Key::Invalid,
			Input::Key::Invalid,
			Input::Key::Invalid,
			Input::Key::NumLock, 	// 90	NUM LOCK key
			Input::Key::ScrollLock, // 91	SCROLL LOCK key
			Input::Key::ShiftL, 	// A0	Left SHIFT
			Input::Key::ShiftR, 	// A1	Right SHIFT
			Input::Key::ControlL, 	// A2	Left CTRL
			Input::Key::ControlR, 	// A3	Right CTRL
			Input::Key::AltL,		// A4	Left ALT
			Input::Key::AltR,		// A5	Right ALT
			Input::Key::Invalid,	// A6	Browser Back
			Input::Key::Invalid,	// A7	Browser Forward
			Input::Key::Invalid,	// A8	Browser Refresh
			Input::Key::Invalid,	// A9	Browser Stop
			Input::Key::Invalid,	// AA	Browser Search
			Input::Key::Invalid,	// AB	Browser Favorites
			Input::Key::Invalid,	// AC	Browser Home
			Input::Key::Invalid,	// AD	Volume Mute
			Input::Key::Invalid,	// AE	Volume Down
			Input::Key::Invalid,	// AF	Volume Up
			Input::Key::Invalid,	// B0	Media Next Track
			Input::Key::Invalid,	// B1	Media Previous Track
			Input::Key::Invalid,	// B2	Media Stop
			Input::Key::Invalid,	// B3	Media Play / Pause
			Input::Key::Invalid,	// B4	Mail
			Input::Key::Invalid,	// B5	Media Select
			Input::Key::Invalid,	// B6	App 1
			Input::Key::Invalid,	// B7	App 2
			Input::Key::Invalid,	// B8-B9	Unknown
			Input::Key::Invalid,
			Input::Key::Semicolon,	// BA	: ;
			Input::Key::Equal,		// BB	+ =
			Input::Key::Comma,		// BC	< ,
			Input::Key::Minus,		// BD	_ -
			Input::Key::Dot,		// BE	> .
			Input::Key::Slash,		// BF	? /
			Input::Key::Backquote,	// C0	~ `
			Input::Key::Invalid,	// C1	Abnt C1
			Input::Key::Invalid,	// C2	Abnt C2
		};

	} // namespace anonymous


	inline Yuni::Input::Key WGLWindow::ConvertInputKey(WPARAM keyCode)
	{
		if (keyCode > 0xAF)
			return Input::Key::Invalid;
		return KeyConversions[keyCode];
	}


	// Definition of the static variable
	WGLWindow::WindowList WGLWindow::sWindowList;


	WGLWindow* WGLWindow::FindWindow(HWND handle)
	{
		if (nullptr != handle)
		{
			const WindowList::iterator end = sWindowList.end();
			for (WindowList::iterator it = sWindowList.begin(); it != end; ++it)
			{
				if (handle == it->first)
					return it->second;
			}
		}
		return nullptr;
	}


	void WGLWindow::RegisterWindow(HWND handle, WGLWindow* window)
	{
		// If handle is already in the list, it will be overwritten
		if (nullptr != handle and window)
			sWindowList[handle] = window;
	}


	void WGLWindow::UnregisterWindow(HWND handle)
	{
		sWindowList.erase(handle);
	}


	uint WGLWindow::WindowCount()
	{
		return (uint)sWindowList.size();
	}


	LRESULT CALLBACK WGLWindow::MessageCallback(HWND handle, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		WGLWindow* window = Yuni::UI::WGLWindow::FindWindow(handle);

		if (!window or !::IsWindowVisible(handle))
			return ::DefWindowProc(handle, uMsg, wParam, lParam);

		switch (uMsg)
		{
			case WM_PAINT:
			{
				window->refreshAndSwap();
				break;
			}

			case WM_MOUSEMOVE:
			{
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);
				window->doMouseMove(x, y);
				break;
			}

			case WM_ERASEBKGND:
			{
				// Do not erase background, this prevents flickering during move / resize.
				return 0;
			}

			// Input

			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
			{
				window->doKeyDown(ConvertInputKey(wParam));
				break;
			}

			case WM_KEYUP:
			case WM_SYSKEYUP:
			{
				window->doKeyUp(ConvertInputKey(wParam));
				break;
			}

			case WM_MOUSEHOVER:
			{
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);
				window->doMouseHover(x, y);
				break;
			}

			case WM_MOUSELEAVE:
			{
				window->doMouseLeave();
				break;
			}

			case WM_CHAR:
			{
				int nbChar = ::WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)&wParam, 1, nullptr, 0, nullptr, nullptr);
				if (nbChar < 1)
					return 0;
				char uniChar[nbChar + 1];
				nbChar = ::WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)&wParam, 1, uniChar, nbChar, nullptr, nullptr);
				if (nbChar < 1)
					return 0;
				uniChar[nbChar] = '\0';
				String str;
				str.resize((uint)(lParam & 0xFF) * (uint)nbChar);
				str.fill((char*)uniChar);
				window->doCharInput(str);
				return 0;
			}

			case WM_LBUTTONDOWN:
			{
				window->doMouseDown(Input::IMouse::ButtonLeft);
				break;
			}

			case WM_RBUTTONDOWN:
			{
				window->doMouseDown(Input::IMouse::ButtonRight);
				break;
			}

			case WM_MBUTTONDOWN:
			{
				window->doMouseDown(Input::IMouse::ButtonMiddle);
				break;
			}

			case WM_LBUTTONUP:
			{
				window->doMouseUp(Input::IMouse::ButtonLeft);
				break;
			}

			case WM_RBUTTONUP:
			{
				window->doMouseUp(Input::IMouse::ButtonRight);
				break;
			}

			case WM_MBUTTONUP:
			{
				window->doMouseUp(Input::IMouse::ButtonMiddle);
				break;
			}

			case WM_LBUTTONDBLCLK:
			{
				window->doMouseDblClick(Input::IMouse::ButtonLeft);
				break;
			}

			case WM_RBUTTONDBLCLK:
			{
				window->doMouseDblClick(Input::IMouse::ButtonRight);
				break;
			}

			case WM_MOUSEWHEEL:
			{
				window->doMouseScroll(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
				break;
			}

			// System

			case WM_SYSCOMMAND:
			{
				switch (wParam)
				{
					case SC_SCREENSAVE:
					case SC_MONITORPOWER:
						// Ignore screen saver and monitor power economy modes.
						return 0;
					case SC_MINIMIZE:
						window->pState = wsMinimized;
						break;
					case SC_MAXIMIZE:
					{
						// int width;
						// int height;
						// ::GetClientRect(&width, &height);
						// window->resize(width, height);
						window->pState = wsMaximized;
						break;
					}
					case SC_RESTORE:
						window->pState = wsNormal;
						break;
				}
				break;
			}

			// Close

			case WM_CLOSE:
			case WM_DESTROY:
			{
				window->kill();
				return 0;
			}

			// Move / Resize

			case WM_WINDOWPOSCHANGED:
			{
				WINDOWPOS* position = (WINDOWPOS*)lParam;
				window->move(position->x, position->y);
				break;
			}

			case WM_SIZE:
			{
				window->internalResize(LOWORD(lParam), HIWORD(lParam));
				break;
			}

			case WM_SIZING:
			{
				RECT* rect = (RECT*)lParam;
				window->internalResize(rect->right - rect->left, rect->bottom - rect->top);
				break;
			}

			case WM_GETMINMAXINFO:
			{
				MINMAXINFO* mmiStruct = (MINMAXINFO*)lParam;

				POINT ptPoint;

				// Minimum width of the window
				ptPoint.x = 300;
				// Minimum height of the window
				ptPoint.y = 200;
				mmiStruct->ptMinTrackSize = ptPoint;

				// Maximum width of the window
				ptPoint.x = ::GetSystemMetrics(SM_CXMAXIMIZED);
				// Maximum height of the window
				ptPoint.y = ::GetSystemMetrics(SM_CYMAXIMIZED);
				mmiStruct->ptMaxTrackSize = ptPoint;
				break;
			}

		} // End switch

		// Pass all unhandled messages to DefWindowProc
		return ::DefWindowProc(handle, uMsg, wParam, lParam);
	}




	WGLWindow::~WGLWindow()
	{
		if (pHWnd)
			kill();

		// Unregister Window Class only when all windows are gone
		if (WindowCount() == 0 and not ::UnregisterClass(L"OpenGL", pHInstance))
		{
			std::cerr << "Window closing error : Could not unregister Window Class !" << std::endl;
		}
	}


	bool WGLWindow::initialize()
	{
		// Grab an instance for our window
		pHInstance = ::GetModuleHandle(nullptr);

		// Windows Class structure
		WNDCLASS wc;
		// Redraw on Size, and own DC for window
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		// WndProc handles messages
		wc.lpfnWndProc = &WGLWindow::MessageCallback;
		// No Extra Window Data
		wc.cbClsExtra = 0;
		// No Extra Window Data
		wc.cbWndExtra = 0;
		// Set the instance
		wc.hInstance = pHInstance;
		// Load the default icon
		wc.hIcon = ::LoadIcon(nullptr, IDI_WINLOGO);
		// Load the arrow pointer
		wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
		// No background required for GL
		wc.hbrBackground = nullptr;
		// We don't want a menu
		wc.lpszMenuName = nullptr;
		// Set the Class Name
		wc.lpszClassName = L"OpenGL";

		// Attempt to register the Window Class only when we have no window yet
		if (!WindowCount() and !::RegisterClassW(&wc))
		{
			std::cerr << "Window init error : Failed to register the window class." << std::endl;
			return false;
		}

		return initWindow();
	}


	bool WGLWindow::enableFullScreen()
	{
		if (pState == wsMaximized)
		{
			pPrevMaximized = true;
			::SendMessage(pHWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
		}
		else
			pPrevMaximized = false;
		pPrevStyle = ::GetWindowLong(pHWnd, GWL_STYLE);
		pPrevExStyle = ::GetWindowLong(pHWnd, GWL_EXSTYLE);
		::GetWindowRect(pHWnd, &pPrevRect);

		// Set new window style and size.
		::SetWindowLong(pHWnd, GWL_STYLE,
			pPrevStyle & ~(WS_CAPTION | WS_THICKFRAME));
		::SetWindowLong(pHWnd, GWL_EXSTYLE,
			pPrevExStyle & ~(WS_EX_DLGMODALFRAME |
			WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));

		// On expand, if we're given a window_rect, grow to it, otherwise do
		// not resize.
		MONITORINFO monitorInfo;
		monitorInfo.cbSize = sizeof(monitorInfo);
		::GetMonitorInfo(::MonitorFromWindow(pHWnd, MONITOR_DEFAULTTONEAREST), &monitorInfo);
		::SetWindowPos(pHWnd, nullptr, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
			monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
			monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
			SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

		return true;
	}


	bool WGLWindow::disableFullScreen()
	{
		// Reset the window style
		::SetWindowLong(pHWnd, GWL_STYLE, pPrevStyle);
		::SetWindowLong(pHWnd, GWL_EXSTYLE, pPrevExStyle);

		// Adjust window to proper size
		::SetWindowPos(pHWnd, nullptr, pPrevRect.left,  pPrevRect.top,
			pPrevRect.right - pPrevRect.left,
			pPrevRect.bottom - pPrevRect.top,
			SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

		if (pPrevMaximized)
			::SendMessage(pHWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);

		return true;
	}


	bool WGLWindow::initWindow()
	{
		DWORD dwStyle = WS_OVERLAPPEDWINDOW;
		DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

		// Adjust window to true requested size
		RECT windowRect;
		windowRect.left = pLeft;
		windowRect.right = (long)pWidth;
		windowRect.top = pTop;
		windowRect.bottom = (long)pHeight;
		::AdjustWindowRectEx(&windowRect, dwStyle, false, dwExStyle);

		// Create the window
		if (!(pHWnd = ::CreateWindowEx(
			dwExStyle, L"OpenGL", WString{pTitle}.c_str(),
			dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			pLeft, pTop, // Position
			windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, // Dimensions
			nullptr, nullptr, pHInstance, nullptr)))
		{
			std::cerr << "Window init error : Window creation failed !" << std::endl;
			std::cerr << "Error : \"" << GetLastError() << "\"" << std::endl;
			kill();
			return false;
		}

		RegisterWindow(pHWnd, this);


		// Get the Device Context
		if (!(pHDC = ::GetDC(pHWnd)))
		{
			std::cerr << "Window init error : Can't create a Device Context !" << std::endl;
			kill();
			return false;
		}

		const ::PIXELFORMATDESCRIPTOR pfd =
			{
				// Size
				(WORD)sizeof (::PIXELFORMATDESCRIPTOR),
				// Version
				1,
				// Flags
				PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_GENERIC_ACCELERATED,
				// Pixel Type
				PFD_TYPE_RGBA,
				// Color depth
				(BYTE)pBitDepth,
				// Color bits ignored
				0, 0, 0, 0, 0, 0,
				// No alpha buffer
				0,
				// Shift bit ignored
				0,
				// No accumulation buffer
				0,
				// Accumulation bits ignored
				0, 0, 0, 0,
				// 24-bit Z-buffer (Depth Buffer)
				24,
				// 8-bit stencil buffer
				8,
				// No auxiliary buffer
				0,
				// Main drawing layer
				PFD_MAIN_PLANE,
				// Reserved
				0,
				// Layer masks ignored
				0, 0, 0
			};

		// Try MSAA init
		if (pHasMSAASupport && pMultiSampling != MultiSampling::msNone)
		{
			if (!initMultisamplePixelFormat(pfd, true))
			{
				// If MSAA init failed, set the requested multisampling to none
				pMultiSampling = MultiSampling::msNone;
				initDefaultPixelFormat(pfd); // Should work, since it worked once before
			}
		}
		else
		{
			// Dummy format to create a GL context
			if (!initDefaultPixelFormat(pfd))
			{
				std::cerr << "Window init error : Can't set pixel format !" << std::endl;
				kill();
				return false;
			}
		}

		// Create a Rendering Context if necessary
		if (!(pHRC = ::wglCreateContext(pHDC)))
		{
			std::cerr << "Window init error : Can't create a GL rendering context !" << std::endl;
			kill();
			return false;
		}

		// Activate the Rendering Context
		if (!::wglMakeCurrent(pHDC, pHRC))
		{
			std::cerr << "Window init error : Can't activate the GL rendering context !" << std::endl;
			kill();
			return false;
		}

		// Initialize our newly created GL window
		if (!GLWindow::initialize())
		{
			std::cerr << "OpenGL Initialization Failed !" << std::endl;
			kill();
			return false;
		}

		if (!pHasMSAASupport && pMultiSampling != MultiSampling::msNone &&
			initMultisamplePixelFormat(pfd, false))
		{
			pHasMSAASupport = true;
			closeWindowForReinit();
			::wglDeleteContext(pHRC);
			initWindow();
		}

		// On success, enable multisampling
		if (pHasMSAASupport && pMultiSampling != MultiSampling::msNone)
			::glEnable(GL_MULTISAMPLE);
		else
			::glDisable(GL_MULTISAMPLE);

		// Show the window
		::ShowWindow(pHWnd,SW_SHOW);
		// Slightly higher priority
		::SetForegroundWindow(pHWnd);
		// Sets keyboard focus to the window
		::SetFocus(pHWnd);

		// Set up our perspective GL screen
		internalResize(pWidth, pHeight);

		TRACKMOUSEEVENT tme;
		tme.cbSize = (DWORD)sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.hwndTrack = pHWnd;
		tme.dwHoverTime = HOVER_DEFAULT;
		::TrackMouseEvent(&tme);

		if (pFullScreen)
		{
			if (!enableFullScreen())
				pFullScreen = false;
		}

		return true;
	}


	bool WGLWindow::initDefaultPixelFormat(const PIXELFORMATDESCRIPTOR& pfd)
	{
		uint pixelFormat;
		// Did Windows find a matching pixel format?
		if (!(pixelFormat = ::ChoosePixelFormat(pHDC, &pfd)))
		{
			std::cerr << "Window init error : Cannot find a suitable Pixel Format !" << std::endl;
			kill();
			return false;
		}

		// Are we able to set the pixel format?
		if (!::SetPixelFormat(pHDC, pixelFormat, &pfd))
		{
			std::cerr << "Window init error : Failed to set the Pixel Format !" << std::endl;
			kill();
			return false;
		}
		return true;
	}


	bool WGLWindow::initMultisamplePixelFormat(const PIXELFORMATDESCRIPTOR& pfd, bool set)
	{
		// Make sure we have multisampling support
		if (not ::glewIsSupported("GL_ARB_multisample") or
			not ::wglewIsSupported("WGL_ARB_multisample"))
		{
			std::cerr << "Window init error : Multisampling is not available, falling back to standard pixel format !" << std::endl;
			return false;
		}

		// Get our pixel format
		// PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)::wglGetProcAddress("wglChoosePixelFormatARB");
		if (!wglChoosePixelFormatARB)
		 	return false;

		int pixelFormat;
		UINT numFormats;

		uint msMultiplier = samplingMultiplier();

		// Get the data from the pixel format descriptor
		// except the SAMPLE_BUFFERS_ARB and SAMPLES_ARB, that manage multisampling
		int iAttributes[] =
			{
				WGL_DRAW_TO_WINDOW_ARB, true, // Draw to window
				WGL_SUPPORT_OPENGL_ARB, true, // OpenGL
				WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB, // Hardware acceleration
				WGL_COLOR_BITS_ARB, (int)pBitDepth,
				WGL_ALPHA_BITS_ARB, 0,
				WGL_DEPTH_BITS_ARB, 24,
				WGL_STENCIL_BITS_ARB, 8,
				WGL_DOUBLE_BUFFER_ARB, true, // Double buffering
				WGL_SAMPLE_BUFFERS_ARB, true, // Activate multisampling
				WGL_SAMPLES_ARB, (int)msMultiplier, // Specify number of samples
				WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
				0
			};

		// Check if we can get a Pixel Format for the wanted number of samples
		bool valid = ::wglChoosePixelFormatARB(pHDC, iAttributes, nullptr, 1, &pixelFormat, &numFormats);
		// Check that the call succeeded and our format count is greater or equal to 1
		if (!valid or numFormats < 1)
		{
			std::cerr << "Window init error : No pixel format found for multisampling !" << std::endl;
			return false;
		}

		if (set)
		{
			// Are we able to set the pixel format?
			if (!::SetPixelFormat(pHDC, pixelFormat, &pfd))
			{
				std::cerr << "Window init error : Failed to set the Pixel Format !" << std::endl;
				kill();
				return false;
			}

			# ifndef NDEBUG
			std::cout << msMultiplier << "x MSAA activated !" << std::endl;
			# endif
		}

		return true;
	}


	void WGLWindow::icon(const AnyString& path)
	{
		// Load 32x32 icon (alt-tab menu)
		WString wstr(path);
		const HICON hIcon = (HICON)::LoadImage(nullptr, wstr.c_str(), IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
		if (hIcon)
			::SendMessage(pHWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		else
			std::cerr << "Error : Could not load large icon : \"" << path << "\"" << std::endl;

		// Load 16x16 icon (window title bar and task bar)
		const HICON hIconSmall = (HICON)::LoadImage(nullptr, wstr.c_str(), IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
		if (hIconSmall)
			::SendMessage(pHWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
		else
			std::cerr << "Error : Could not load small icon : \"" << path << "\"" << std::endl;
	}


	void WGLWindow::title(const AnyString& title)
	{
		if (!::SetWindowText(pHWnd, WString{ title }.c_str()))
			std::cerr << "Error : Could not change window title to : \"" << title << "\"" << std::endl;
	}


	void WGLWindow::vsync(bool enable)
	{
		::wglSwapIntervalEXT(enable ? 1 : 0);
	}


	bool WGLWindow::vsync() const
	{
		return ::wglGetSwapIntervalEXT();
	}


	void WGLWindow::multiSampling(MultiSampling::Type samplingType)
	{
		// No change
		if (samplingType == pMultiSampling)
			return;

		// Not supported (also means it could not have been activated before)
		if (not ::glewIsSupported("GL_ARB_multisample") or
			not ::wglewIsSupported("WGL_ARB_multisample"))
			return;

		// Store the old vsync because we will lose it otherwise
		bool hasVsync = vsync();
		// Try to change the FSAA by klling the window and creating it again with the new values
		closeWindowForReinit();
		pMultiSampling = samplingType;
		if (!initWindow())
			// Init window will generate errors by itself if it fails
			// It will automatically go back to a default non-fsaa state on error
			return;
		// Reapply vsync
		vsync(hasVsync);
		if (MultiSampling::msNone != pMultiSampling)
			::glEnable(GL_MULTISAMPLE);
		else
			::glDisable(GL_MULTISAMPLE);
	}


	void WGLWindow::resize(uint width, uint height)
	{
		if (width != this->width() || height != this->height())
		{
			internalResize(width, height);
			::SetWindowPos(pHWnd, nullptr, 0, 0, width, height, SWP_NOMOVE);
		}
	}


	void WGLWindow::internalResize(uint width, uint height)
	{
		GLWindow::resize(width, height);
	}


	void WGLWindow::fullScreen(bool enable)
	{
		// No change
		if (enable == pFullScreen)
			return;

		pFullScreen = enable;
		if (enable)
		{
			if (!enableFullScreen())
				pFullScreen = false;
		}
		else
			disableFullScreen();
	}


	bool WGLWindow::fullScreen() const
	{
		return pFullScreen;
	}


	bool WGLWindow::loop()
	{
		if (killed() || !pHWnd || !pHRC)
			return false;

		MSG msg;
		if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			// Check for a quit message
			if (msg.message == WM_QUIT )
				return false;
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		return true;
	}


	void WGLWindow::closeWindowForReinit()
	{
		// Are We In Fullscreen Mode?
		if (pFullScreen)
		{
			// Switch back to the desktop
			::ChangeDisplaySettings(nullptr, 0);
			// Show Mouse Pointer
			::ShowCursor(true);
		}

		// Release the DC And RC contexts
		if (pHRC and !::wglMakeCurrent(nullptr, nullptr))
			std::cerr << "GL closing error : Could not disable current context !" << std::endl;

		// Release the DC
		if (pHDC and !ReleaseDC(pHWnd, pHDC))
		{
			std::cerr << "Window closing error : Device Context release failed." << std::endl;
		}
		pHDC = nullptr;

		// Remove reference to the window at the last possible moment
		UnregisterWindow(pHWnd);

		// Destroy the window
		if (pHWnd and !DestroyWindow(pHWnd))
		{
			std::cerr << "Window closing error : Could not destroy window !" << std::endl;
		}
		pHWnd = nullptr;
	}


	void WGLWindow::kill()
	{
		GLWindow::kill();

		closeWindowForReinit();

		// Delete the RC
		if (pHRC and !::wglDeleteContext(pHRC))
		{
			std::cerr << "Window closing error : Rendering Context release failed !" << std::endl;
		}
		pHRC = nullptr;
	}


	RenderWindow* WindowFactory::CreateGLWindow(const AnyString& title, uint width,
		uint height, uint bits, bool fullScreen)
	{
		RenderWindow* wnd = new WGLWindow(title, width, height, bits, fullScreen);
		if (!wnd->initialize())
		{
			wnd->kill();
			delete wnd;
			return nullptr;
		}
		return wnd;
	}




} // namespace UI
} // namespace Yuni
