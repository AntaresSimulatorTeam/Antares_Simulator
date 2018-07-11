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

#include "cluster.h"
#include "../../windows/inspector.h"
#include "../../toolbox/components/datagrid/renderer/area/thermalmodulation.h"
#include "../../toolbox/components/notebook/notebook.h"
#include "../../toolbox/components/refresh.h"
#include <antares/study/parts/thermal/cluster.h>
#include <wx/sizer.h>
#include "../../windows/inspector/frame.h"
#include <ui/common/dispatcher/gui.h>


using namespace Yuni;



namespace Antares
{
namespace Window
{
namespace Thermal
{


	CommonProperties::CommonProperties(wxWindow* parent, Toolbox::InputSelector::ThermalCluster* notifier) :
		Component::Panel(parent),
		pMainSizer(nullptr),
		pAggregate(nullptr),
		pNotifier(notifier),
		pGroupHasChanged(false)
	{
		// The main sizer
		wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
		pMainSizer = sizer;
		SetSizer(sizer);

		auto* inspector = new Window::Inspector::Frame(this);
		pUpdateInfoAboutPlant.bind(inspector, &Window::Inspector::Frame::apply);

		wxBoxSizer* vs = new wxBoxSizer(wxVERTICAL);
		vs->Add(inspector, 1, wxALL|wxEXPAND);
		sizer->Add(vs, 0, wxALL|wxEXPAND);
		sizer->SetItemMinSize(inspector, 280, 50);

		sizer->Add(new Component::Datagrid::Component(this,
			new Component::Datagrid::Renderer::ThermalClusterCommonModulation(this, notifier)),
			1, wxALL|wxEXPAND);

		// Connection with the notifier
		thermalEventConnect();

		OnStudyThermalClusterRenamed.connect(this, &CommonProperties::onStudyThermalClusterRenamed);
		OnStudyThermalClusterCommonSettingsChanged.connect(this, &CommonProperties::thermalSettingsChanged);
		OnStudyClosed.connect(this, &CommonProperties::onStudyClosed);
	}


	CommonProperties::~CommonProperties()
	{
		pUpdateInfoAboutPlant.clear();
		destroyBoundEvents();
	}


	void CommonProperties::onThermalClusterChanged(Data::ThermalCluster* cluster)
	{
		if (cluster)
		{
			auto* data = new Window::Inspector::InspectorData(*Data::Study::Current::Get());
			data->clusters.insert(cluster);
			pUpdateInfoAboutPlant(data);
		}
		else
			pUpdateInfoAboutPlant(nullptr);

		pGroupHasChanged = false;
		pAggregate = cluster;
	}


	void CommonProperties::onStudyClosed()
	{
		onThermalClusterChanged(nullptr);
	}


	void CommonProperties::onUpdAggregateListDueToGroupChange()
	{
		if (pGroupHasChanged)
		{
			pGroupHasChanged = false;
			if (pNotifier)
			{
				thermalEventDisconnect();
				pNotifier->update();
				pNotifier->Refresh();

				// (Re) Connection with the notifier
				pNotifier->UpdateWindowUI();
				thermalEventConnect();
			}
		}
	}


	void CommonProperties::thermalEventConnect()
	{
		if (pNotifier)
			pNotifier->onThermalClusterChanged.connect(this, &CommonProperties::onThermalClusterChanged);
	}


	void CommonProperties::thermalEventDisconnect()
	{
		if (pNotifier)
			pNotifier->onThermalClusterChanged.remove(this);
	}


	void CommonProperties::onStudyThermalClusterRenamed(Data::ThermalCluster* cluster)
	{
		if (cluster == pAggregate and cluster)
			onThermalClusterChanged(cluster);
		Dispatcher::GUI::Refresh(this);
	}


	void CommonProperties::thermalSettingsChanged()
	{
		Dispatcher::GUI::Refresh(this);
	}



} // namespace Thermal
} // namespace Window
} // namespace Antares
