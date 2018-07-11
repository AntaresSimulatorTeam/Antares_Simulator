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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_H__
# define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_H__

# include <antares/wx-wrapper.h>
# include "../../../input/area.h"
# include "../renderer.h"
# include <yuni/core/event.h>


namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{


	class ARendererArea : public virtual IRenderer
	{
	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Constructor
		**
		** \param parent The parent window
		** \param notifier The area notifier
		*/
		ARendererArea(wxWindow* parent, Toolbox::InputSelector::Area* notifier);
		//! Destructor
		virtual ~ARendererArea();
		//@}

		//! \name Dimensions
		//@{
		//! Get the current width
		virtual int width() const = 0;
		//! Get the current height
		virtual int height() const = 0;
		//@}

		//! \name Captions
		//@{
		/*!
		** \brief Get the column caption
		** \param colIndx The column index
		*/
		virtual wxString columnCaption(int colIndx) const = 0;
		/*!
		** \brief Get the row caption
		** \param rowIndx The row index
		*/
		virtual wxString rowCaption(int rowIndx) const = 0;
		//@}

		/*!
		** \brief Get the string representation of a cell
		** \param x The X-Coordinate
		** \param y The Y-Coordinate
		** \return The value
		*/
		virtual wxString cellValue(int x, int y) const = 0;

		/*!
		** \brief Set the value of a cell
		**
		** \param x The X-Coordinate
		** \param y The Y-Coordinate
		** \param value The new value
		** \return True if the operation was successful
		*/
		virtual bool cellValue(int x, int y, const Yuni::String& value) = 0;

		virtual void resetColors(int x, int y, wxColour& background, wxColour& textForeground) const = 0;

		virtual bool valid() const {return (pArea != NULL);}

	protected:
		virtual void internalAreaChanged(Data::Area* area);
		//! The study has been closed
		virtual void onStudyClosed() override;
		//! Event: the study has been loaded
		virtual void onStudyLoaded() override;

	protected:
		//! The attached control
		wxWindow* pControl;
		//! The current area
		Data::Area* pArea;

	private:
		void onAreaChanged(Data::Area* area);
		void onAreaDelete(Data::Area* area);

	}; // class ARendererArea





} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_H__
