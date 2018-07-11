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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_ALL_AREAS_H__
# define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_ALL_AREAS_H__

# include <antares/wx-wrapper.h>
# include "../../renderer.h"
# include <antares/study.h>
# include <yuni/core/event.h>


namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{


	template<enum Data::TimeSeries T>
	class XCastAllAreas final : public Renderer::IRenderer
	{
	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Constructor
		*/
		XCastAllAreas(wxWindow* parent, Toolbox::InputSelector::Area* notifier);

		//! Destructor
		virtual ~XCastAllAreas();
		//@}

		virtual int width() const;

		virtual int height() const;

		virtual wxString columnCaption(int colIndx) const;

		virtual wxString rowCaption(int rowIndx) const;

		virtual wxString cellValue(int, int) const;

		virtual double cellNumericValue(int,int) const;

		virtual bool cellValue(int x, int y, const Yuni::String& value);

		virtual void resetColors(int, int, wxColour&, wxColour&) const
		{/*Do nothing*/}

		virtual IRenderer::CellStyle cellStyle(int col, int row) const;
		virtual wxColour cellBackgroundColor(int, int) const;

		virtual bool valid() const;

	protected:
		virtual void onUpdate() {}
		void onAreaRenamed(Data::Area* area);

	private:
		wxWindow* pControl;
		Toolbox::InputSelector::Area* pNotifier;

	}; // class XCastAllAreas





} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

# include "xcast-allareas.hxx"

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_ALL_AREAS_H__
