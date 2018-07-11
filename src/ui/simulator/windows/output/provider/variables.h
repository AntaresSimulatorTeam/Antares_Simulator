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
#ifndef __ANTARES_WINDOWS_OUTPUT_PROVIDER_VARIABLES_H__
# define __ANTARES_WINDOWS_OUTPUT_PROVIDER_VARIABLES_H__

# include <antares/wx-wrapper.h>
# include <ui/common/component/spotlight.h>
# include "../fwd.h"
# include <wx/bitmap.h>


namespace Antares
{
namespace Window
{
namespace OutputViewer
{
namespace Provider
{

	class Variables : public Antares::Component::Spotlight::IProvider
	{
	public:
		//! The spotlight component (alias)
		typedef Antares::Component::Spotlight Spotlight;

	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		Variables(Component* com);
		//! Destructor
		virtual ~Variables();
		//@}

		/*!
		** \brief Perform a new search
		*/
		virtual void search(Spotlight::IItem::Vector& out, const Spotlight::SearchToken::Vector& tokens, const Yuni::String& text = "");

		/*!
		** \brief An item has been selected
		*/
		virtual bool onSelect(Spotlight::IItem::Ptr& item);


	private:
		//! The parent component
		Component* pComponent;

		//! Bitmap re-attach
		wxBitmap* pBmpVariable;
		wxBitmap* pBmpThermal;

	}; // class Layer








} // namespace Provider
} // namespace OutputViewer
} // namespace Window
} // namespace Antares

#endif // __ANTARES_WINDOWS_OUTPUT_PROVIDER_VARIABLES_H__
