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
#ifndef __ANTARES_WINDOWS_INSPECTOR_EDITOR_AREA_THERMAL_CLUSTERS_H__
# define __ANTARES_WINDOWS_INSPECTOR_EDITOR_AREA_THERMAL_CLUSTERS_H__

#include <antares/wx-wrapper.h>
#include <wx/menu.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/editors.h>




namespace Antares
{
namespace Window
{
namespace Inspector
{


	class AreaThermalClustersBtnEditor : public wxPGTextCtrlEditor
	{
		DECLARE_DYNAMIC_CLASS(AreaThermalClustersBtnEditor)
	public:
		AreaThermalClustersBtnEditor();
		virtual ~AreaThermalClustersBtnEditor();

		virtual wxPGWindowList CreateControls(wxPropertyGrid* propgrid,
			wxPGProperty* property,
			const wxPoint& pos,
			const wxSize& size) const;

		virtual wxString GetName() const;

		virtual bool OnEvent( wxPropertyGrid* grid,
			wxPGProperty* property, wxWindow* ctrl, wxEvent& event ) const;

	private:
		mutable wxMenu* pMenu;

	}; // class AreaThermalClustersBtnEditor

	WX_PG_DECLARE_EDITOR(AreaThermalClustersBtnEditor)




} // namespace Inspector
} // namespace Window
} // namespace Antares

#endif // __ANTARES_WINDOWS_INSPECTOR_EDITOR_AREA_THERMAL_CLUSTERS_H__
