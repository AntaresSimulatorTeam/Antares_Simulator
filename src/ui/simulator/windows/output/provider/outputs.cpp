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
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/antares/antares.h"
#include "outputs.h"
#include <cassert>
#include "../output.h"
#include "../layer.h"
#include "../content.h"
#include "../../../toolbox/resources.h"
#include <antares/study/study.h>

#include <memory>

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace OutputViewer
{
namespace Provider
{
namespace // anonymous
{
class OutputSpotlightItem : public Antares::Component::Spotlight::IItem
{
public:
    //! Ptr
    using Ptr = std::shared_ptr<OutputSpotlightItem>;
    //! Vector of items
    using Vector = std::vector<Ptr>;
    //! Vector Ptr
    using VectorPtr = std::shared_ptr<Vector>;

public:
    OutputSpotlightItem()
    {
    }
    virtual ~OutputSpotlightItem()
    {
    }

public:
    Layer* layer;
    Data::Output::Ptr output;

}; // class OutputSpotlightItem

} // anonymous namespace

Outputs::Outputs(Component& com, Layer* layer) : pComponent(com), pLayer(layer)
{
    assert(&pComponent);
    pBmpClose = Resources::BitmapLoadFromFile("images/16x16/close.png");

    CString<32, false> filename;
    for (uint i = 0; i != 10; ++i)
    {
        filename.clear() << "images/16x16/" << i << ".png";
        pBmpNumbers[i] = Resources::BitmapLoadFromFile(filename.c_str());
    }
    pBmpMultiple = Resources::BitmapLoadFromFile("images/16x16/000.png");
    pBmpEmpty = Resources::BitmapLoadFromFile("images/16x16/empty.png");
}

Outputs::~Outputs()
{
    for (uint i = 0; i != 10; ++i)
        delete pBmpNumbers[i];
    delete pBmpMultiple;
    delete pBmpEmpty;
    delete pBmpClose;
}

void Outputs::search(Spotlight::IItem::Vector& out,
                     const Spotlight::SearchToken::Vector& tokens,
                     const Yuni::String& /* text */)
{
    // More than one tab: we would like to be able to close the current one
    if (pLayer && pComponent.pTabs.size() > 1)
    {
        auto item = std::make_shared<OutputSpotlightItem>();
        item->caption("Close the tab");
        item->image(pBmpClose);
        item->tag = -1;
        item->layer = pLayer;
        item->countedAsResult(false);
        out.push_back(item);
    }
    if (pLayer && pLayer->isVirtual())
    {
    }
    else
    {
        // OutputSpotlightItem
        if (pLayer && pComponent.pTabs.size() > 1)
            out.push_back(std::make_shared<Spotlight::Separator>());

        Data::Output::Ptr outputSelected;
        if (pLayer)
            outputSelected = pLayer->selection;

        Data::Output::MapByTimestampDesc map;
        foreach (Data::Output::Ptr output, pComponent.pOutputs)
        {
            assert(!(!output) && "invalid output");
            if (!(!output))
                map[(output->timestamp)] = output;
        }

        auto end = map.end();
        auto tend = tokens.end();

        for (auto i = map.begin(); i != end; ++i)
        {
            Data::Output::Ptr& output = i->second;
            assert(!(!output));

            auto& title = output->title;

            if (not tokens.empty())
            {
                bool canContinue = false;
                for (auto ti = tokens.begin(); ti != tend; ++ti)
                {
                    if (title.icontains((*ti)->text))
                    {
                        canContinue = true;
                        break;
                    }
                }
                if (not canContinue)
                    continue;
            }

            auto item = std::make_shared<OutputSpotlightItem>();
            if (output->version != Data::StudyVersion::latest())
            {
                CString<16, false> text;
                text << 'v' << output->version.toString();
                item->addRightTag(text, 220, 220, 240);
            }
            item->caption(title);
            item->group("outputs");
            item->output = output;
            item->layer = pLayer;

            switch (output->mode)
            {
            case Data::SimulationMode::Economy:
                item->addTag("ECO", 162, 178, 197);
                break;
            case Data::SimulationMode::Adequacy:
                item->addTag("ADQ", 220, 192, 245);
                break;
            case Data::SimulationMode::Expansion:
                item->addTag("EXP", 220, 192, 245);
                break;
            case Data::SimulationMode::Unknown:
                item->addTag("...", 213, 213, 213);
            }
            int imgIndex = pComponent.imageIndexForOutput(output);
            switch (imgIndex)
            {
            case -1:
                item->image(pBmpMultiple);
                break;
            case -2:
                item->image(pBmpEmpty);
                break;
            default:
                if (imgIndex < 9)
                    item->image(pBmpNumbers[imgIndex + 1]);
                else
                    item->image(pBmpEmpty);
            }
            out.push_back(item);
        }
    }
}

bool Outputs::onSelect(Spotlight::IItem::Ptr& item)
{
    auto withlayer = std::dynamic_pointer_cast<OutputSpotlightItem>(item);
    if (!item)
        return true;

    if (not withlayer->output)
        pComponent.removeOutput(withlayer->layer);
    else
        pComponent.selectAnotherOutput(withlayer->output);

    return true;
}

} // namespace Provider
} // namespace OutputViewer
} // namespace Window
} // namespace Antares
