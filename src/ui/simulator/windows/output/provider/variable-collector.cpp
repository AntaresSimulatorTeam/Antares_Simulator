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
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "variable-collector.h"

namespace Antares
{
namespace Window
{
namespace OutputViewer
{
namespace Provider
{
VariableCollector::VariableCollector(Spotlight::IItem::Vector& out,
                                     const Spotlight::SearchToken::Vector& tokens,
                                     wxBitmap* bmp,
                                     wxBitmap* thermalBmp,
                                     const String& selection) :
 pOut(out), pTokens(tokens), pBmp(bmp), pThermalBmp(thermalBmp), pCurrentSelection(selection)
{
}

void VariableCollector::add(const AnyString& name,
                            const AnyString& /*unit*/,
                            const AnyString& /*comments*/)
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

    auto item = std::make_shared<Spotlight::IItem>();
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

void VariableCollector::addCluster(const String& name)
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

    auto item = std::make_shared<Spotlight::IItem>();
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

} // namespace Provider
} // namespace OutputViewer
} // namespace Window
} // namespace Antares
