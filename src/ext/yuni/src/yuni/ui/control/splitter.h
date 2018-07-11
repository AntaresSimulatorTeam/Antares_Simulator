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
#ifndef __YUNI_UI_CONTROL_SPLITTER_H__
# define __YUNI_UI_CONTROL_SPLITTER_H__

# include "../../yuni.h"
# include "control.h"
# include "panel.h"

namespace Yuni
{
namespace UI
{
namespace Control
{


	//! A splitter panel is a vertically- or horizontally-splitting container, each side containing a panel
	class Splitter: public IControl
	{
	public:
		//! Smart pointer
		typedef Ancestor::SmartPtrType<Splitter>::Ptr  Ptr;

	public:
		//! Orientation of the split
		enum SplitOrientation
		{
			soVertical,
			soHorizontal
		};

	public:
		//! Constructor
		Splitter(float x, float y, float width, float height):
			IControl(x, y, width, height),
			pOrient(soVertical),
			pSplitOffset(width / 2.0f)
		{
			addChild(new Panel(x, y, pSplitOffset, height));
			addChild(new Panel(x + pSplitOffset, y, pSplitOffset, height));
		}

		//! Constructor
		Splitter(const Point2D<float>& position, const Point2D<float>& size):
			Splitter(position.x, position.y, size.x, size.y)
		{}

		//! Virtual destructor
		virtual ~Splitter() {}

		//! Draw the splitter
		virtual void draw(DrawingSurface::Ptr& surface, float xOffset, float yOffset) const override;

		//! Get the current orientation
		SplitOrientation orientation() const { return pOrient; }
		//! Set the orientation
		void orientation(SplitOrientation newOrientation) { pOrient = newOrientation; }

		//! Get the offset at which the split appears (in pixels)
		float splitOffset() const { return pSplitOffset; }
		//! Set the offset at which the split appears (in pixels)
		void splitOffset(float newOffset) { pSplitOffset = newOffset; resizePanels(); }

		//! Left panel when vertical, top panel when horizontal
		UI::IControl::Ptr& firstPanel() { assert(pChildren.size() > 0); return pChildren[0]; }
		const UI::IControl::Ptr& firstPanel() const { assert(pChildren.size() > 0); return pChildren[0]; }

		//! Right panel when vertical, bottom panel when horizontal
		UI::IControl::Ptr& secondPanel() { assert(pChildren.size() > 1); return pChildren[1]; }
		const UI::IControl::Ptr secondPanel() const { assert(pChildren.size() > 1); return pChildren[1]; }

	private:
		//! Automatically resize the two contained panels depending on the split characteristics
		void resizePanels();

	private:
		//! Orientation of the split
		SplitOrientation pOrient;

		//! Position of the split
		float pSplitOffset;

	}; // class Splitter



} // namespace Control
} // namespace UI
} // namespace Yuni

#endif // __YUNI_UI_CONTROL_SPLITTER_H__
