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
#ifndef __ANTARES_TOOLBOX_MAP_NODE_H__
# define __ANTARES_TOOLBOX_MAP_NODE_H__

# include <antares/wx-wrapper.h>
# include <antares/study.h>
# include <set>
# include "item.h"
# include "../drawingcontext.h"



namespace Antares
{
namespace Map
{

	// Forward declaration of the manager
	class Manager;
	class DrawingContext;




	/*!
	** \brief A node in a map
	*/
	class Node : public Item
	{
	public:
		//! \name Constructors & Destructor
		//@{

		/*!
		** \brief Default constructor
		*/
		Node(Manager& manager);

		/*!
		** \brief Constructor with a given id
		*/
		Node(Manager& manager, const wxString& id);

		/*!
		** \brief Destructor
		*/
		virtual ~Node();

		//@}

		//! \name layerVisibility
		//@{
		/*!
		** \brief Get the visivility for a layerId
		*/
		const bool isVisibleOnLayer(const size_t& layerID)const;
		//@}

		virtual Type type() const {return tyNode;}

		//! \name ID
		//@{
		const wxString& id() const {return pId;}
		//@}


		void position(const int x, const int y);


		/*!
		** \brief Move the node
		**
		** \param x A relative X-coordinate
		** \param y A relative Y-coordinate
		*/
		virtual void move(const int x, const int y);


		//! \name Cache
		//@{

		/*!
		** \brief Refresh the cache (even if not invalidated)
		*/
		virtual void refreshCache(wxDC& dc);

		/*!
		** \brief Get the cached position
		*/
		const wxPoint& cachedPosition() const {return pCachedPosition;}

		/*!
		** \brief Get the cached size
		*/
		const wxPoint& cachedSize() const {return pCachedSize;}

		//@}


		//! \name Misc
		//@{
		/*!
		** \brief Get if the drawing representation of the node contains the point (x,y)
		**
		** This method is used to know if the mouse if over a node or not
		*/
		virtual bool contains(const int x, const int y, double& distance);

		/*!
		** \brief Get if the drawing representation of the node is contained inside a bounding box
		**
		** This method is used to know if the mouse if over a node or not
		*/
		virtual bool isContained(const int x1, const int y1, const int x2, const int y2) const;

		/*!
		** \brief Draw the node
		*/
		virtual void draw(DrawingContext& dc);

		//@}

		virtual void extendBoundingBox(wxPoint& topLeft, wxPoint& bottomRight);

		Data::Area* attachedArea() const {return pAttachedArea;}
		void attachedArea(Data::Area* a) {pAttachedArea = a;}
		
		/*!
		** \brief make this node visible/invisible in a layer
		*/
		void addLayerVisibility(size_t id = 0);
		void removeLayerVisibility(size_t id);

	protected:
		virtual void captionHasChanged();
		virtual void positionHasChanged();
		virtual void colorHasChanged();

		void createANewAreaIfNotAlreadyAttached();

	private:
		//! id
		wxString pId;

		wxPoint pCachedPosition;
		wxPoint pCachedSize;
		wxColour pCachedColorGradientStart;
		wxColour pCachedColorGradientEnd;
		wxColour pCachedColorGradientStart2;
		wxColour pCachedColorGradientEnd2;

		wxColour pCachedColorText;
		wxPen pBorderPen;
		wxPen pShadowPen;

		Data::Area* pAttachedArea;

	}; // class Node






} // namespace Map
} // namespace Antares


#endif // __ANTARES_TOOLBOX_MAP_NODE_H__
