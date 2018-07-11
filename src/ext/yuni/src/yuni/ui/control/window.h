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
#ifndef __YUNI_UI_CONTROL_WINDOW_H__
# define __YUNI_UI_CONTROL_WINDOW_H__

# include "../../yuni.h"
# include "control.h"


namespace Yuni
{
namespace UI
{
namespace Control
{


	//! A window is a movable control that is meant to contain other controls
	class Window: public IControl
	{
	public:
		//! Smart pointer
		typedef Ancestor::SmartPtrType<Window>::Ptr  Ptr;

	public:
		Window(float x, float y, float width, float height):
			IControl(x, y, width, height)
		{}

		Window(const Point2D<float>& position, const Point2D<float>& size):
			IControl(position, size)
		{}

		//! Virtual destructor
		virtual ~Window() {}

		//! Draw the window
		virtual void draw(DrawingSurface::Ptr& surface, float xOffset, float yOffset) const override;

	}; // class Window



} // namespace Control
} // namespace UI
} // namespace Yuni

#endif // __YUNI_UI_CONTROL_WINDOW_H__
