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
#ifndef __YUNI_INPUT_KEYBOARD_H__
# define __YUNI_INPUT_KEYBOARD_H__

# include "../../yuni.h"
# include "../../core/bind.h"
# include "../../core/smartptr.h"
# include "key.h"
# include "../eventpropagation.h"

namespace Yuni
{

namespace UI
{

	//! Forward declaration
	class RenderWindow;

} // namespace UI


namespace Input
{


	//! Keyboard input management
	class Keyboard
	{
	public:
		//! Smart pointer
		typedef SmartPtr<Keyboard>  Ptr;

	public:
		//! Arguments for event callbacks
		struct EventArgs
		{
			EventArgs(const bool* keyStates):
				keys(keyStates),
				propagate(UI::epStop)
			{}

			//! Key states : true if the key is currently pushed
			const bool* const keys;

			//! Should we propagate the event to underlying controls / views ?
			UI::EventPropagation propagate;

		}; // struct EventArgs

	public:
		//! Prototype for a keyboard event callback
		//typedef Yuni::Bind<void (IControl::Ptr sender, EventArgs& args)>  KeyboardCallback;

		//! \name Bindings for user-code event listeners
		//@{
		Yuni::Bind<void (Key key)>  onKeyDown;
		Yuni::Bind<void (Key key)>  onKeyUp;
		//@}

	public:
		//! Constructor
		Keyboard()
		{
			for (uint i = 0; i < KeyCount; ++i)
				pKeyDown[i] = false;
		}

		//! Virtual destructor
		virtual ~Keyboard()
		{
			onKeyDown.unbind();
			onKeyUp.unbind();
		}

		//! Is the key pressed ?
		bool isPressed(Key key) const { return pKeyDown[key]; }

	private:
		//! \name Event notifications from window
		//@{
		void doDown(Key key) { pKeyDown[key] = true; onKeyDown(key); }
		void doUp(Key key) { pKeyDown[key] = false; onKeyUp(key); }
		//@}

	protected:
		//! Keep button state
		volatile bool pKeyDown[KeyCount];

		//! Friend declaration : RenderWindow manages data in this class
		friend class Yuni::UI::RenderWindow;

	}; // class Keyboard



} // namespace Input
} // namespace Yuni


#endif // __YUNI_INPUT_KEYBOARD_H__
