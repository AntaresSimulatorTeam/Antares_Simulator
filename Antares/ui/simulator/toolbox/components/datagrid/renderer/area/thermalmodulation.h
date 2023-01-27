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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_THERMAL_COMMON_MODULATION_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_THERMAL_COMMON_MODULATION_H__

#include <antares/wx-wrapper.h>
#include "../../../../input/thermal-cluster.h"
#include "../matrix.h"
#include <antares/study/parts/thermal/prepro.h>

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
