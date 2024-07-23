/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "variables.h"
#include <cassert>
#include "../output.h"
#include "../content.h"
#include "../../../toolbox/resources.h"
#include <antares/study/study.h>
#include "antares/solver/variable/economy/all.h"
#include "variable-collector.h"

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace OutputViewer
{
namespace Provider
{
Variables::Variables(Component* com) : pComponent(com)
{
    assert(pComponent);

    pBmpVariable = Resources::BitmapLoadFromFile("images/16x16/variable.png");
    pBmpThermal = Resources::BitmapLoadFromFile("images/16x16/thermal.png");
}

Variables::~Variables()
{
    delete pBmpVariable;
    delete pBmpThermal;
}

void Variables::search(Spotlight::IItem::Vector& out,
                       const Spotlight::SearchToken::Vector& tokens,
                       const Yuni::String& /* text */)
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
        using ThermalNameSetPerArea = Antares::Private::OutputViewerData::ThermalNameSetPerArea;
        using ThermalNameSet = Antares::Private::OutputViewerData::ThermalNameSet;

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
