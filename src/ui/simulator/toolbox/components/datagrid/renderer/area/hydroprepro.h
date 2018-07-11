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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_HYDROPREPRO_H__
# define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_HYDROPREPRO_H__

# include <antares/wx-wrapper.h>
# include "../area.h"
# include "../matrix.h"
# include <antares/date.h>
# include <antares/study/parts/wind/prepro.h>



namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{


	class HydroPrepro final :
		public Renderer::Matrix<double, double, 3>,
		public Renderer::ARendererArea
	{
	public:
		typedef Renderer::Matrix<double, double, 3>  MatrixAncestorType;

	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Constructor
		*/
		HydroPrepro(wxWindow* control, Toolbox::InputSelector::Area* notifier);
		//! Destructor
		virtual ~HydroPrepro();
		//@}

		virtual int width() const;
		virtual int height() const;

		virtual wxString columnCaption(int colIndx) const;

		virtual wxString rowCaption(int rowIndx) const;

		virtual wxString cellValue(int x, int y) const;

		virtual double cellNumericValue(int x, int y) const;

		virtual bool cellValue(int, int, const Yuni::String&);

		virtual void resetColors(int, int, wxColour&, wxColour&) const
		{/*Do nothing*/}

		virtual Date::Precision precision() {return Date::monthly;}

		virtual IRenderer::CellStyle cellStyle(int col, int row) const;

		virtual bool valid() const;

		virtual uint maxWidthResize() const {return 0;}
		virtual uint maxHeightResize() const {return 0;}

		virtual bool circularShiftRowsUntilDate(MonthName month, uint daymonth);

	protected:
		virtual void internalAreaChanged(Antares::Data::Area* area);
		//! Event: the study has been closed
		virtual void onStudyClosed() override;
		//! Event: the study has been loaded
		virtual void onStudyLoaded() override;

	}; // class HydroPrepro






} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_HYDROPREPRO_H__
