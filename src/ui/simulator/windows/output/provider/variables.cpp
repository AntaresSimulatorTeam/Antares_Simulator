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

#include "variables.h"
#include <cassert>
#include "../output.h"
#include "../content.h"
#include "../../../toolbox/resources.h"
#include <antares/study/study.h>
#include <antares/study/memory-usage.h>
#include "../../../../../solver/variable/economy/all.h"

using namespace Yuni;



namespace Antares
{
namespace Window
{
namespace OutputViewer
{
namespace Provider
{


	namespace // anonymous namespace
	{

		class VariableCollector
		{
		public:
			//! Spotlight (alias)
			typedef Antares::Component::Spotlight Spotlight;

		public:
			VariableCollector(Spotlight::IItem::Vector& out, const Spotlight::SearchToken::Vector& tokens,
				wxBitmap* bmp, wxBitmap* thermalBmp, const String& selection) :
				pOut(out),
				pTokens(tokens),
				pBmp(bmp),
				pThermalBmp(thermalBmp),
				pCurrentSelection(selection)
			{
			}


			void add(const AnyString& name, const AnyString& /*unit*/, const AnyString& /*comments*/)
			{
				if (!pTokens.empty())
				{
					Spotlight::SearchToken::Vector::const_iterator end = pTokens.end();
					Spotlight::SearchToken::Vector::const_iterator i = pTokens.begin();
					bool gotcha = false;
					for (; i != end; ++i)
					{
						const String& text = (*i)->text;
						if (name.icontains(text))
						{
							gotcha = true;
							break;
						}
					}
					if (!gotcha)
						return;
				}

				Spotlight::IItem* item = new Spotlight::IItem();
				if (item)
				{
					item->caption(name);
					item->group("Variables");
					if (pBmp)
						item->image(*pBmp);
					if (pCurrentSelection == name)
						item->select();
					pOut.push_back(item);
				}
			}

			void addCluster(const String& name)
			{
				if (!pTokens.empty())
				{
					Spotlight::SearchToken::Vector::const_iterator end = pTokens.end();
					Spotlight::SearchToken::Vector::const_iterator i = pTokens.begin();
					bool gotcha = false;
					for (; i != end; ++i)
					{
						const String& text = (*i)->text;
						if (name.icontains(text))
						{
							gotcha = true;
							break;
						}
					}
					if (!gotcha)
						return;
				}

				Spotlight::IItem* item = new Spotlight::IItem();
				if (item)
				{
					item->caption(name);
					item->group("Clusters");
					if (pThermalBmp)
						item->image(*pThermalBmp);
					if (pCurrentSelection == name)
						item->select();
					pOut.push_back(item);
				}
			}


		private:
			//! The results
			Spotlight::IItem::Vector& pOut;
			//! Search tokens
			const Spotlight::SearchToken::Vector& pTokens;
			//! Bitmap
			wxBitmap* pBmp;
			wxBitmap* pThermalBmp;
			//!
			const String& pCurrentSelection;

		}; // class VariableCollector


	} // anonymous namespace





	Variables::Variables(Component* com) :
		pComponent(com)
	{
		assert(pComponent);

		pBmpVariable = Resources::BitmapLoadFromFile("images/16x16/variable.png");
		pBmpThermal  = Resources::BitmapLoadFromFile("images/16x16/thermal.png");
	}


	Variables::~Variables()
	{
		delete pBmpVariable;
		delete pBmpThermal;
	}


	void Variables::search(Spotlight::IItem::Vector& out, const Spotlight::SearchToken::Vector& tokens, const Yuni::String& text)
	{
		if (!pComponent)
			return;

		const String& selection = pComponent->pCurrentVariable;
		VariableCollector collector(out, tokens, pBmpVariable, pBmpThermal, selection);

		// Browse all available variables
		Antares::Solver::Variable::Economy::AllVariables::RetrieveVariableList(collector);

		// Adding all thermal clusters
		if (pComponent->pCurrentClusters)
		{
			typedef Antares::Private::OutputViewerData::ThermalNameSetPerArea ThermalNameSetPerArea;
			typedef Antares::Private::OutputViewerData::ThermalNameSet ThermalNameSet;

			String name;
			const ThermalNameSetPerArea::const_iterator areaEnd = pComponent->pCurrentClusters->end();
			ThermalNameSetPerArea::const_iterator areaI = pComponent->pCurrentClusters->begin();
			for (; areaI != areaEnd; ++areaI)
			{
				const Data::AreaName& area = areaI->first;
				const ThermalNameSet::const_iterator end = areaI->second.end();
				ThermalNameSet::const_iterator i = areaI->second.begin();
				for (; i != end; ++i)
				{
					name.clear() << area << " :: " << *i;
					collector.addCluster(name);
				}
			}
		}
	}


	bool Variables::onSelect(Spotlight::IItem::Ptr& item)
	{
		if (!pComponent || !item || item->caption().empty())
			return false;

		pComponent->pCurrentVariable = item->caption();
		String::Size offset = pComponent->pCurrentVariable.find(" :: ");
		if (offset < pComponent->pCurrentVariable.size())
			pComponent->pCurrentVariable.consume(offset + 4);

		// Upper case
		pComponent->pCurrentVariable.trim();
		pComponent->pCurrentVariable.toUpper();

		Component::ConvertVarNameToID(pComponent->pCurrentVariableID, pComponent->pCurrentVariable);
		Dispatcher::GUI::Post(pComponent, &Component::updateGlobalSelection);
		return true;
	}







} // namespace Provider
} // namespace OutputViewer
} // namespace Window
} // namespace Antares

