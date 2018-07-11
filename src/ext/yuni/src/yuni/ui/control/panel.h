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
#ifndef __YUNI_UI_CONTROL_PANEL_H__
# define __YUNI_UI_CONTROL_PANEL_H__

# include "../../yuni.h"
# include "../../core/color/rgba.h"
# include "control.h"

namespace Yuni
{
namespace UI
{
namespace Control
{


	//! A panel is an unmovable, by default transparent control meant to contain other controls.
	class Panel: public IControl
	{
	public:
		//! Smart pointer
		typedef Ancestor::SmartPtrType<Panel>::Ptr  Ptr;

	public:
		//! Constructor
		Panel(float x, float y, float width, float height);

		//! Constructor
		Panel(const Point2D<float>& position, const Point2D<float>& size);

		//! Virtual destructor
		virtual ~Panel() {}

		//! Draw the panel
		virtual void draw(DrawingSurface::Ptr& surface, float xOffset, float yOffset) const override;

		void color(const Color::RGBA<float>& newColor) { pColor = newColor; }
		const Color::RGBA<float>& color() const { return pColor; }

	protected:
		Color::RGBA<float> pColor;

	}; // class Panel



} // namespace Control
} // namespace UI
} // namespace Yuni

#endif // __YUNI_UI_CONTROL_PANEL_H__
