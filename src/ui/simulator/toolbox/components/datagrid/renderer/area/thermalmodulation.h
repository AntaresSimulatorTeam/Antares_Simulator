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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_THERMAL_COMMON_MODULATION_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_THERMAL_COMMON_MODULATION_H__

#include "../../../../input/thermal-cluster.h"
#include "../matrix.h"
#include <antares/solver/ts-generator/prepro.h>

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
class ThermalClusterCommonModulation : public Renderer::Matrix<>
{
public:
    using AncestorType = Renderer::Matrix<>;

public:
    ThermalClusterCommonModulation(wxWindow* control,
                                   Toolbox::InputSelector::ThermalCluster* notifier);

    virtual ~ThermalClusterCommonModulation();

    virtual int width() const
    {
        return AncestorType::width() + 4;
    }

    virtual int height() const
    {
        return AncestorType::height();
    }

    virtual wxString columnCaption(int column) const;

    virtual wxString rowCaption(int row) const;

    virtual wxString cellValue(int x, int y) const;

    virtual double cellNumericValue(int x, int y) const;

    virtual bool cellValue(int x, int y, const Yuni::String& v);

    virtual void resetColors(int, int, wxColour&, wxColour&) const
    { /*Do nothing*/
    }

    virtual wxColour verticalBorderColor(int x, int y) const;
    virtual wxColour horizontalBorderColor(int x, int y) const;

    virtual IRenderer::CellStyle cellStyle(int col, int row) const;

    virtual uint maxWidthResize() const
    {
        return 0;
    }
    virtual uint maxHeightResize() const
    {
        return 0;
    }

    virtual Date::Precision precision()
    {
        return Date::hourly;
    }

protected:
    virtual void internalThermalClusterChanged(Antares::Data::ThermalCluster* cluster);
    virtual void onStudyClosed();

private:
    Antares::Data::ThermalCluster* pCluster;

}; // class ThermalClusterPrepro

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_THERMAL_COMMON_MODULATION_H__
