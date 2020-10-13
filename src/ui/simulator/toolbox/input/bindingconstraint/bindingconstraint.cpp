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

#include <antares/wx-wrapper.h>
#include "bindingconstraint.h"
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <assert.h>
#include <wx/bmpbuttn.h>
#include <wx/msgdlg.h>
#include <wx/wupdlock.h>

#include "../../components/captionpanel.h"
#include "../../../application/study.h"
#include "../../resources.h"
#include "../../../application/study.h"
#include "../../../application/main.h"

#include <ui/common/component/spotlight.h>
#include <ui/common/lock.h>
#include "../../spotlight/constraint.h"

using namespace Yuni;



namespace Antares
{
namespace Toolbox
{
namespace InputSelector
{


	class SpotlightProviderConstraint final : public Component::Spotlight::IProvider
	{
	public:
		typedef Antares::Component::Spotlight  Spotlight;

	public:
		SpotlightProviderConstraint(Toolbox::InputSelector::BindingConstraint* component) :
			pComponent(component)
		{
			assert(pComponent);

			pBmpOn  = Resources::BitmapLoadFromFile("images/16x16/light_green.png");
			pBmpOff = Resources::BitmapLoadFromFile("images/16x16/light_red.png");

			OnStudyThermalClusterRenamed.connect(this, &SpotlightProviderConstraint::onStudyClusterRename);
			OnStudyAreaRename.connect(this, &SpotlightProviderConstraint::onStudyAreaRename);
			OnStudyEndUpdate.connect(this, &SpotlightProviderConstraint::onStudyEndUpdate);
			OnStudyAreasChanged.connect(this, &SpotlightProviderConstraint::onStudyAreasChanged);
			OnStudyAreaDelete.connect(this, &SpotlightProviderConstraint::onStudyAreaDeleted);
			OnStudyAreaAdded.connect(this, &SpotlightProviderConstraint::onStudyAreaAdded);
			OnStudyClosed.connect(this, &SpotlightProviderConstraint::onStudyClosed);
			OnStudyLinkAdded.connect(this, &SpotlightProviderConstraint::onStudyLinkUpdate);
			OnStudyLinkDelete.connect(this, &SpotlightProviderConstraint::onStudyLinkUpdate);
			OnStudyConstraintAdded.connect(this, &SpotlightProviderConstraint::onStudyConstraintUpdate);
			OnStudyConstraintDelete.connect(this, &SpotlightProviderConstraint::onStudyConstraintUpdate);
			OnStudyConstraintModified.connect(this, &SpotlightProviderConstraint::onStudyConstraintUpdate);
		}

		virtual ~SpotlightProviderConstraint()
		{
			destroyBoundEvents(); // avoid corrupt vtable
			delete pBmpOn;
			delete pBmpOff;
		}

		virtual void search(Spotlight::IItem::Vector& out, const Spotlight::SearchToken::Vector& tokens, const Yuni::String& text)
		{
			if (not Data::Study::Current::Valid())
				return;

			String tmp;
			auto& study = *Data::Study::Current::Get();

			const Spotlight::SearchToken::Vector::const_iterator tend = tokens.end();
			Data::BindConstList::iterator end = study.bindingConstraints.end();
			Data::BindConstList::iterator i = study.bindingConstraints.begin();

			std::vector<Data::BindingConstraint*> layerFilteredItems;
			if (!text.empty())
			{
				auto layerListEnd = study.layers.end();
				for (auto layerIt = study.layers.begin(); layerIt != layerListEnd; layerIt++)
				{
					if (layerIt->second == text)
					{

						for (; i != end; ++i)
						{
							// The current constraint
							Data::BindingConstraint& constraint = *(*i);

							if ((constraint.hasAllWeightedLinksOnLayer(layerIt->first) && constraint.hasAllWeightedClustersOnLayer(layerIt->first)) || layerIt == study.layers.begin())//all binding constraints are shown for first layer
							{
								layerFilteredItems.push_back(&constraint);
							}

						}
						break;
					}
				}
			}

			Data::BindConstList::iterator j;
			Data::BindConstList::iterator endJ;

			j = layerFilteredItems.begin();
			endJ = layerFilteredItems.end();
			

			for (; j != endJ; ++j)
			{
				// The current constraint
				Data::BindingConstraint& constraint = *(*j);

				if (!tokens.empty())
				{
					bool go = false;
					Spotlight::SearchToken::Vector::const_iterator ti = tokens.begin();
					for (; ti != tend; ++ti)
					{
						const String& text = (*ti)->text;
						if (constraint.name().icontains(text))
						{
							go = true;
							break;
						}
					}
					if (!go)
						continue;
				}

				auto* item = new Toolbox::Spotlight::ItemConstraint(&constraint);
				if (constraint.enabled() && (constraint.linkCount() > 0 || constraint.enabledClusterCount() > 0))
				{
					if (pBmpOn)
						item->image(*pBmpOn);
				}
				else
				{
					if (pBmpOff)
						item->image(*pBmpOff);
				}
				out.push_back(item);
			}
		}


		virtual bool onSelect(Spotlight::IItem::Ptr& item)
		{
			if (not Data::Study::Current::Valid())
				return false;

			GUILocker locker;
			typedef Toolbox::Spotlight::ItemConstraint::Ptr ItemConstraint;
			ItemConstraint itemconstraint = Spotlight::IItem::Ptr::DynamicCast<ItemConstraint>(item);
			if (!(!itemconstraint))
			{
				Data::BindingConstraint* constraint = itemconstraint->constraint;
				pComponent->onBindingConstraintChanged(constraint);
				return true;
			}
			return false;
		}

		virtual bool onDoubleClickSelect(Spotlight::IItem::Ptr& item)
		{
			if (not Data::Study::Current::Valid())
				return false;

			GUILocker locker;
			typedef Toolbox::Spotlight::ItemConstraint::Ptr ItemConstraint;
			ItemConstraint itemconstraint = Spotlight::IItem::Ptr::DynamicCast<ItemConstraint>(item);
			if (!(!itemconstraint))
			{
				Data::BindingConstraint* constraint = itemconstraint->constraint;
				pComponent->onBindingConstraintDblClick(constraint);
				return true;
			}

			return true;
		}

	protected:
		void onStudyEndUpdate()
		{
			redoResearch();
		}

		void onStudyAreasChanged()
		{
			redoResearch();
		}

		void onStudyAreaAdded(Data::Area*)
		{
			redoResearch();
		}

		void onStudyAreaDeleted(Data::Area*)
		{
			redoResearch();
		}

		void onStudyAreaRename(Data::Area*)
		{
			redoResearch();
		}

		void onStudyClusterRename(Data::ThermalCluster*)
		{
			redoResearch();
		}

		void onStudyClosed()
		{
			if (component())
			{
				// The component will be updated
				component()->resetSearchInput();
			}
			else
				redoResearch(); // automatically called by resetSearchInput
		}

		void onStudyLinkUpdate(Data::AreaLink*)
		{
			redoResearch();
		}

		void onStudyConstraintUpdate(Data::BindingConstraint*)
		{
			redoResearch();
		}


	private:
		wxBitmap* pBmpOn;
		wxBitmap* pBmpOff;
		Toolbox::InputSelector::BindingConstraint* pComponent;

	}; // class SpotlightProviderConstraint








	BindingConstraint::BindingConstraint(wxWindow* parent) :
		AInput(parent),
		Yuni::IEventObserver<BindingConstraint>()
	{
		SetSize(300, 350);
		this->internalBuildSubControls();
		this->update();

		// Connect to the global event
		OnStudyClosed.connect(this, &BindingConstraint::onUpdate);
		OnStudyLoaded.connect(this, &BindingConstraint::onUpdate);

		OnStudyEndUpdate.connect(this, &BindingConstraint::update);
		OnStudyAreasChanged.connect(this, &BindingConstraint::update);
		OnStudyAreaRename.connect(this, &BindingConstraint::onStudyAreaUpdate);
		OnStudyAreaDelete.connect(this, &BindingConstraint::onStudyAreaUpdate);
		OnStudyLinkAdded.connect(this, &BindingConstraint::onStudyLinkUpdate);
		OnStudyLinkDelete.connect(this, &BindingConstraint::onStudyLinkUpdate);

		// Avoid SegV at exit
		Forms::ApplWnd::Instance()->onApplicationQuit.connect(this, &BindingConstraint::onApplicationOnQuit);
	}


	BindingConstraint::~BindingConstraint()
	{
		// reset
		clear();
		destroyBoundEvents();
	}




	void BindingConstraint::internalBuildSubControls()
	{
		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		SetSizer(sizer);

		{
			Component::Spotlight* spotlight = new Component::Spotlight(this, 0); // Component::Spotlight::optGroups);
			OnMapLayerChanged.connect(spotlight, &Component::Spotlight::onMapLayerChanged);
			OnMapLayerAdded.connect(spotlight, &Component::Spotlight::onMapLayerAdded);
			OnMapLayerRemoved.connect(spotlight, &Component::Spotlight::onMapLayerRemoved);
			OnMapLayerRenamed.connect(spotlight, &Component::Spotlight::onMapLayerRenamed);
			spotlight->itemHeight(34);
			spotlight->provider(new SpotlightProviderConstraint(this));

			wxBoxSizer* hz = new wxBoxSizer(wxHORIZONTAL);
			hz->AddSpacer(10);
			hz->Add(spotlight, 1, wxALL|wxEXPAND);
			hz->AddSpacer(5);
			sizer->Add(hz, 1, wxALL|wxEXPAND);
		}

		sizer->Layout();
	}


	void BindingConstraint::update()
	{
	}


	void BindingConstraint::updateRowContent()
	{
	}


	void BindingConstraint::onUpdate()
	{
		update();
	}


	void BindingConstraint::onStudyAreaUpdate(Data::Area*)
	{
		update();
	}


	void BindingConstraint::onStudyLinkUpdate(Data::AreaLink*)
	{
		update();
	}


	void BindingConstraint::onApplicationOnQuit()
	{
		clear();
	}


	void BindingConstraint::updateInnerData(const wxString&)
	{
	}


	void BindingConstraint::clear()
	{
		GUILocker locker;
		onBindingConstraintChanged(nullptr);
	}





} // namespace InputSelector
} // namespace Toolbox
} // namespace Antares

