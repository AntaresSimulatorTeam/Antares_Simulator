/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_THERMAL_SUMMARY_SINGLE_AREA_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_THERMAL_SUMMARY_SINGLE_AREA_H__

#include "common.areasummary.h"

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
class ThermalClusterSummarySingleArea : public CommonClusterSummarySingleArea
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
        return (pArea) ? pArea->thermal.list.size() : 0;
    }

    virtual wxString columnCaption(int colIndx) const override;

    virtual wxString rowCaption(int rowIndx) const override;

    virtual wxString cellValue(int x, int y) const override;

    virtual double cellNumericValue(int x, int y) const override;

    virtual bool cellValue(int x, int y, const Yuni::String& v) override;

}; // class ThermalClusterSummarySingleArea

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_THERMAL_SUMMARY_SINGLE_AREA_H__
