// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "links.h"
#include "../../../../../application/study.h"
#include "../../component.h"
#include <antares/study/study.h>
#include <wx/wx.h>

using namespace Yuni;

namespace Antares::Component::Datagrid::Renderer::ConstraintsBuilder
{
void Links::initializeFromStudy()
{
    if (!(!study))
    {
        pRecord.mutex.lock();

        uint indx = 0;
        uint countLinks = 0; // to do : create function study->getNLinks();
        const Data::Area::Map::iterator end = study->areas.end();
        for (Data::Area::Map::iterator i = study->areas.begin(); i != end; ++i)
        {
            Data::Area& area = *(i->second);
            const Data::AreaLink::Map::iterator end = area.links.end();
            for (Data::AreaLink::Map::iterator i = area.links.begin(); i != end; ++i)
            {
                ++countLinks;
            }
        }
        pRecord.array.resize(countLinks);

        for (Data::Area::Map::iterator i = study->areas.begin(); i != end; ++i)
        {
            // Reference to the area
            Data::Area& area1 = *(i->second);

            // For each Interconnection for the area
            const Data::AreaLink::Map::iterator end = area1.links.end();
            for (Data::AreaLink::Map::iterator i = area1.links.begin(); i != end; ++i)
            {
                Data::AreaLink* lnk = i->second;

                assert(indx < pRecord.size());
                // assert(area.load.prepro);

                using Record = ConstraintsBuilder::Links::Record;

                Record& record = pRecord.array[indx];
                record.enabled = true;
                record.areaFromId = lnk->from->id;
                record.areaWithId = lnk->with->id;

                record.wxLinkName = wxStringFromUTF8("");
                record.wxLinkName << wxStringFromUTF8(lnk->from->name) + wxStringFromUTF8(" / ")
                                       + wxStringFromUTF8(lnk->with->name);
                // record.wxLinkName   << wxStringFromUTF8(lnk->with->name);

                record.type = Antares::Data::atAC;
                record.mWidth = (uint)-1;
                record.mHeight = (uint)-1;
                ++indx;
            }
        }
    }

    pRecord.mutex.unlock();
}

Links::~Links()
{
    destroyBoundEvents();
}

wxString Links::columnCaption(int colIndx) const
{
    static const wxChar* const names[] = {/*wxT("Cost"),*/
                                          wxT("Minimum spanning tree"),
                                          wxT("Minimum cycle Basis")};
    if (colIndx < 2)
    {
        return names[colIndx];
    }
    return wxEmptyString;
}

wxString Links::rowCaption(int rowIndx) const
{
    if ((uint)rowIndx < pCBuilder->linkCount())
    {
        return pCBuilder->getLink(rowIndx)->getName().to<std::string>();
    }
    return wxEmptyString;
}

IRenderer::CellStyle Links::cellStyle(int col, int row) const
{
    auto record = pCBuilder->getLink(row);

    if (!record->enabled)
    {
        return IRenderer::cellStyleDefaultDisabled;
    }
    else if (record->type != Antares::Data::atAC && record->type != Antares::Data::atDC)
    {
        if (col == 2 || col == 3)
        {
            return IRenderer::cellStyleDefaultDisabled;
        }
    }

    return IRenderer::cellStyleDefault;
}

wxColour Links::cellBackgroundColor(int, int) const
{
    return wxColour();
}

wxColour Links::cellTextColor(int, int) const
{
    return wxColour();
}

wxString Links::cellValue(int x, int y) const
{
    if (!pCBuilder->getUpToDate())
    {
        return "?";
    }

    auto record = pCBuilder->getLink(y);

    switch (x)
    {
    /*case 0:
            {
                    const double d = record->getWeightWithImpedance();;
                    return DoubleToWxString(d);
            }*/
    case 0:
        return pCBuilder->isCycleDriver(record) ? "0" : "1";

    case 1:
    {
        const uint i = pCBuilder->cycleCount(record);
        return wxString::Format(wxT("%u"), i);
    }
    case 2:
    {
        const double d = cellNumericValue(x, y);
        return DoubleToWxString(d);
    }
    }
    return wxEmptyString;
}

double Links::cellNumericValue(int x, int y) const
{
    if (!pCBuilder->getUpToDate())
    {
        return -1;
    }
    auto record = pCBuilder->getLink(y);

    switch (x)
    {
    case 0:
    {
        return record->getWeightWithImpedance();
    }
    case 1:
        return pCBuilder->isCycleDriver(record) ? 0 : 1;

    case 2:
    {
        return pCBuilder->cycleCount(record);
    }
    case 3:
    {
        return cellNumericValue(x, y);
    }
    }
    return 0.;
}

bool Links::cellValue(int /*x*/, int /*y*/, const String& /*value*/)
{
    return false;
}

bool Links::valid() const
{
    return pCBuilder != nullptr;
}

} // namespace Antares::Component::Datagrid::Renderer::ConstraintsBuilder
