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
#include "../../yuni.h"

#include "../windowfactory.h"
#include "nsglwindow.h"
#include "../../thread/utility.h"

/// A reference to NSApp.  Always a good idea, seeing as it's probably the most helpful thing in CocoaLand
extern id NSApp;


namespace Yuni
{
namespace UI
{

    namespace // anonymous
    {

		bool OnAppDidFinishLaunching(AppDelegate* self, SEL _cmd, id notification)
		{
			self->window = cmacs_simple_msgSend((id)objc_getClass("NSWindow"), sel_getUid("alloc"));

			/// Create an instance of the window.
			self->window = cmacs_window_init_msgSend(self->window, sel_getUid("initWithContentRect:styleMask:backing:defer:"), (CMRect){ {0,0},{1024,460}},
				(NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask | NSMiniaturizableWindowMask), 0, false);
	    
			/// Create an instance of our view class.
			///
			/// Relies on the view having declared a constructor that allocates a class pair for it.
			id view = cmacs_rect_msgSend1(cmacs_simple_msgSend((id)objc_getClass("YView"), sel_getUid("alloc")), sel_getUid("initWithFrame:"), (CMRect){ {0, 0}, {320, 480} });

			// here we simply add the view to the window.
			cmacs_void_msgSend1(self->window, sel_getUid("setContentView:"), view);
			cmacs_simple_msgSend(self->window, sel_getUid("becomeFirstResponder"));

			// Shows our window in the bottom-left hand corner of the screen.
			//cmacs_void_msgSend1(self->window, sel_getUid("makeKeyAndOrderFront:"), self);
		}

    } // namespace anonymous



	NSGLWindow::WindowList NSGLWindow::sWindowList;


	void NSGLWindow::OnDrawRect(id self, SEL _cmd, CMRect rect)
	{
		NSGLWindow* window = NSGLWindow::FindWindow(self);
		if (nullptr != window)
			window->refreshAndSwap();
	}

	void NSGLWindow::OnMouseDown(id self, SEL _cmd, void* event)
	{
		NSGLWindow* window = NSGLWindow::FindWindow(self);
		if (nullptr != window)
			window->doMouseDown(Input::IMouse::ButtonLeft);
	}

	/*	void NSGLWindow::OnWindowClose(id self, SEL _cmd, )
	{
		NSGLWindow* window = NSGLWindow::FindWindow(self);
		if (nullptr != window)
			window->kill();
			}*/


	NSGLWindow* NSGLWindow::FindWindow(id windowId)
	{
		if (nullptr != windowId)
		{
			auto it = sWindowList.find(windowId);
			if (sWindowList.end() != it)
				return it->second;
		}
		return nullptr;
	}


	void NSGLWindow::RegisterWindow(id windowId, NSGLWindow* window)
	{
		// If handle is already in the list, it will be overwritten
		if (nullptr != windowId and window)
			sWindowList[windowId] = window;
	}


	void NSGLWindow::UnregisterWindow(id windowId)
	{
		sWindowList.erase(windowId);
	}




	NSGLWindow::~NSGLWindow()
	{
		kill();
	}


	bool NSGLWindow::initialize()
	{
		// Strong reference to the class of the AppDelegate (same as [AppDelegate class])
		static Class appDelClass = nullptr;
		if (not appDelClass)
		{
			appDelClass = objc_allocateClassPair((Class)objc_getClass("NSObject"), "AppDelegate", 0);
			class_addMethod(appDelClass, sel_getUid("applicationDidFinishLaunching:"), (IMP)OnAppDidFinishLaunching, "i@:@");
			objc_registerClassPair(appDelClass);
		}

		// Tell the runtime to create a new class, a subclass of 'NSOpenGLView' named 'YView'.
		static Class viewClass = nullptr;
		if (not viewClass)
		{
			viewClass = objc_allocateClassPair((Class)objc_getClass("NSOpenGLView"), "YView", 0);

			// Tell the runtime to add functions for various events to our custom view.
			class_addMethod(viewClass, sel_getUid("drawRect:"), (IMP)NSGLWindow::OnDrawRect, "v@:");
			class_addMethod(viewClass, sel_getUid("mouseDown:"), (IMP)NSGLWindow::OnMouseDown, "v@:");
			//class_addMethod(viewClass, sel_getUid("close:"), (IMP)NSGLWindow::OnWindowClose, "v@:");

			// And again, we tell the runtime that this class is now valid to be used.
			// At this point, the application should run and display the screenshot shown below.
			objc_registerClassPair(viewClass);
		}

		cmacs_simple_msgSend((id)objc_getClass("NSApplication"), sel_getUid("sharedApplication"));
		if (not NSApp)
		{
			std::cerr << "Failed to initialized NSApplication...  terminating..." << std::endl;
			return false;
		}

		// Create the app delegate
		static AppDelegate* appDelObj = nullptr;
		if (not appDelObj)
		{
			appDelObj = (AppDelegate*)cmacs_simple_msgSend((id)objc_getClass("AppDelegate"), sel_getUid("alloc"));
			appDelObj = (AppDelegate*)cmacs_simple_msgSend((id)appDelObj, sel_getUid("init"));

			cmacs_void_msgSend1(NSApp, sel_getUid("setDelegate:"), appDelObj);

			// Launch main loop
			//cmacs_void_msgSend(NSApp, sel_getUid("run"));
		}

		appDelObj->window = cmacs_simple_msgSend((id)objc_getClass("NSOpenGLWindow"), sel_getUid("alloc"));

		/// Create an instance of the window.
		appDelObj->window = cmacs_window_init_msgSend(appDelObj->window, sel_getUid("initWithContentRect:styleMask:backing:defer:"), (CMRect){ {0,0},{1024,460}},
			(NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask | NSMiniaturizableWindowMask), 0, false);
	    
		/// Create an instance of our view class.
		///
		/// Relies on the view having declared a constructor that allocates a class pair for it.
		id view = cmacs_rect_msgSend1(cmacs_simple_msgSend((id)objc_getClass("YView"), sel_getUid("alloc")), sel_getUid("initWithFrame:"), (CMRect){ {0, 0}, {640, 480} });

		// here we simply add the view to the window.
		cmacs_void_msgSend1(appDelObj->window, sel_getUid("setContentView:"), view);
		cmacs_simple_msgSend(appDelObj->window, sel_getUid("becomeFirstResponder"));

		// Shows our window in the bottom-left hand corner of the screen.
		//cmacs_void_msgSend1(appDelObj->window, sel_getUid("makeKeyAndOrderFront:"), appDelObj);

		return true;
	}


	bool NSGLWindow::loop()
	{
		// TODO : Can we pump events manually ?
		//NSEvent* event;
		id event;
		do
		{
// 			event = [NSApp nextEventMatchingMask:NSAnyEventMask untilDate:[NSDate distantPast]
// 					 inMode:NSDefaultRunLoopMode dequeue:YES];
			event = cmacs_next_event_msgSend(NSApp, sel_getUid("nextEventMatchingMask"), 0, 0, true);
            //Convert the cocoa events to something useful here and add them to your own event queue

			//[NSApp sendEvent: event];
		}
		while (event != nil);
		return true;
	}


	void NSGLWindow::kill()
	{
		GLWindow::kill();

		UnregisterWindow(pID);
		// TODO : cleanly destroy the window
	}




	RenderWindow* WindowFactory::CreateGLWindow(const AnyString& title, uint width,
		uint height, uint bits, bool fullScreen)
	{
		RenderWindow* wnd = new NSGLWindow(title, width, height, bits, fullScreen);
		if (!wnd->initialize())
		{
			wnd->kill();
			delete wnd;
			return nullptr;
		}
		return wnd;
	}



}  // namespace UI
}  // namespace Yuni
