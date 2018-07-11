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
#ifndef __YUNI_UI_CONTROL_CHECKBOX_H__
# define __YUNI_UI_CONTROL_CHECKBOX_H__

# include "../../yuni.h"
# include "control.h"
# include "../font.h"

namespace Yuni
{
namespace UI
{
namespace Control
{


	//! A check box is a clickable control that has a boolean state: either checked or unchecked
	class CheckBox: public IControl
	{
	public:
		//! Smart pointer
		typedef Ancestor::SmartPtrType<CheckBox>::Ptr  Ptr;

		//! Checkbox checked callback
		Yuni::Bind<void (IControl* sender, bool newValue)>  onCheckChanged;

	public:
		CheckBox(float x, float y, float width, float height):
			IControl(x, y, width, height),
			pChecked(false),
			pBeingClicked(false)
		{}

		CheckBox(const Point2D<float>& position, const Point2D<float>& size):
			IControl(position, size),
			pChecked(false),
			pBeingClicked(false)
		{}

		//! Virtual destructor
		virtual ~CheckBox() {}

		const String& text() const { return pText; }
		void text(const AnyString& text) { pText = text; invalidate(); }

		//! Modify whether the checkbox is checked
		void checked(bool newValue)
		{
			if (pChecked == newValue)
				return;
			pChecked = newValue;
			invalidate();
			// Callback
			onCheckChanged(this, newValue);
		}
		//! Get whether the checkbox is checked
		bool checked() const { return pChecked; }

		//! Draw the checkbox on the surface
		virtual void draw(DrawingSurface::Ptr& surface, float xOffset, float yOffset) const override;

	private:
		//! Control reaction to mouse down
		virtual EventPropagation mouseDown(Input::IMouse::Button btn, float, float) override
		{
			if (Input::IMouse::ButtonLeft == btn)
				pBeingClicked = true;
			return epStop;
		}
		//! Control reaction to mouse up
		virtual EventPropagation mouseUp(Input::IMouse::Button btn, float, float) override
		{
			if (Input::IMouse::ButtonLeft == btn && pBeingClicked)
			{
				pChecked = !pChecked;
				invalidate();
				onCheckChanged(this, pChecked);
			}
			pBeingClicked = false;
			return epStop;
		}

	private:
		bool pChecked;

		bool pBeingClicked;

		String pText;

		String pHoverText;

	}; // class CheckBox





} // namespace Control
} // namespace UI
} // namespace Yuni

#endif // __YUNI_UI_CONTROL_CHECKBOX_H__
