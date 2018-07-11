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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_CONVERTION_H__
# define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_CONVERTION_H__

# include <antares/wx-wrapper.h>
# include "../area.h"
# include "../matrix.h"



namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{


	template<enum Data::TimeSeries T>
	class XCastConversion : public Renderer::Matrix<float>, public Renderer::ARendererArea
	{
	public:
		//! Ancestor
		typedef Renderer::Matrix<float>  MatrixAncestorType;

	public:
		//! \name Constructor & Destructor
		//@{

		/*!
		** \brief Constructor
		*/
		XCastConversion(wxWindow* control, Toolbox::InputSelector::Area* notifier);

		//! Destructor
		virtual ~XCastConversion();
		//@}

		virtual int width() const {return MatrixAncestorType::width() - 2;}
		virtual int height() const {return MatrixAncestorType::height();}

		virtual wxString columnCaption(int colIndx) const;

		virtual wxString rowCaption(int rowIndx) const;

		virtual wxString cellValue(int x, int y) const;

		virtual double cellNumericValue(int x, int y) const;

		virtual bool cellValue(int x, int y, const Yuni::String& value);

		virtual void resetColors(int, int, wxColour&, wxColour&) const
		{/*Do nothing*/}

		virtual IRenderer::CellStyle cellStyle(int col, int row) const;

		virtual uint maxWidthResize() const {return Data::XCast::conversionMaxPoints;}
		virtual bool valid() const {return MatrixAncestorType::valid();}

		virtual bool onMatrixResize(uint oldX, uint oldY, uint& newX, uint& newY);
		virtual void onMatrixLoad();

	protected:
		virtual void internalAreaChanged(Data::Area* area);
		//! Event: the study has been closed
		virtual void onStudyClosed() override;
		//! Event: the study has been loaded
		virtual void onStudyLoaded() override;

	}; // class XCastConversion






} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

# include "xcast-conversion.hxx"

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_CONVERTION_H__
