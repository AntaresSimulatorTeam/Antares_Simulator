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

#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/components/datagrid/renderer/correlation.h"

namespace Antares
{
namespace Window
{
class DatasourceAlphaOrder final
 : public Component::Datagrid::Renderer::CorrelationMatrix::IDatasource
{
public:
    DatasourceAlphaOrder()
    {
    }

    virtual wxString name() const
    {
        return wxT("Areas in alphabetical order");
    }

    virtual const char* icon() const
    {
        return "images/16x16/sort_alphabet.png";
    }

    virtual void reload()
    {
        pArray.clear();
        auto study = GetCurrentStudy();
        if (!(!study))
        {
            pArray.reserve(study->areas.size());
            const Data::Area::Map::iterator end = study->areas.end();
            for (Data::Area::Map::iterator i = study->areas.begin(); i != end; ++i)
                pArray.push_back(i->second);
        }
    }

    virtual uint size() const
    {
        return (uint)pArray.size();
    }

    virtual const Data::Area* at(uint i) const
    {
        return (i < pArray.size()) ? pArray[i] : NULL;
    }

    virtual uint areaIndex(uint i) const
    {
        return pArray[i]->index;
    }

public:
    Data::Area::Vector pArray;
};

class DatasourceReverseAlphaOrder
 : public Component::Datagrid::Renderer::CorrelationMatrix::IDatasource
{
public:
    DatasourceReverseAlphaOrder()
    {
    }

    virtual wxString name() const
    {
        return wxT("Areas in reverse alphabetical order");
    }

    virtual const char* icon() const
    {
        return "images/16x16/sort_alphabet_descending.png";
    }

    virtual void reload()
    {
        pArray.clear();

        auto study = GetCurrentStudy();
        if (!(!study))
        {
            pArray.reserve(study->areas.size());
            const Data::Area::Map::reverse_iterator end = study->areas.rend();
            for (Data::Area::Map::reverse_iterator i = study->areas.rbegin(); i != end; ++i)
                pArray.push_back(i->second);
        }
    }

    virtual uint size() const
    {
        return (uint)pArray.size();
    }

    virtual const Data::Area* at(uint i) const
    {
        return (i < pArray.size()) ? pArray[i] : NULL;
    }

    virtual uint areaIndex(uint i) const
    {
        return pArray[i]->index;
    }

public:
    Data::Area::Vector pArray;
};

struct SortColor
{
    inline bool operator()(const Data::Area* a, const Data::Area* b) const
    {
        // We must have strict weak ordering
        return (a->ui->cacheColorHSV == b->ui->cacheColorHSV)
                 ? (a->name < b->name)
                 : (a->ui->cacheColorHSV) > (b->ui->cacheColorHSV);
    }
};

class DatasourceColorOrder : public Component::Datagrid::Renderer::CorrelationMatrix::IDatasource
{
public:
    DatasourceColorOrder()
    {
    }

    virtual wxString name() const
    {
        return wxT("Areas ordered by theur color");
    }

    virtual const char* icon() const
    {
        return "images/16x16/color.png";
    }

    virtual void reload()
    {
        pArray.clear();

        auto study = GetCurrentStudy();
        if (!(!study))
        {
            {
                const Data::Area::Map::iterator end = study->areas.end();
                for (Data::Area::Map::iterator i = study->areas.begin(); i != end; ++i)
                    pArray.push_back(i->second);
            }
            std::sort(pArray.begin(), pArray.end(), SortColor());
            pRegionStart.resize(pArray.size(), 0);
            pRegionEnd.resize(pArray.size(), 0);
            pRegionColor.resize(pArray.size());
            pRegionColorIdentity.resize(pArray.size());

            {
                int lastLevel = 0;
                int j = 0;
                Yuni::CString<12, false> old;
                wxColour color;
                wxColour colorIdentity;
                const Data::Area::Vector::const_iterator end = pArray.end();
                for (Data::Area::Vector::const_iterator i = pArray.begin(); i != end; ++i, ++j)
                {
                    if (old != (*i)->ui->cacheColorHSV)
                    {
                        for (int z = lastLevel; z < j; ++z)
                            pRegionEnd[z] = j - 1;
                        old = (*i)->ui->cacheColorHSV;
                        color.Set(
                          (unsigned char)Yuni::Math::MinMax<int>((*i)->ui->color[0] + 35, 0, 255),
                          (unsigned char)Yuni::Math::MinMax<int>((*i)->ui->color[1] + 35, 0, 255),
                          (unsigned char)Yuni::Math::MinMax<int>((*i)->ui->color[2] + 35, 0, 255));
                        colorIdentity.Set(
                          (unsigned char)Yuni::Math::MinMax<int>((*i)->ui->color[0] - 35, 0, 255),
                          (unsigned char)Yuni::Math::MinMax<int>((*i)->ui->color[1] - 35, 0, 255),
                          (unsigned char)Yuni::Math::MinMax<int>((*i)->ui->color[2] - 35, 0, 255));
                        lastLevel = j;
                    }
                    pRegionStart[j] = lastLevel;
                    pRegionColor[j] = color;
                    pRegionColorIdentity[j] = colorIdentity;
                }
                for (int z = lastLevel; z < (int)pArray.size(); ++z)
                    pRegionEnd[z] = (int)pArray.size() - 1;
            }
        }
    }

    virtual uint size() const
    {
        return (uint)pArray.size();
    }

    virtual const Data::Area* at(uint i) const
    {
        return (i < pArray.size()) ? pArray[i] : NULL;
    }

    virtual Component::Datagrid::Renderer::IRenderer::CellStyle cellStyle(int col, int row) const
    {
        return (col == row || (row >= pRegionStart[col] && row <= pRegionEnd[col]))
                 ? Component::Datagrid::Renderer::IRenderer::cellStyleCustom
                 : Component::Datagrid::Renderer::IRenderer::cellStyleDefault;
    }

    virtual wxColour cellBackgroundColor(int col, int row) const
    {
        return (col == row) ? pRegionColorIdentity[col] : pRegionColor[col];
    }

    virtual wxColour cellTextColor(int, int) const
    {
        return wxColour(0, 0, 0);
    }

    virtual uint areaIndex(uint i) const
    {
        return pArray[i]->index;
    }

public:
    Data::Area::Vector pArray;
    std::vector<int> pRegionStart;
    std::vector<int> pRegionEnd;
    std::vector<wxColour> pRegionColor;
    std::vector<wxColour> pRegionColorIdentity;

}; // class DatasourceAlphaOrder

} // namespace Window
} // namespace Antares
