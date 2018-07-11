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

#include <antares/study/xcast/xcast.h>


namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{


	template<enum Data::TimeSeries T>
	XCastK<T>::XCastK(wxWindow* control, Toolbox::InputSelector::Area* notifier) :
		MatrixAncestorType(control),
		Renderer::ARendererArea(control, notifier)
	{}


	template<enum Data::TimeSeries T>
	XCastK<T>::~XCastK()
	{
		destroyBoundEvents();
	}


	template<enum Data::TimeSeries T>
	wxString XCastK<T>::columnCaption(int colIndx) const
	{
		return (colIndx < 12)
			? wxStringFromUTF8(Date::MonthToString(colIndx)) << wxT("  ")
			: wxString();
	}


	template<enum Data::TimeSeries T>
	wxString XCastK<T>::cellValue(int x, int y) const
	{
		return MatrixAncestorType::cellValue(x,y);
	}


	template<enum Data::TimeSeries T>
	double XCastK<T>::cellNumericValue(int x, int y) const
	{
		return MatrixAncestorType::cellNumericValue(x,y);
	}


	template<enum Data::TimeSeries T>
	bool XCastK<T>::cellValue(int x, int y, const Yuni::String& value)
	{
		return MatrixAncestorType::cellValue(x,y,value);
	}


	template<enum Data::TimeSeries T>
	void XCastK<T>::internalAreaChanged(Data::Area* area)
	{
		if (area)
		{
			auto* xcastData = area->xcastData<T>();
			this->matrix(&(xcastData->K));
		}
		else
			this->matrix(nullptr);
		// parent
		Renderer::ARendererArea::internalAreaChanged(area);
	}


	template<enum Data::TimeSeries T>
	inline IRenderer::CellStyle XCastK<T>::cellStyle(int col, int row) const
	{
		return MatrixAncestorType::cellStyle(col, row);
	}


	template<enum Data::TimeSeries T>
	inline wxString XCastK<T>::rowCaption(int rowIndx) const
	{
		return wxString() << rowIndx;
	}


	template<enum Data::TimeSeries T>
	inline int XCastK<T>::width() const
	{
		return MatrixAncestorType::width();
	}


	template<enum Data::TimeSeries T>
	inline int XCastK<T>::height() const
	{
		return MatrixAncestorType::height();
	}



	template<enum Data::TimeSeries T>
	inline uint XCastK<T>::maxWidthResize() const
	{
		return 0;
	}


	template<enum Data::TimeSeries T>
	inline bool XCastK<T>::valid() const
	{
		return MatrixAncestorType::valid();
	}


	template<enum Data::TimeSeries T>
	void XCastK<T>::onStudyClosed()
	{
		Renderer::Matrix<float>::onStudyClosed();
		Renderer::ARendererArea::onStudyClosed();
	}


	template<enum Data::TimeSeries T>
	void XCastK<T>::onStudyLoaded()
	{
		Renderer::Matrix<float>::onStudyLoaded();
		Renderer::ARendererArea::onStudyLoaded();
	}





} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

