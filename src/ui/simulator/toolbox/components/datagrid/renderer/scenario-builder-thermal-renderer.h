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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_THERMAL_SCENARIO_BUILDER_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_THERMAL_SCENARIO_BUILDER_H__

#include "scenario-builder-renderer-base.h"

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
class thermalScBuilderRenderer : public ScBuilderRendererForAreaSelector
{
public:
    using ScBuilderRendererForAreaSelector::ScBuilderRendererForAreaSelector;
    ~thermalScBuilderRenderer() override = default;

    bool cellValue(int x, int y, const Yuni::String& value) override;
    int height() const override;
    wxString rowCaption(int rowIndx) const override;
    double cellNumericValue(int x, int y) const override;
}; // class thermalScBuilderRenderer

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_THERMAL_SCENARIO_BUILDER_H__

#pragma once
