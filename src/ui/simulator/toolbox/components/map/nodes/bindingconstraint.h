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
#ifndef __ANTARES_TOOLBOX_MAP_BINDING_CONSTRAINT_H__
# define __ANTARES_TOOLBOX_MAP_BINDING_CONSTRAINT_H__

# include <antares/wx-wrapper.h>
# include "item.h"


namespace Antares
{
namespace Map
{


	class BindingConstraint final : public Item
	{
	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Constructor
		*/
		BindingConstraint(Manager& manager);
		/*!
		** \brief Destructor
		*/
		virtual ~BindingConstraint();
		//@}

		/*!
		** \brief Type
		*/
		virtual Type type() const {return tyBindingConstraint;}

		//! \name Selection
		//@{
		virtual bool selected() const {return pSelected;}
		virtual void selected(bool v);
		//@}


		//! \name Cache
		//@{
		/*!
		** \brief Refresh the cache (even if not invalidated)
		*/
		virtual void refreshCache(wxDC& dc);
		//@}


		//! \name Misc
		//@{
		/*!
		** \brief Get if the drawing representation of the node contains the point (x,y)
		*/
		virtual bool contains(const int, const int, double& distance) {return false;}

		/*!
		** \brief Get if the drawing representation of the node is contained inside a bounding box
		**
		** This method is used to know if the mouse if over a node or not
		*/
		virtual bool isContained(const int, const int, const int, const int) const {return false;}

		/*!
		** \brief Draw the node
		*/
		virtual void draw(DrawingContext& dc);
		virtual void drawExternalDrawer(DrawingContext& dc);
		//@}

		/*!
		** \brief Extends the bounding box
		*/
		virtual void extendBoundingBox(wxPoint&, wxPoint&) {}


		void clear();

		bool empty() const {return pConnections.empty();}
		uint count() const {return (uint) pConnections.size();}

		void add(Item* item);

		bool remove(Item* item);


	private:
		struct Infos
		{
		public:
			Infos() :weight(1.), selected(false) {}
			Infos(const Infos& c) :weight(c.weight), selected(false) {}
			double weight;
			bool selected;
		};

		typedef std::map<Connection*, Infos>  Connections;
		Connections pConnections;

		struct TextPart
		{
			TextPart() :text(), color(0,0,0), size(0,0){}
			TextPart(const wxString& s, const int r, const int g, const int b)
				:text(s), color(r,g,b)
			{
			}
			wxString text;
			wxColour color;
			wxSize   size;

			void refreshCache(wxDC& dc);
		};

		std::vector<TextPart>  pTextParts;

	}; // class BindingConstraint





} // namespace Item
} // namespace Antares

#endif // __ANTARES_TOOLBOX_MAP_BINDING_CONSTRAINT_H__
