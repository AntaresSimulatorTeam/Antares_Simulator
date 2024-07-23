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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_RENEWABLE_SUMMARY_SINGLE_AREA_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_RENEWABLE_SUMMARY_SINGLE_AREA_H__

#include "common.areasummary.h"

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
class RenewableClusterSummarySingleArea : public CommonClusterSummarySingleArea
{
public:
    RenewableClusterSummarySingleArea(wxWindow* control, Toolbox::InputSelector::Area* notifier);

    virtual ~RenewableClusterSummarySingleArea();

    virtual int width() const
    {
        return 4;
    }
    virtual int height() const
    {
        return (pArea) ? pArea->renewable.list.allClustersCount() : 0;
    }

    virtual wxString columnCaption(int colIndx) const override;

    virtual wxString rowCaption(int rowIndx) const override;

    virtual wxString cellValue(int x, int y) const override;

    virtual double cellNumericValue(int x, int y) const override;

    virtual bool cellValue(int x, int y, const Yuni::String& v) override;

    /*
    virtual void resetColors(int, int, wxColour&, wxColour&) const
    {
        // Do nothing
    }

    virtual IRenderer::CellStyle cellStyle(int col, int row) const;

    virtual uint maxWidthResize() const
    {
        return 0;
    }
    virtual uint maxHeightResize() const
    {
        return 0;
    }

    virtual bool valid() const
    {
        return (pArea != NULL);
    }
    */

    /*
    protected:
        virtual void onAreaChanged(Antares::Data::Area* area);
        virtual void onStudyClosed() override;
        void onStudyAreaDelete(Antares::Data::Area* area);

    private:
        Antares::Data::Area* pArea;
        wxWindow* pControl;
        Toolbox::InputSelector::Area* pAreaNotifier;
    */
}; // class RenewableClusterSummarySingleArea

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_RENEWABLE_SUMMARY_SINGLE_AREA_H__
