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
#ifndef __ANTARES_APPLICATION_WINDOW_THERMAL_COMMON_H__
# define __ANTARES_APPLICATION_WINDOW_THERMAL_COMMON_H__

# include <antares/wx-wrapper.h>
# include <ui/common/component/panel.h>
# include "../../toolbox/components/datagrid/component.h"
# include "../../toolbox/input/thermal-cluster.h"
# include "../../toolbox/validator.h"
# include "../../windows/inspector/data.h"



namespace Antares
{
namespace Window
{
namespace Thermal
{


	class CommonProperties : public Component::Panel, public Yuni::IEventObserver<CommonProperties>
	{
	public:
		CommonProperties(wxWindow* parent, Toolbox::InputSelector::ThermalCluster* notifier);
		virtual ~CommonProperties();

	private:
		void onThermalClusterChanged(Data::ThermalCluster* cluster);

		void onUpdAggregateListDueToGroupChange();

		void thermalEventConnect();
		void thermalEventDisconnect();

		void onStudyThermalClusterRenamed(Data::ThermalCluster* cluster);

		void thermalSettingsChanged();

		void onStudyClosed();

	private:
		//! The main sizer
		wxSizer* pMainSizer;
		Data::ThermalCluster* pAggregate;
		Toolbox::InputSelector::ThermalCluster* pNotifier;
		bool pGroupHasChanged;

		Yuni::Bind<void (const Window::Inspector::InspectorData::Ptr&)> pUpdateInfoAboutPlant;

	}; // class Aggregate




} // namespace Thermal
} // namespace Window
} // namespace Antares


#endif // __ANTARES_APPLICATION_WINDOW_THERMAL_COMMON_H__
