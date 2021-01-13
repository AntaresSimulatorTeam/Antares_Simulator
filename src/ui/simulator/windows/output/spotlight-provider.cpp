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

#include "spotlight-provider.h"
#include <cassert>
#include "output.h"
#include "content.h"
#include "../../toolbox/resources.h"
#include <ui/common/lock.h>

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace OutputViewer
{
namespace // anonymous
{
class ItemAreaLink final : public Antares::Component::Spotlight::IItem
{
public:
    //! Smart ptr
    typedef Yuni::SmartPtr<ItemAreaLink> Ptr;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    template<class StringT>
    ItemAreaLink(SelectionType sel, const StringT& itemname) : selectionType(sel), id(itemname)
    {
        caption(id);
    }

    //! Destructor
    virtual ~ItemAreaLink()
    {
    }
    //@}

public:
    const SelectionType selectionType;
    const String id;

}; // class ItemArea

} // anonymous namespace

SpotlightProviderGlobalSelection::SpotlightProviderGlobalSelection(Component* com) :
 pComponent(com), pCurrentAreaOrLink(nullptr)
{
    assert(pComponent);

    pBmpReattach = Resources::BitmapLoadFromFile("images/16x16/unpin.png");
    pBmpGroup = Resources::BitmapLoadFromFile("images/16x16/group.png");
    pBmpArea = Resources::BitmapLoadFromFile("images/16x16/area.png");
    pBmpLink = Resources::BitmapLoadFromFile("images/16x16/link.png");
}

SpotlightProviderGlobalSelection::~SpotlightProviderGlobalSelection()
{
    items.clear();
    delete pBmpReattach;
    delete pBmpArea;
    delete pBmpGroup;
    delete pBmpLink;
}

SpotlightProviderMCAll::SpotlightProviderMCAll(Component* com) : pComponent(com)
{
    assert(pComponent);

    pBmpReattach = Resources::BitmapLoadFromFile("images/16x16/attach.png");
    pBmpVariable = Resources::BitmapLoadFromFile("images/16x16/variable.png");
}

SpotlightProviderMCAll::~SpotlightProviderMCAll()
{
    delete pBmpReattach;
    delete pBmpVariable;
}

SpotlightProvider::SpotlightProvider(Component* com, Layer* layer) : pComponent(com), pLayer(layer)
{
    assert(pComponent);
    assert(pLayer);

    pBmpReattach = Resources::BitmapLoadFromFile("images/16x16/unpin.png");
    pBmpArea = Resources::BitmapLoadFromFile("images/16x16/area.png");
    pBmpGroup = Resources::BitmapLoadFromFile("images/16x16/group.png");
    pBmpLink = Resources::BitmapLoadFromFile("images/16x16/link.png");
}

SpotlightProvider::~SpotlightProvider()
{
    delete pBmpReattach;
    delete pBmpArea;
    delete pBmpGroup;
    delete pBmpLink;
}

void SpotlightProvider::search(Spotlight::IItem::Vector& out,
                               const Spotlight::SearchToken::Vector& tokens,
                               const Yuni::String& text)
{
    assert(pLayer);
    if (!pComponent || !pLayer)
        return;

    Data::Output::Ptr output = pLayer->selection;
    if (!output)
        return;
    const String& outputPath = output->path;
    if (!outputPath)
        return;

    // Looking for the list of areas and links
    Yuni::MutexLocker locker(pComponent->pMutex);

    auto it = pComponent->pAlreadyPreparedContents.find(outputPath);
    if (it == pComponent->pAlreadyPreparedContents.end())
        return;

    // Reattach
    {
        auto* item = new Spotlight::IItem();
        item->caption("Unpin");
        item->countedAsResult(false);

        if (pBmpReattach)
            item->image(*pBmpReattach);
        out.push_back(item);
        out.push_back(new Spotlight::Separator());
    }

    const auto& content = *it->second;

    const auto& list = content.economy;

    // All areas
    if (tokens.empty())
    {
        const auto end = list.areas.end();

        for (auto i = list.areas.begin(); i != end; ++i)
        {
            if (i->first() == '@')
                appendAreaName(out, *i);
        }
        for (auto i = list.areas.begin(); i != end; ++i)
        {
            if (i->first() != '@')
                appendAreaName(out, *i);
        }
    }
    else
    {
        auto tend = tokens.end();
        auto end = list.areas.end();

        for (auto i = list.areas.begin(); i != end; ++i)
        {
            auto& areaName = *i;
            if (areaName.first() != '@')
                continue;
            for (auto ti = tokens.begin(); ti != tend; ++ti)
            {
                auto& text = (*ti)->text;
                if (areaName.icontains(text))
                    appendAreaName(out, *i);
            }
        }
        for (auto i = list.areas.begin(); i != end; ++i)
        {
            const String& areaName = *i;
            if (areaName.first() == '@')
                continue;
            auto ti = tokens.begin();
            for (; ti != tend; ++ti)
            {
                auto& text = (*ti)->text;
                if (areaName.icontains(text))
                    appendAreaName(out, *i);
            }
        }
    }
    // All links
    if (tokens.empty())
    {
        auto end = list.links.end();
        for (auto i = list.links.begin(); i != end; ++i)
            appendLinkName(out, *i);
    }
    else
    {
        auto tend = tokens.end();
        auto end = list.links.end();
        for (auto i = list.links.begin(); i != end; ++i)
        {
            const String& linkName = *i;
            auto ti = tokens.begin();
            for (; ti != tend; ++ti)
            {
                auto& text = (*ti)->text;
                if (linkName.icontains(text))
                    appendLinkName(out, *i);
            }
        }
    }
}

void SpotlightProvider::appendAreaName(Spotlight::IItem::Vector& out, const String& name)
{
    auto* item = new ItemAreaLink(stArea, name);
    if (item)
    {
        if (name.first() == '@')
        {
            item->group("DISTRICTS");
            if (pBmpGroup)
                item->image(*pBmpGroup);
        }
        else
        {
            item->group("AREAS");
            if (pBmpArea)
                item->image(*pBmpArea);
        }
        if (pLayer && pLayer->customSelectionType == stArea && pLayer->customAreaOrLink == name)
            item->select();
        out.push_back(item);
    }
}

void SpotlightProvider::appendLinkName(Spotlight::IItem::Vector& out, const String& name)
{
    auto* item = new ItemAreaLink(stLink, name);
    if (item)
    {
        item->group("LINKS");
        if (pBmpLink)
            item->image(*pBmpLink);
        if (pLayer && pLayer->customSelectionType == stLink && pLayer->customAreaOrLink == name)
            item->select();
        out.push_back(item);
    }
}

bool SpotlightProvider::onSelect(Spotlight::IItem::Ptr& item)
{
    if (!pLayer || GUIIsLock() || !pComponent)
        return false;

    ItemAreaLink::Ptr arealink = Spotlight::IItem::Ptr::DynamicCast<ItemAreaLink::Ptr>(item);
    if (not arealink)
    {
        // Restoring the global selection
        pLayer->customSelectionType = pComponent->pCurrentSelectionType;
        pLayer->customAreaOrLink = pComponent->pCurrentAreaOrLink;
        pLayer->detached = false;
    }
    else
    {
        // Updating the area/link selection for the tab
        pLayer->customSelectionType = arealink->selectionType;
        pLayer->customAreaOrLink = arealink->id;
    }

    // Refresh the tab
    pComponent->refreshAllTabs();
    pComponent->refreshPanel(pLayer->index);
    pComponent->refreshAllPanelsWithVirtualLayer();
    return true;
}

void SpotlightProviderGlobalSelection::appendSetName(Spotlight::IItem::Vector& out,
                                                     const String& name,
                                                     const char* grp)
{
    auto* item = new ItemAreaLink(stArea, name);
    item->group(grp);
    if (pBmpGroup)
        item->image(*pBmpGroup);
    out.push_back(item);
}

void SpotlightProviderGlobalSelection::appendAreaName(Spotlight::IItem::Vector& out,
                                                      const String& name,
                                                      const char* grp)
{
    auto* item = new ItemAreaLink(stArea, name);
    item->group(grp);
    if (pBmpArea)
        item->image(*pBmpArea);
    out.push_back(item);
}

void SpotlightProviderGlobalSelection::appendLinkName(Spotlight::IItem::Vector& out,
                                                      const String& name,
                                                      const char* grp)
{
    auto* item = new ItemAreaLink(stLink, name);
    item->group(grp);
    if (pBmpLink)
        item->image(*pBmpLink);
    out.push_back(item);
}

size_t SpotlightProviderGlobalSelection::getSelectedLayerID(const Yuni::String& layerName)
{
    auto& study = *Data::Study::Current::Get();
    if (!study.areas.empty())
    {
        auto layerListEnd = study.layers.end();
        for (auto layerIt = study.layers.begin(); layerIt != layerListEnd; layerIt++)
        {
            if (layerIt->second == layerName)
            {
                return layerIt->first;
            }
        }
    }
    return -1;
}

void SpotlightProviderGlobalSelection::search(Spotlight::IItem::Vector& out,
                                              const Spotlight::SearchToken::Vector& tokens,
                                              const Yuni::String& text)
{
    auto& study = *Data::Study::Current::Get();

    /**
    ** Filtering sets of areas
    **/
    if (text == "Districts")
    {
        Spotlight::IItem::Vector layerFilteredItems;
        auto iend = items.end();
        auto i = items.begin();
        for (; i != iend; ++i)
        {
            auto& caption = (*i)->caption();
            if (caption.first() == '@')
                layerFilteredItems.push_back(*i);
        }
        search(out, tokens, layerFilteredItems);
        return;
    }

    /**
    ** Building the set of links in the study
    **/
    std::set<Data::AreaLink*> linkSet;
    {
        const Data::Area::Map::iterator end = study.areas.end();
        for (Data::Area::Map::iterator i = study.areas.begin(); i != end; ++i)
        {
            // Reference to the area
            Data::Area& area = *(i->second);

            // Foreach Interconnection for the area
            const Data::AreaLink::Map::iterator end = area.links.end();
            for (Data::AreaLink::Map::iterator i = area.links.begin(); i != end; ++i)
            {
                Data::AreaLink* lnk = i->second;
                // Adding the item for the interconnection
                linkSet.insert(lnk);
            }
        }
    }
    /**
    ** Filtering items not belonging to any map
    **/
    if (text == "Unknown")
    {
        Spotlight::IItem::Vector layerFilteredItems;
        auto iend = items.end();
        auto i = items.begin();
        for (; i != iend; ++i)
        {
            auto& caption = (*i)->caption();

            if (caption.first() == '@')
                continue;

            ItemAreaLink::Ptr arealink = Spotlight::IItem::Ptr::DynamicCast<ItemAreaLink::Ptr>(*i);

            if (arealink && arealink->selectionType == stArea)
            {
                bool foundInALayer = false;
                auto aEnd = study.areas.end();
                for (auto itArea = study.areas.begin(); itArea != aEnd; ++itArea)
                {
                    if (arealink->id == itArea->first) // this area is in the study (it is visible
                                                       // at least on layer 0)
                    {
                        foundInALayer = true;
                        break;
                    }
                }
                if (!foundInALayer)
                    layerFilteredItems.push_back(*i);
            }

            if (arealink && arealink->selectionType == stLink)
            {
                bool foundInALayer = false;
                auto sEnd = linkSet.end();
                for (auto itLink = linkSet.begin(); itLink != sEnd; ++itLink)
                {
                    wxString outputStyledName
                      = (wxString() << wxStringFromUTF8((*itLink)->from->name) << wxT(" - ")
                                    << wxStringFromUTF8((*itLink)->with->name))
                          .MakeLower();
                    if (arealink->id
                        == std::string(
                          outputStyledName.mb_str())) // this link is in the study (it
                                                      // is visible at least on layer 0)
                    {
                        foundInALayer = true;
                        break;
                    }
                }
                if (!foundInALayer)
                    layerFilteredItems.push_back(*i);
            }
        }
        search(out, tokens, layerFilteredItems);
        return;
    }

    /**
    ** The selected combo option is a layer's name
    **/
    size_t layerID = getSelectedLayerID(text);
    Spotlight::IItem::Vector layerFilteredItems;
    auto iend = items.end();
    auto i = items.begin();
    for (; i != iend; ++i)
    {
        auto& caption = (*i)->caption();

        if (caption.first() == '@' && layerID == 0)
        {
            layerFilteredItems.push_back(*i);
            continue;
        }

        ItemAreaLink::Ptr arealink = Spotlight::IItem::Ptr::DynamicCast<ItemAreaLink::Ptr>(*i);

        if (arealink && arealink->selectionType == stArea)
        {
            auto aEnd = study.areas.end();
            for (auto itArea = study.areas.begin(); itArea != aEnd; ++itArea)
            {
                if (arealink->id == itArea->first
                    && itArea->second->isVisibleOnLayer(
                      layerID)) // this area is in the study (it is visible at least on layer 0)
                {
                    layerFilteredItems.push_back(*i);
                }
            }
        }

        if (arealink && arealink->selectionType == stLink)
        {
            bool foundInALayer = false;
            auto sEnd = linkSet.end();
            for (auto itLink = linkSet.begin(); itLink != sEnd; ++itLink)
            {
                wxString outputStyledName
                  = (wxString() << wxStringFromUTF8((*itLink)->from->name) << wxT(" - ")
                                << wxStringFromUTF8((*itLink)->with->name))
                      .MakeLower();
                if (arealink->id == std::string(outputStyledName.mb_str())
                    && (*itLink)->isVisibleOnLayer(
                      layerID)) // this link is in the study (it is visible at least on layer 0)
                {
                    layerFilteredItems.push_back(*i);
                }
            }
        }
    }
    search(out, tokens, layerFilteredItems);
}

void SpotlightProviderGlobalSelection::search(Spotlight::IItem::Vector& out,
                                              const Spotlight::SearchToken::Vector& tokens,
                                              Spotlight::IItem::Vector& in)
{
    if (tokens.empty())
    {
        // There is no token. No search to perform
        out = in;

        if (not pComponent->pASelectionHasAlreadyBeenMade && not out.empty())
        {
            pComponent->pASelectionHasAlreadyBeenMade = true;
            out[0]->select();
            onSelect(out[0]);
        }
    }
    else
    {
        auto tend = tokens.end();
        auto end = in.end();
        auto i = in.begin();
        for (; i != end; ++i)
        {
            auto& caption = (*i)->caption();
            auto ti = tokens.begin();
            for (; ti != tend; ++ti)
            {
                auto& text = (*ti)->text;
                if (caption.icontains(text))
                    out.push_back(*i);
            }
        }
    }

    if (pCurrentAreaOrLink)
    {
        auto it = find(out.begin(), out.end(), pCurrentAreaOrLink);
        if (it == out.end())
        {
            pComponent->pCurrentAreaOrLink.clear();
            pComponent->pCurrentSelectionType = stNone;
            Dispatcher::GUI::Post(pComponent, &Component::updateGlobalSelection);
        }
    }
}

bool SpotlightProviderGlobalSelection::onSelect(Spotlight::IItem::Ptr& item)
{
    if (GUIIsLock() || !pComponent)
        return false;

    ItemAreaLink::Ptr arealink = Spotlight::IItem::Ptr::DynamicCast<ItemAreaLink::Ptr>(item);
    if (!(!arealink))
    {
        // Restoring the global selection
        pComponent->pCurrentSelectionType = arealink->selectionType;
        pComponent->pCurrentAreaOrLink = arealink->id;
        pCurrentAreaOrLink = item;
        Dispatcher::GUI::Post(pComponent, &Component::updateGlobalSelection);
        return true;
    }
    return false;
}

void SpotlightProviderGlobalSelection::addText(const String& name)
{
    auto* item = new Spotlight::IItem();
    item->countedAsResult(false);
    item->caption(name);
    items.push_back(item);
}

void SpotlightProviderGlobalSelection::addEconomy()
{
    auto* item = new Spotlight::IItem();
    item->countedAsResult(false);
    item->addTag(" economy ", 200, 200, 200);
    items.push_back(item);
}

void SpotlightProviderGlobalSelection::addAdequacy()
{
    auto* item = new Spotlight::IItem();
    item->countedAsResult(false);
    item->addTag(" adequacy ", 200, 200, 200);
    items.push_back(item);
}

void SpotlightProviderGlobalSelection::addNoCommonItem()
{
    auto* item = new Spotlight::IItem();
    item->countedAsResult(false);
    item->caption("No common item");
    item->group("Outputs");
    item->addTag("!!", 230, 30, 30);
    items.push_back(item);
}

void SpotlightProviderGlobalSelection::addAreaName(const String& name)
{
    appendAreaName(items, name);
}

void SpotlightProviderGlobalSelection::addSetName(const String& name)
{
    appendSetName(items, name);
}

void SpotlightProviderGlobalSelection::addLinkName(const String& name)
{
    appendLinkName(items, name);
}

void SpotlightProviderGlobalSelection::addUncommonAreaName(const String& name)
{
    appendAreaName(items, name, "Uncommon");
}

void SpotlightProviderGlobalSelection::addUncommonLinkName(const String& name)
{
    appendLinkName(items, name, "Uncommon");
}

void SpotlightProviderMCAll::search(Spotlight::IItem::Vector& out,
                                    const Spotlight::SearchToken::Vector& tokens,
                                    const Yuni::String& text)
{
    typedef Spotlight::IItem IItem;
    IItem* item;

    if (pComponent->pCurrentLOD != lodDetailedResultsWithConcatenation || tokens.empty())
    {
        item = new IItem();
        item->countedAsResult(false);
        item->caption("MC Synthesis");
        item->group("Views");
        item->image(*pBmpReattach);
        if (pComponent->pCurrentLOD == lodAllMCYears)
            item->select();
        out.push_back(item);

        if (pComponent->pHasYearByYear)
        {
            item = new IItem();
            item->caption("Year-by-year");
            item->countedAsResult(false);
            String text;
            text << pComponent->pYearsLimits[0] << " .. " << pComponent->pYearsLimits[1];
            item->addRightTag(text, 230, 230, 230);
            item->group("Views");
            item->image(*pBmpReattach);
            if (pComponent->pCurrentLOD == lodDetailledResults)
                item->select();
            out.push_back(item);
        }

        if (pComponent->pHasConcatenedDataset)
        {
            item = new IItem();
            item->caption("Variable per variable");
            item->countedAsResult(false);
            item->group("Views");
            item->image(*pBmpReattach);
            if (pComponent->pCurrentLOD == lodDetailedResultsWithConcatenation)
                item->select();
            out.push_back(item);
        }
    }
}

bool SpotlightProviderMCAll::onSelect(Spotlight::IItem::Ptr& item)
{
    if (!item || GUIIsLock() || !pComponent)
        return false;

    // TODO : add distincitve data instead of performing stupid
    // string comparisons
    if (item->group() == "Views")
    {
        if (item->caption().icontains("synthesis"))
        {
            pComponent->pCurrentLOD = lodAllMCYears;
            Dispatcher::GUI::Post(pComponent, &Component::updateGlobalSelection);
            return true;
        }
        if (item->caption().icontains("variable per variable"))
        {
            pComponent->pCurrentLOD = lodDetailedResultsWithConcatenation;
            Dispatcher::GUI::Post(pComponent, &Component::updateGlobalSelection);
            return true;
        }
        if (item->caption().icontains("year-by-year"))
        {
            pComponent->pCurrentLOD = lodDetailledResults;
            Dispatcher::GUI::Post(pComponent, &Component::updateGlobalSelection);
            return true;
        }
    }
    return true;
}

} // namespace OutputViewer
} // namespace Window
} // namespace Antares
