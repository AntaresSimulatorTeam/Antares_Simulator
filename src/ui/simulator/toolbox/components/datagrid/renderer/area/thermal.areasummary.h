// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_THERMAL_SUMMARY_SINGLE_AREA_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_THERMAL_SUMMARY_SINGLE_AREA_H__

#include "common.areasummary.h"

namespace Antares::Component::Datagrid::Renderer
{
class ThermalClusterSummarySingleArea: public CommonClusterSummarySingleArea
{
public:
    ThermalClusterSummarySingleArea(wxWindow* control, Toolbox::InputSelector::Area* notifier);

    virtual ~ThermalClusterSummarySingleArea();

    virtual int width() const override
    {
        return 14;
    }

    virtual int height() const override
    {
        return (pArea) ? pArea->thermal.list.allClustersCount() : 0;
    }

    virtual wxString columnCaption(int colIndx) const override;

    virtual wxString rowCaption(int rowIndx) const override;

    virtual wxString cellValue(int x, int y) const override;

    virtual double cellNumericValue(int x, int y) const override;

    virtual bool cellValue(int x, int y, const Yuni::String& v) override;

}; // class ThermalClusterSummarySingleArea

} // namespace Antares::Component::Datagrid::Renderer

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_THERMAL_SUMMARY_SINGLE_AREA_H__
