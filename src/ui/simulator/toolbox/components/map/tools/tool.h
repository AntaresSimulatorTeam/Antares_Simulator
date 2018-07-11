/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_TOOLBOX_MAP_TOOLS_TOOL_H__
# define __ANTARES_TOOLBOX_MAP_TOOLS_TOOL_H__

# include <vector>
# include <antares/wx-wrapper.h>
# include "../drawingcontext.h"



namespace Antares
{
namespace Map
{
namespace Tool
{


	/*!
	** \brief Life span for a tool
	*/
	enum LifeSpan
	{
		//! The tool will not survive to the next selection/deselection
		lifeSpanMouseSelection,
		/*!
		** \brief The tool must always remain on the map, and it is not really
		** attached to any items.
		*/
		lifeSpanImmortality,

	}; // enum LifeSpan





	/*!
	** \brief Visual component to make an action on the selected items
	*/
	class Tool
	{
	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Constructor
		**
		** \param manager The parent manager
		** \param icon The icon representing this tool (16x16)
		*/
		Tool(Manager& manager, const char* icon);
		//! Destructor
		virtual ~Tool();
		//@}

		//! \name Manager
		//@{
		Manager& manager() {return pManager;}
		const Manager& manager() const {return pManager;}
		//@}

		//! \name X-Coordinate
		//@{
		//! Get the X-Coordinate of the tool
		int x() const {return pX;}
		//! Set the X-Coordinate of the tool
		void x(const int v) {pX = v;}
		//@}
		//! \name Y-Coordinate
		//@{
		//! Get the Y-Coordinate of the tool
		int y() const {return pY;}
		//! Set the Y-Coordinate of the tool
		void y(const int v) {pY = v;}
		//@}

		//! \name Sizes
		//@{
		//! Get the width of the tool
		uint width() const {return pWidth;}
		//! Get the height of the tool
		uint height() const {return pHeight;}
		//@}

		//! \name Icon
		//@{
		//! Get the icon of the tool, NULL if not available
		const wxBitmap* icon() const {return pIcon;}
		/*!
		** \brief Set the icon representing the tool
		**
		** The given filename is a relative filename in the resources
		** folder.
		** \param filename A relative filename
		*/
		void icon(const char* filename);
		//}


		//! \name Life span
		//@{
		//! Get the lifespan of the tool
		LifeSpan lifeSpan() const {return lifeSpanMouseSelection;}
		//@}

		//! \name Drawing
		//@{

		/*!
		** \brief Get if a point is insided the bounding box of the tool
		**
		** \param x The X-Coordinate of the point
		** \param y The Y-Coordinate of the point
		** \return True if the point is inside the bounding box, false otherwise
		*/
		bool contains(const int x, const int y) const
		{return x >= pX && x <= pX + pWidth && y >= pY && y <= pY + pHeight;}


		/*!
		** \brief Draw the tool
		**
		** \param dc A drawing context
		** \param mouseDown True if the mouse is down
		** \param position
		** \param absolute
		*/
		virtual void draw(DrawingContext& dc, const bool mouseDown,
			const wxPoint& position, const wxPoint& absolute) const;

		//@}

		/*!
		** \brief Get if the action triggered by the tool is immediate
		**
		** When the value is true, the action should be executed as soon
		** as the user clicks on the tool. When equals to false, the action
		** should be delayed until the next click.
		**
		** \return True if the action triggered by the tool is immediate
		*/
		virtual bool actionIsImmediate() const {return true;}

		/*!
		** \brief Event: The user has clicked on the tool
		**
		** \return True to invalidate all tools (ex: some nodes have been removed, moved...)
		*/
		virtual bool onMouseUp(const int mx, const int my) = 0;


	protected:
		//! The parent manager
		Manager& pManager;
		//! The X-Coordinate
		int pX;
		//! The Y-Coordinate
		int pY;
		//! Width
		int pWidth;
		//! Height
		int pHeight;
		//! Icon
		wxBitmap* pIcon;

	}; // class Tool



	//! List of tools
	typedef std::vector<Tool*>  List;


} // namespace Tool
} // namespace Map
} // namespace Antares


#endif // __ANTARES_TOOLBOX_MAP_TOOLS_TOOL_H__
