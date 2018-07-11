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
#ifndef __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_H__
# define __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_H__

# include <antares/wx-wrapper.h>



namespace Antares
{
namespace Component
{
namespace HTMLListbox
{

	// Forward declaration
	struct ISortPredicate;


namespace Item
{


	/*!
	** \brief Single item for an Input (abstract)
	**
	** An input is often an item list (list of areas, list of interconnections...)
	** but this list may be heterogenous (for example `areas + groups`)
	** This class a merely a simple wrapper to bring commons display operations
	*/
	class IItem
	{
	public:
		//! The most suitable smartptr for this class
		typedef Yuni::SmartPtr<IItem>  Ptr;
		//! Item list
		typedef std::vector<Ptr>  List;

	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default Constructor
		*/
		IItem();
		//! Destructor
		virtual ~IItem();
		//@}

		/*!
		** \brief Get the HTML Content for the item
		**
		** \param searchString The current string to highlight
		*/
		virtual wxString htmlContent(const wxString& searchString) = 0;

		/*!
		** \brief Get if the item is highlighted
		**
		** An item may be highlighted by an user's research. The variable
		** is updated when the method `htmlContent()` is called.
		*/
		bool highlighted() const;

		//! Get if the item is visible
		bool visible() const;
		//! Set if the item is visible
		void visible(bool v);

		/*!
		** \brief Get the item is interactive (can be clicked by the user)
		*/
		virtual bool interactive() const;

	protected:
		//! Is the item highlighted
		bool pHighlighted;
		//! Visible
		bool pVisible;

	}; // class IItem



	/*!
	** \brief Hightlight a sub part of a string (with HTML Code)
	**
	** \param s The String to modify
	** \param toHighlight The string to highlight for the user
	*/
	bool HTMLCodeHighlightString(wxString& s, const wxString& toHighlight);

	/*!
	** \brief Make a color darker
	*/
	wxColour ColorDarker(int r, int g, int b, int level = 40);
	wxColour ColorDarker(const wxColour& c, int level = 40);




} // namespace Item
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares

# include "item.hxx"

#endif // __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_H__
