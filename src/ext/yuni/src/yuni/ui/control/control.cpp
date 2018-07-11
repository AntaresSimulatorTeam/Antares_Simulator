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
#include "control.h"

namespace Yuni
{
namespace UI
{


	IControl* IControl::getControlAt(float x, float y)
	{
		// Invisible controls don't answer to events
		if (!pVisible)
			return nullptr;
		// If the point is outside the control, return null
		if (x < pPosition.x || y < pPosition.y ||
			x > pPosition.x + pSize.x ||
			y > pPosition.y + pSize.y)
			return nullptr;

		// If no child contains the point, return this control
		IControl* result = this;

		// Try to find a child that contains the point
		float relativeX = x - pPosition.x;
		float relativeY = y - pPosition.y;
		Vector::iterator end = pChildren.end();
		for (Vector::iterator it = pChildren.begin(); it != end; ++it)
		{
			// Recursive call
			IControl* child = (*it)->getControlAt(relativeX, relativeY);
			if (child)
			{
				result = child;
				break;
			}
		}
		return result;
	}

	void IControl::getControlStackAt(float x, float y, std::vector<IControl*>& stack)
	{
		// Invisible controls don't answer to events
		if (!pVisible)
			return;
		// If the point is outside the control, return null
		if (x < pPosition.x || y < pPosition.y ||
			x > pPosition.x + pSize.x ||
			y > pPosition.y + pSize.y)
			return;

		// Push this control in the stack
		stack.push_back(this);

		// Try to find a child that contains the point
		float relativeX = x - pPosition.x;
		float relativeY = y - pPosition.y;
		Vector::iterator end = pChildren.end();
		for (Vector::iterator it = pChildren.begin(); it != end; ++it)
		{
			// Recursive call
			(*it)->getControlStackAt(relativeX, relativeY, stack);
		}
	}

	void IControl::show(bool visible)
	{
		if (visible == pVisible)
			return;
		pVisible = visible;
		invalidate();
	}


	bool IControl::modified() const
	{
		if (pModified)
			return true;

		// Ask children for modifications
		for (const auto& child : pChildren)
			if (child->modified())
				return true;

		return false;
	}


	EventPropagation IControl::doMouseMove(float x, float y, Set& enteredControls)
	{
		if (!pVisible)
			return epContinue;

		// Check first which controls have been left
		if (not enteredControls.empty())
		{
			Vector toRemoveControls;
			for (auto control : enteredControls)
			{
				if (!control->contains(x, y))
				{
					// Control has been left
					toRemoveControls.push_back(control);
					EventPropagation prop = control->doMouseLeave();
					if (epStop == prop)
						break;
				}
			}
			for (auto toRemove : toRemoveControls)
				enteredControls.erase(toRemove);
		}

		// Get the stack of controls under the (x,y) point
		std::vector<IControl*> stack;
		getControlStackAt(x, y, stack);
		EventPropagation finalProp = epContinue;
		EventPropagation enterProp = epContinue;
		while (!stack.empty())
		{
			IControl* child = stack.back();
			stack.pop_back();

			// Mouse enter check
			if (enterProp < epStop && enteredControls.end() == enteredControls.find(child))
			{
				// Control has been entered
				enteredControls.insert(child);
				EventPropagation prop = child->doMouseEnter();
				if (prop > enterProp)
					enterProp = prop;
			}
			// Allow the control to react to the event before the callback
			EventPropagation prop = child->mouseMove(x, y);
			if (epStop == prop)
				return epStop;
			if (prop > finalProp)
				finalProp = prop;

			// Event callback
			prop = eventFold(child->onMouseMove, child, x, y);
			if (epStop == prop)
				return epStop;
			if (prop > finalProp)
				finalProp = prop;
		}
		// Allow the control to react to the event before the callback
		EventPropagation prop = mouseMove(x, y);
		// Event callback on the root control
		prop = eventFold(onMouseMove, this, x, y);
		return Math::Max(prop, finalProp);
	}

	EventPropagation IControl::doMouseDown(Input::IMouse::Button btn, float x, float y)
	{
		if (!pVisible)
			return epContinue;

		std::vector<IControl*> stack;
		getControlStackAt(x, y, stack);
		EventPropagation finalProp = epContinue;
		while (!stack.empty())
		{
			IControl* child = stack.back();
			stack.pop_back();
			EventPropagation prop = child->mouseDown(btn, x, y);
			if (epStop == prop)
				return epStop;
			if (prop > finalProp)
				finalProp = prop;

			prop = eventFold(child->onMouseDown, child, btn, x, y);
			if (epStop == prop)
				return epStop;
			if (prop > finalProp)
				finalProp = prop;
		}
		EventPropagation prop = mouseDown(btn, x, y);
		if (epStop == prop)
			return epStop;
		if (prop > finalProp)
			finalProp = prop;
		prop = eventFold(onMouseDown, this, btn, x, y);
		return Math::Max(prop, finalProp);
	}

	EventPropagation IControl::doMouseUp(Input::IMouse::Button btn, float x, float y)
	{
		if (!pVisible)
			return epContinue;

		std::vector<IControl*> stack;
		getControlStackAt(x, y, stack);
		EventPropagation finalProp = epContinue;
		while (!stack.empty())
		{
			IControl* child = stack.back();
			stack.pop_back();
			EventPropagation prop = child->mouseUp(btn, x, y);
			if (epStop == prop)
				return epStop;
			if (prop > finalProp)
				finalProp = prop;

			prop = eventFold(child->onMouseUp, child, btn, x, y);
			if (epStop == prop)
				return epStop;
			if (prop > finalProp)
				finalProp = prop;
		}
		EventPropagation prop = mouseUp(btn, x, y);
		if (epStop == prop)
			return epStop;
		if (prop > finalProp)
			finalProp = prop;
		prop = eventFold(onMouseUp, this, btn, x, y);
		return Math::Max(prop, finalProp);
	}

	EventPropagation IControl::doMouseDblClick(Input::IMouse::Button btn, float x, float y)
	{
		if (!pVisible)
			return epContinue;

		std::vector<IControl*> stack;
		getControlStackAt(x, y, stack);
		EventPropagation finalProp = epContinue;
		while (!stack.empty())
		{
			IControl* child = stack.back();
			stack.pop_back();
			EventPropagation prop = child->mouseDblClick(btn, x, y);
			if (epStop == prop)
				return epStop;
			if (prop > finalProp)
				finalProp = prop;

			prop = eventFold(child->onMouseDblClick, child, btn, x, y);
			if (epStop == prop)
				return epStop;
			if (prop > finalProp)
				finalProp = prop;
		}
		EventPropagation prop = mouseDblClick(btn, x, y);
		if (epStop == prop)
			return epStop;
		if (prop > finalProp)
			finalProp = prop;
		prop = eventFold(onMouseDblClick, this, btn, x, y);
		return Math::Max(prop, finalProp);
	}

	EventPropagation IControl::doMouseScroll(float delta, float x, float y)
	{
		if (!pVisible)
			return epContinue;

		std::vector<IControl*> stack;
		getControlStackAt(x, y, stack);
		EventPropagation finalProp = epContinue;
		while (!stack.empty())
		{
			IControl* child = stack.back();
			stack.pop_back();
			EventPropagation prop = child->mouseScroll(delta, x, y);
			if (epStop == prop)
				return epStop;
			else if (prop > finalProp)
				finalProp = prop;
			prop = eventFold(child->onMouseScroll, child, delta);
			if (epStop == prop)
				return epStop;
			else if (prop > finalProp)
				finalProp = prop;
		}
		EventPropagation prop = mouseScroll(delta, x, y);
		if (epStop == prop)
			return epStop;
		else if (prop > finalProp)
			finalProp = prop;
		prop = eventFold(onMouseScroll, this, delta);
		return Math::Max(prop, finalProp);
	}


	EventPropagation IControl::doMouseEnter()
	{
		EventPropagation prop = mouseEnter();
		if (epStop == prop)
			return epStop;
		EventPropagation prop2 = eventFold(onMouseEnter, this);
		return Math::Max(prop, prop2);
	}

	EventPropagation IControl::doMouseLeave()
	{
		EventPropagation prop = mouseLeave();
		if (epStop == prop)
			return epStop;
		EventPropagation prop2 = eventFold(onMouseLeave, this);
		return Math::Max(prop, prop2);
	}


	EventPropagation IControl::doMouseHover(float x, float y)
	{
		if (!pVisible)
			return epContinue;

		std::vector<IControl*> stack;
		getControlStackAt(x, y, stack);
		EventPropagation finalProp = epContinue;
		while (!stack.empty())
		{
			IControl* child = stack.back();
			stack.pop_back();
			EventPropagation prop = child->mouseHover(x, y);
			if (epStop == prop)
				return epStop;
			if (prop > finalProp)
				finalProp = prop;

			prop = eventFold(child->onMouseHover, child, x, y);
			if (epStop == prop)
				return epStop;
			if (prop > finalProp)
				finalProp = prop;
		}
		EventPropagation prop = mouseHover(x, y);
		if (epStop == prop)
			return epStop;
		if (prop > finalProp)
			finalProp = prop;
		prop = eventFold(onMouseHover, this, x, y);
		return Math::Max(prop, finalProp);
	}


	EventPropagation IControl::doKeyDown(Input::Key key, float x, float y)
	{
		if (!pVisible)
			return epContinue;

		std::vector<IControl*> stack;
		getControlStackAt(x, y, stack);
		EventPropagation finalProp = epContinue;
		while (!stack.empty())
		{
			IControl* child = stack.back();
			stack.pop_back();
			EventPropagation prop = child->keyDown(key);
			if (epStop == prop)
				return epStop;
			if (prop > finalProp)
				finalProp = prop;

			prop = eventFold(child->onKeyDown, child, key);
			if (epStop == prop)
				return epStop;
			if (prop > finalProp)
				finalProp = prop;
		}
		EventPropagation prop = keyDown(key);
		if (epStop == prop)
			return epStop;
		if (prop > finalProp)
			finalProp = prop;
		prop = eventFold(onKeyDown, this, key);
		return Math::Max(prop, finalProp);
	}


	EventPropagation IControl::doKeyUp(Input::Key key, float x, float y)
	{
		if (!pVisible)
			return epContinue;

		std::vector<IControl*> stack;
		getControlStackAt(x, y, stack);
		EventPropagation finalProp = epContinue;
		while (!stack.empty())
		{
			IControl* child = stack.back();
			stack.pop_back();
			EventPropagation prop = child->keyUp(key);
			if (epStop == prop)
				return epStop;
			if (prop > finalProp)
				finalProp = prop;

			prop = eventFold(child->onKeyUp, child, key);
			if (epStop == prop)
				return epStop;
			if (prop > finalProp)
				finalProp = prop;
		}
		EventPropagation prop = keyUp(key);
		if (epStop == prop)
			return epStop;
		if (prop > finalProp)
			finalProp = prop;
		prop = eventFold(onKeyUp, this, key);
		return Math::Max(prop, finalProp);
	}


	EventPropagation IControl::doCharInput(const AnyString& str, float x, float y)
	{
		if (!pVisible)
			return epContinue;

		std::vector<IControl*> stack;
		getControlStackAt(x, y, stack);
		EventPropagation finalProp = epContinue;
		while (!stack.empty())
		{
			IControl* child = stack.back();
			stack.pop_back();
			EventPropagation prop = child->charInput(str);
			if (epStop == prop)
				return epStop;
			if (prop > finalProp)
				finalProp = prop;

			prop = child->onCharInput.fold(epContinue, Functional::Max<EventPropagation>(), child, str);
			if (epStop == prop)
				return epStop;
			if (prop > finalProp)
				finalProp = prop;
		}
		EventPropagation prop = charInput(str);
		if (epStop == prop)
			return epStop;
		if (prop > finalProp)
			finalProp = prop;
		prop = onCharInput.fold(epContinue, Functional::Max<EventPropagation>(), this, str);
		return Math::Max(prop, finalProp);
	}



} // namespace UI
} // namespace Yuni
