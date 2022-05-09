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

#include <yuni/yuni.h>
#include "gridhelper.h"
#include <typeinfo>
#include <iostream>
#include "../../../application/study.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
class GridCellAttrProvider final : public wxGridCellAttrProvider
{
public:
    GridCellAttrProvider(VGridHelper& parent) : pParent(parent)
    {
        assert(&parent != NULL);
        assert(wxIsMainThread() == true and "Must be ran from the main thread");

        // WARNING : Several instances will share pStyles and pStylesAreInitialized
        // They can not be destroyed like that
        // TODO Fixed the memory leak when exiting the program
        if (pStylesAreInitialized)
            return;

        pStylesAreInitialized = true;

        // Fonts
        const wxFont fontBold(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false);

        // Initializing Styles
        for (uint i = 0; i != (uint)(CellStyle::cellStyleMax); ++i)
        {
            pStyles[i] = new wxGridCellAttr();
            pStyles[i]->IncRef();
            pStyles[i]->SetBackgroundColour(wxColour(255, 255, 255));
            pStyles[i]->SetTextColour(wxColour(60, 60, 60));
            pStyles[i]->SetAlignment(wxALIGN_RIGHT, wxALIGN_CENTRE);
        }
        // Default alternate
        pStyles[CellStyle::cellStyleDefaultAlternate]->SetBackgroundColour(wxColour(245, 245, 255));
        // Default alternate disabled
        pStyles[CellStyle::cellStyleDefaultAlternateDisabled]->SetBackgroundColour(
          wxColour(245, 245, 255));
        pStyles[CellStyle::cellStyleDefaultAlternateDisabled]->SetTextColour(
          wxColour(200, 200, 200));
        // Default disabled
        pStyles[CellStyle::cellStyleDefaultDisabled]->SetTextColour(wxColour(200, 200, 200));

        // Default
        pStyles[CellStyle::cellStyleDefaultCenter]->SetAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
        // Default alternate
        pStyles[CellStyle::cellStyleDefaultCenterAlternate]->SetBackgroundColour(
          wxColour(245, 245, 255));
        pStyles[CellStyle::cellStyleDefaultCenterAlternate]->SetAlignment(wxALIGN_CENTRE,
                                                                          wxALIGN_CENTRE);
        // Default alternate disabled
        pStyles[CellStyle::cellStyleDefaultCenterAlternateDisabled]->SetBackgroundColour(
          wxColour(245, 245, 255));
        pStyles[CellStyle::cellStyleDefaultCenterAlternateDisabled]->SetTextColour(
          wxColour(200, 200, 200));
        pStyles[CellStyle::cellStyleDefaultCenterAlternateDisabled]->SetAlignment(wxALIGN_CENTRE,
                                                                                  wxALIGN_CENTRE);
        // Default disabled
        pStyles[CellStyle::cellStyleDefaultCenterDisabled]->SetTextColour(wxColour(200, 200, 200));
        pStyles[CellStyle::cellStyleDefaultCenterDisabled]->SetAlignment(wxALIGN_CENTRE,
                                                                         wxALIGN_CENTRE);

        // Disabled
        pStyles[CellStyle::cellStyleDisabled]->SetBackgroundColour(wxColour(245, 224, 213));
        pStyles[CellStyle::cellStyleDisabled]->SetTextColour(wxColour(50, 50, 60));
        pStyles[CellStyle::cellStyleDisabled]->SetReadOnly(true);
        // Warning
        pStyles[CellStyle::cellStyleWarning]->SetBackgroundColour(wxColour(255, 184, 62));
        pStyles[CellStyle::cellStyleWarning]->SetTextColour(wxColour(98, 47, 1));
        // Error
        pStyles[CellStyle::cellStyleError]->SetBackgroundColour(wxColour(226, 95, 95));
        pStyles[CellStyle::cellStyleError]->SetTextColour(wxColour(250, 250, 250));

        // Average
        pStyles[CellStyle::cellStyleAverage]->SetBackgroundColour(wxColour(245, 224, 213));
        pStyles[CellStyle::cellStyleAverage]->SetTextColour(wxColour(50, 50, 50));
        // MinMax
        pStyles[CellStyle::cellStyleMinMax]->SetBackgroundColour(wxColour(255, 234, 218));
        pStyles[CellStyle::cellStyleMinMax]->SetTextColour(wxColour(50, 50, 50));

        // Constraint - Enabled
        {
            pStyles[CellStyle::cellStyleConstraintEnabled]->SetBackgroundColour(
              wxColour(186, 255, 184));
            pStyles[CellStyle::cellStyleConstraintEnabled]->SetAlignment(wxALIGN_CENTRE,
                                                                         wxALIGN_CENTRE);
            pStyles[CellStyle::cellStyleConstraintEnabled]->SetTextColour(wxColour(100, 100, 100));
        }
        // Constraint - Disabled
        {
            pStyles[CellStyle::cellStyleConstraintDisabled]->SetBackgroundColour(
              wxColour(255, 179, 179));
            pStyles[CellStyle::cellStyleConstraintDisabled]->SetTextColour(wxColour(100, 100, 100));
            pStyles[CellStyle::cellStyleConstraintDisabled]->SetAlignment(wxALIGN_CENTRE,
                                                                          wxALIGN_CENTRE);
        }
        // Constraint - No Weight
        {
            pStyles[CellStyle::cellStyleConstraintNoWeight]->SetTextColour(wxColour(200, 200, 200));
            pStyles[CellStyle::cellStyleConstraintNoWeight]->SetAlignment(wxALIGN_CENTRE,
                                                                          wxALIGN_CENTRE);
        }
        // Constraint - Weight
        {
            pStyles[CellStyle::cellStyleConstraintWeight]->SetTextColour(wxColour(255, 100, 43));
            pStyles[CellStyle::cellStyleConstraintWeight]->SetAlignment(wxALIGN_CENTRE,
                                                                        wxALIGN_CENTRE);
        }
        // Constraint - Weight count
        {
            pStyles[CellStyle::cellStyleConstraintWeightCount]->SetBackgroundColour(
              wxColour(230, 230, 230));
            pStyles[CellStyle::cellStyleConstraintWeightCount]->SetTextColour(
              wxColour(170, 170, 220));
            pStyles[CellStyle::cellStyleConstraintWeightCount]->SetRenderer(
              new wxGridCellNumberRenderer());
        }
        // Constraint - Operator
        {
            pStyles[CellStyle::cellStyleConstraintOperator]->SetBackgroundColour(
              wxColour(230, 230, 230));
            pStyles[CellStyle::cellStyleConstraintOperator]->SetTextColour(wxColour(50, 50, 50));
            pStyles[CellStyle::cellStyleConstraintOperator]->SetFont(fontBold);
        }
        // Constraint - Type
        {
            pStyles[CellStyle::cellStyleConstraintType]->SetBackgroundColour(
              wxColour(230, 230, 230));
            pStyles[CellStyle::cellStyleConstraintType]->SetTextColour(wxColour(50, 50, 50));
            pStyles[CellStyle::cellStyleConstraintType]->SetFont(fontBold);
        }
        // Layer - Disabled visibility field
        {
            pStyles[CellStyle::cellStyleLayerDisabled]->SetTextColour(wxColour(200, 200, 200));
            pStyles[CellStyle::cellStyleLayerDisabled]->SetAlignment(wxALIGN_CENTRE,
                                                                     wxALIGN_CENTRE);
            pStyles[CellStyle::cellStyleLayerDisabled]->SetReadOnly(true);
        }
        // Geographic trimming
        {
            pStyles[CellStyle::cellStyleFilterYearByYearOn]->SetAlignment(wxALIGN_CENTRE,
                                                                          wxALIGN_CENTRE);
            pStyles[CellStyle::cellStyleFilterYearByYearOn]->SetTextColour(wxColour(0, 128, 0));

            pStyles[CellStyle::cellStyleFilterYearByYearOff]->SetAlignment(wxALIGN_CENTRE,
                                                                           wxALIGN_CENTRE);
            pStyles[CellStyle::cellStyleFilterYearByYearOff]->SetTextColour(wxColour(204, 51, 0));

            pStyles[CellStyle::cellStyleFilterSynthesisOn]->SetBackgroundColour(
              wxColour(221, 221, 221));
            pStyles[CellStyle::cellStyleFilterSynthesisOn]->SetAlignment(wxALIGN_CENTRE,
                                                                         wxALIGN_CENTRE);
            pStyles[CellStyle::cellStyleFilterSynthesisOn]->SetTextColour(wxColour(0, 128, 0));

            pStyles[CellStyle::cellStyleFilterSynthesisOff]->SetBackgroundColour(
              wxColour(221, 221, 221));
            pStyles[CellStyle::cellStyleFilterSynthesisOff]->SetAlignment(wxALIGN_CENTRE,
                                                                          wxALIGN_CENTRE);
            pStyles[CellStyle::cellStyleFilterSynthesisOff]->SetTextColour(wxColour(204, 51, 0));

            // Undefined geographic trimming
            pStyles[CellStyle::cellStyleFilterUndefined]->SetBackgroundColour(
              wxColour(255, 155, 0));
            pStyles[CellStyle::cellStyleFilterUndefined]->SetAlignment(wxALIGN_CENTRE,
                                                                       wxALIGN_CENTRE);
        }
        // Adequacy patch areas
        {
            pStyles[CellStyle::cellStyleAdqPatchVirtual]->SetAlignment(wxALIGN_CENTRE,
                                                                       wxALIGN_CENTRE);
            pStyles[CellStyle::cellStyleAdqPatchVirtual]->SetTextColour(wxColour(0, 0, 0));

            pStyles[CellStyle::cellStyleAdqPatchOutside]->SetAlignment(wxALIGN_CENTRE,
                                                                       wxALIGN_CENTRE);
            pStyles[CellStyle::cellStyleAdqPatchOutside]->SetTextColour(wxColour(204, 51, 0));

            pStyles[CellStyle::cellStyleAdqPatchInside]->SetAlignment(wxALIGN_CENTRE,
                                                                      wxALIGN_CENTRE);
            pStyles[CellStyle::cellStyleAdqPatchInside]->SetTextColour(wxColour(0, 128, 0));
        }
    }

    virtual ~GridCellAttrProvider()
    {
        // See comment in the constructor
        // Releasing all cell attr
        /*for (uint i = 0; i != (uint)CellStyle::cellStyleMax; ++i)
        {
                if (pStyles)
                {
                        pStyles[i]->DecRef();
                        pStyles[i] = nullptr;
                }
        }
        pStylesAreInitialized = false;*/
    }

    virtual wxGridCellAttr* GetAttr(int row, int col, wxGridCellAttr::wxAttrKind) const override
    {
        assert(this and "Invalid THIS");
        assert(pStylesAreInitialized and "Styles are required");
        col = col == -1 ? 0 : col; //! PN Bug sur les filtres
        if (!pStylesAreInitialized)
        {
            // WAHT ?
            wxGridCellAttr* attr = new wxGridCellAttr();
            attr->IncRef();
            return attr;
        }

        wxGridCellAttr* attr = nullptr;
        Renderer::IRenderer* renderer = pParent.renderer();
        if (!renderer)
        {
            attr = pStyles[Renderer::IRenderer::cellStyleDefault];
        }
        else
        {
            // Retrieving the style of the given cell
            const Renderer::IRenderer::CellStyle style
              = renderer->cellStyle(pParent.realCol(col), pParent.realRow(row));

            if (style == Renderer::IRenderer::cellStyleCustom)
            {
                attr = new wxGridCellAttr();
                int x = pParent.realCol(col);
                int y = pParent.realRow(row);
                attr->SetBackgroundColour(renderer->cellBackgroundColor(x, y));
                attr->SetTextColour(renderer->cellTextColor(x, y));
                int align = renderer->cellAlignment(x, y);
                if (!align)
                    attr->SetAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
                else
                {
                    if (align < 0)
                        attr->SetAlignment(wxALIGN_LEFT, wxALIGN_CENTRE);
                    else
                        attr->SetAlignment(wxALIGN_RIGHT, wxALIGN_CENTRE);
                }
            }
            else
                attr = pStyles[(uint)style];
        }

        assert(attr != NULL and "Invalid cell attribute");
        attr->IncRef();
        return attr;
    }

    void appendCSSForAllCellStyles(wxString& s)
    {
        /* Header */
        s << wxT("table {border:1px solid black;}\r\n");
        s << wxT("th {white-space:nowrap;font-weight:bold;text-align:center;border:1px solid "
                 "black;background-color:#222222;color:white;}\r\n");
        s << wxT("td {border:1px solid #777777;white-space:nowrap;}\r\n");
        s << wxT(".ukn {}\r\n");

        // Styles
        internalAppendCSSCode(s, CellStyle::cellStyleDefault);
        internalAppendCSSCode(s, CellStyle::cellStyleDefaultAlternate);
        internalAppendCSSCode(s, CellStyle::cellStyleDefaultDisabled);
        internalAppendCSSCode(s, CellStyle::cellStyleDefaultAlternateDisabled);
        internalAppendCSSCode(s, CellStyle::cellStyleDefaultCenter);
        internalAppendCSSCode(s, CellStyle::cellStyleDefaultCenterAlternate);
        internalAppendCSSCode(s, CellStyle::cellStyleDefaultCenterDisabled);
        internalAppendCSSCode(s, CellStyle::cellStyleDefaultCenterAlternateDisabled);
        internalAppendCSSCode(s, CellStyle::cellStyleWarning);
        internalAppendCSSCode(s, CellStyle::cellStyleError);
        internalAppendCSSCode(s, CellStyle::cellStyleDisabled);
        internalAppendCSSCode(s, CellStyle::cellStyleAverage);
        internalAppendCSSCode(s, CellStyle::cellStyleSum);
        internalAppendCSSCode(s, CellStyle::cellStyleMinMax);
        internalAppendCSSCode(s, CellStyle::cellStyleConstraintEnabled);
        internalAppendCSSCode(s, CellStyle::cellStyleConstraintDisabled);
        internalAppendCSSCode(s, CellStyle::cellStyleConstraintWeight);
        internalAppendCSSCode(s, CellStyle::cellStyleConstraintNoWeight);
        internalAppendCSSCode(s, CellStyle::cellStyleConstraintWeightCount);
        internalAppendCSSCode(s, CellStyle::cellStyleFilterYearByYearOn);
        internalAppendCSSCode(s, CellStyle::cellStyleFilterYearByYearOff);
        internalAppendCSSCode(s, CellStyle::cellStyleFilterSynthesisOn);
        internalAppendCSSCode(s, CellStyle::cellStyleFilterSynthesisOff);
        internalAppendCSSCode(s, CellStyle::cellStyleFilterUndefined);
        internalAppendCSSCode(s, CellStyle::cellStyleAdqPatchVirtual);
        internalAppendCSSCode(s, CellStyle::cellStyleAdqPatchOutside);
        internalAppendCSSCode(s, CellStyle::cellStyleAdqPatchInside);
    }

    void appendCSSForAllCellStyles2(Yuni::String& s)
    {
        // Header
        s << "table {border:1px solid black;}\r\n";
        s << "th {white-space:nowrap;font-weight:bold;text-align:center;border:1px solid "
             "black;background-color:#222222;color:white;}\r\n";
        s << "td {border:1px solid #777777;white-space:nowrap;}\r\n";
        s << ".ukn {}\r\n";

        // Styles
        internalAppendCSSCode(s, CellStyle::cellStyleDefault);
        internalAppendCSSCode(s, CellStyle::cellStyleDefaultAlternate);
        internalAppendCSSCode(s, CellStyle::cellStyleDefaultDisabled);
        internalAppendCSSCode(s, CellStyle::cellStyleDefaultAlternateDisabled);
        internalAppendCSSCode(s, CellStyle::cellStyleDefaultCenter);
        internalAppendCSSCode(s, CellStyle::cellStyleDefaultCenterAlternate);
        internalAppendCSSCode(s, CellStyle::cellStyleDefaultCenterDisabled);
        internalAppendCSSCode(s, CellStyle::cellStyleDefaultCenterAlternateDisabled);
        internalAppendCSSCode(s, CellStyle::cellStyleWarning);
        internalAppendCSSCode(s, CellStyle::cellStyleError);
        internalAppendCSSCode(s, CellStyle::cellStyleDisabled);
        internalAppendCSSCode(s, CellStyle::cellStyleAverage);
        internalAppendCSSCode(s, CellStyle::cellStyleSum);
        internalAppendCSSCode(s, CellStyle::cellStyleMinMax);
        internalAppendCSSCode(s, CellStyle::cellStyleConstraintEnabled);
        internalAppendCSSCode(s, CellStyle::cellStyleConstraintDisabled);
        internalAppendCSSCode(s, CellStyle::cellStyleConstraintWeight);
        internalAppendCSSCode(s, CellStyle::cellStyleConstraintNoWeight);
        internalAppendCSSCode(s, CellStyle::cellStyleConstraintWeightCount);
        internalAppendCSSCode(s, CellStyle::cellStyleFilterYearByYearOn);
        internalAppendCSSCode(s, CellStyle::cellStyleFilterYearByYearOff);
        internalAppendCSSCode(s, CellStyle::cellStyleFilterSynthesisOn);
        internalAppendCSSCode(s, CellStyle::cellStyleFilterSynthesisOff);
        internalAppendCSSCode(s, CellStyle::cellStyleFilterUndefined);
         internalAppendCSSCode(s, CellStyle::cellStyleAdqPatchVirtual);
        internalAppendCSSCode(s, CellStyle::cellStyleAdqPatchOutside);
        internalAppendCSSCode(s, CellStyle::cellStyleAdqPatchInside);
    }

private:
    typedef Renderer::IRenderer CellStyle;

private:
    void internalAppendCSSCode(wxString& s, CellStyle::CellStyle style)
    {
        const wxColour b = pStyles[style]->GetBackgroundColour();
        s << wxT('.') << Renderer::IRenderer::CellStyleToCSSClass(style)
          << wxT(" {background-color:rgb(") << (int)b.Red() << wxT(',') << (int)b.Green()
          << wxT(',') << (int)b.Blue() << wxT(");color:rgb(")
          << (int)pStyles[style]->GetTextColour().Red() << wxT(',')
          << (int)pStyles[style]->GetTextColour().Green() << wxT(',')
          << (int)pStyles[style]->GetTextColour().Blue() << wxT(");");
        int valign, halign;
        pStyles[style]->GetAlignment(&halign, &valign);
        switch (halign)
        {
        case wxALIGN_CENTER:
            s << wxT("text-align:center;");
            break;
        case wxALIGN_RIGHT:
            s << wxT("text-align:right;");
            break;
        }
        s << wxT("}\r\n");
    }

    void internalAppendCSSCode(Yuni::String& s, CellStyle::CellStyle style)
    {
        const wxColour b = pStyles[style]->GetBackgroundColour();
        s << '.';
        Renderer::IRenderer::CellStyleToCSSClass(style, s);
        s << " {background-color:rgb(" << (int)b.Red() << ',' << (int)b.Green() << ','
          << (int)b.Blue() << ");color:rgb(" << (int)pStyles[style]->GetTextColour().Red() << ','
          << (int)pStyles[style]->GetTextColour().Green() << ','
          << (int)pStyles[style]->GetTextColour().Blue() << ");}\r\n";
        int valign, halign;
        pStyles[style]->GetAlignment(&halign, &valign);
        switch (halign)
        {
        case wxALIGN_CENTER:
            s << "text-align:center;";
            break;
        case wxALIGN_RIGHT:
            s << "text-align:right;";
            break;
        }
        s << "}\r\n";
    }

private:
    // Friend
    friend class VGridHelper;

    //! Parent
    VGridHelper& pParent;
    //! All styles
    static wxGridCellAttr* pStyles[CellStyle::cellStyleMax];
    //! Global flag to initialize only once all styles
    static bool pStylesAreInitialized;

}; // class GridCellAttrProvider

wxGridCellAttr* GridCellAttrProvider::pStyles[CellStyle::cellStyleMax];
bool GridCellAttrProvider::pStylesAreInitialized = false;

static inline bool IsNumeric(const AnyString& text, bool& hasDot)
{
    hasDot = false;
    bool canHaveMinus = true;
    for (uint i = 0; i != text.size(); ++i)
    {
        const char c = text[i];
        if (c >= '0' and c <= '9')
        {
            // ok
            canHaveMinus = false;
            continue;
        }
        if (c == '.' or c == ',')
        {
            if (hasDot)
                return false;
            hasDot = true;
            canHaveMinus = false;
            continue;
        }
        if (c == '-')
        {
            if (!canHaveMinus)
                return false;
            canHaveMinus = false;
            continue;
        }
        return false;
    }
    return true;
}

VGridHelper::VGridHelper(Renderer::IRenderer* renderer, bool markModified) :
 virtualSize(0, 0),
 valid(true),
 pRenderer(renderer),
 pDataGridPrecision(Date::stepNone),
 pMarkStudyModifiedWhenModifyingCell(markModified)
{
    // Indices
    resetIndicesToDefault();
    // Attr provider...
    SetAttrProvider(new GridCellAttrProvider(*this));
}

void VGridHelper::resetIndicesToDefault()
{
    assert(this);
    if (valid and pRenderer)
    {
        virtualSize.x = pRenderer->width();
        virtualSize.y = pRenderer->height();

        if (virtualSize.x and virtualSize.y)
        {
            assert((uint)virtualSize.x < 2000000 and "The virtual size seems a bit excessive");
            assert((uint)virtualSize.y < 2000000 and "The virtual size seems a bit excessive");
            indicesRows.resize(virtualSize.y);
            indicesCols.resize(virtualSize.x);

            for (int i = 0; i != virtualSize.x; ++i)
                indicesCols[i] = i;
            for (int i = 0; i != virtualSize.y; ++i)
                indicesRows[i] = i;
            return;
        }
    }
    // reset
    virtualSize.x = 0;
    virtualSize.y = 0;
}

void VGridHelper::resetIndicesToDefaultWithoutInit()
{
    assert(this);
    if (valid and pRenderer)
    {
        assert((uint)virtualSize.x < 2000000 and "The virtual size seems a bit excessive");
        assert((uint)virtualSize.y < 2000000 and "The virtual size seems a bit excessive");
        virtualSize.x = pRenderer->width();
        virtualSize.y = pRenderer->height();
        indicesCols.resize(virtualSize.x);
        indicesRows.resize(virtualSize.y);
    }
    else
    {
        virtualSize.x = 0;
        virtualSize.y = 0;
        indicesCols.clear();
        indicesRows.clear();
    }
}

wxString VGridHelper::GetRowLabelValue(int row)
{
    if (pRenderer and valid)
        return pRenderer->rowCaption(realRow(row));
    return wxEmptyString;
}

wxString VGridHelper::GetColLabelValue(int col)
{
    return (pRenderer and valid) ? pRenderer->columnCaption(realCol(col)) : wxString();
}

wxString VGridHelper::GetValue(int row, int col)
{
    if (pRenderer and valid)
        return (pRenderer->cellValue(realCol(col), realRow(row)) << wxT(' '));
    return wxEmptyString;
}

double VGridHelper::GetNumericValue(int row, int col) const
{
    return (pRenderer and valid) ? pRenderer->cellNumericValue(realCol(col), realRow(row)) : 0.;
}

void VGridHelper::SetValue(int row, int col, const wxString& v)
{
    assert(this);
    if (pRenderer and valid and row >= 0 and col >= 0)
    {
        String value;
        wxStringToString(v, value);
        value.trim(" \t\r\n");
        bool hasDot;
        if (IsNumeric(value, hasDot))
        {
            if (hasDot)
            {
                value.replace(',', '.');
                value += '0';
            }
        }
        if (pRenderer->cellValue(realCol(col), realRow(row), value))
        {
            if (pMarkStudyModifiedWhenModifyingCell)
                MarkTheStudyAsModified();
        }
    }
}

void VGridHelper::SetValue(int row, int col, const Yuni::String& v)
{
    assert(this);
    if (pRenderer and valid)
    {
        String value = v;
        value.trim(" \t\r\n");
        bool hasDot;
        if (IsNumeric(value, hasDot))
        {
            if (hasDot)
            {
                value.replace(',', '.');
                value += '0';
            }
        }
        if (pRenderer->cellValue(realCol(col), realRow(row), value))
            MarkTheStudyAsModified();
    }
}

void VGridHelper::precision(const Date::Precision p)
{
    pDataGridPrecision = p;
    if (pRenderer)
        pRenderer->dataGridPrecision = p;
}

wxColour VGridHelper::foregroundColorFromCellStyle(Renderer::IRenderer::CellStyle s) const
{
    GridCellAttrProvider* attr = dynamic_cast<GridCellAttrProvider*>(GetAttrProvider());
    if (attr)
        return attr->pStyles[s]->GetTextColour();
    return wxColour();
}

wxColour VGridHelper::backgroundColorFromCellStyle(Renderer::IRenderer::CellStyle s) const
{
    GridCellAttrProvider* attr = dynamic_cast<GridCellAttrProvider*>(GetAttrProvider());
    if (attr)
        return attr->pStyles[s]->GetBackgroundColour();
    return wxColour();
}

void VGridHelper::appendCSSForAllCellStyles(wxString& s)
{
    dynamic_cast<GridCellAttrProvider*>(GetAttrProvider())->appendCSSForAllCellStyles(s);
}

void VGridHelper::appendCSSForAllCellStyles2(Yuni::String& s)
{
    dynamic_cast<GridCellAttrProvider*>(GetAttrProvider())->appendCSSForAllCellStyles2(s);
}

bool VGridHelper::markTheStudyAsModified() const
{
    return pMarkStudyModifiedWhenModifyingCell;
}

} // namespace Datagrid
} // namespace Component
} // namespace Antares
