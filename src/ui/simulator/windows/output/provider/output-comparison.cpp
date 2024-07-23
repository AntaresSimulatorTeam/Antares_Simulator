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

#include "output-comparison.h"
#include <cassert>
#include "../output.h"
#include "../layer.h"
#include "../content.h"
#include "../../../toolbox/resources.h"
#include <antares/study/study.h>

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace OutputViewer
{
namespace Provider
{
Comparison::Comparison(Component& com) : pComponent(com)
{
    assert(&pComponent);

    pBmpSum = Resources::BitmapLoadFromFile("images/16x16/sum.png");
    pBmpAverage = Resources::BitmapLoadFromFile("images/16x16/average.png");
    pBmpMin = Resources::BitmapLoadFromFile("images/16x16/bottom.png");
    pBmpMax = Resources::BitmapLoadFromFile("images/16x16/top.png");
    pBmpDiff = Resources::BitmapLoadFromFile("images/16x16/merge.png");
}

Comparison::~Comparison()
{
    delete pBmpSum;
    delete pBmpAverage;
    delete pBmpMin;
    delete pBmpMax;
    delete pBmpDiff;
}

void Comparison::search(Spotlight::IItem::Vector& out,
                        const Spotlight::SearchToken::Vector&,
                        const Yuni::String& /* text */)
{
    {
        auto item = std::make_shared<Spotlight::IItem>();
        item->caption("Average");
        item->group("comparison");
        item->image(pBmpAverage);
        item->tag = ltAverage;
        out.push_back(item);
    }
    {
        auto item = std::make_shared<Spotlight::IItem>();
        item->caption("Differences");
        item->group("comparison");
        item->image(pBmpDiff);
        item->tag = ltDiff;
        out.push_back(item);
    }
    {
        auto item = std::make_shared<Spotlight::IItem>();
        item->caption("Minimum");
        item->group("comparison");
        item->image(pBmpMin);
        item->tag = ltMin;
        out.push_back(item);
    }
    {
        auto item = std::make_shared<Spotlight::IItem>();
        item->caption("Maximum");
        item->group("comparison");
        item->image(pBmpMax);
        item->tag = ltMax;
        out.push_back(item);
    }
    {
        auto item = std::make_shared<Spotlight::IItem>();
        item->caption("Sum");
        item->group("comparison");
        item->image(pBmpSum);
        item->tag = (int)ltSum;
        out.push_back(item);
    }
}

bool Comparison::onSelect(Spotlight::IItem::Ptr& item)
{
    if (!(!item) && (uint)item->tag <= ltMax)
    {
        pComponent.createNewVirtualLayer((LayerType)item->tag);
        return true;
    }
    return false;
}

} // namespace Provider
} // namespace OutputViewer
} // namespace Window
} // namespace Antares
