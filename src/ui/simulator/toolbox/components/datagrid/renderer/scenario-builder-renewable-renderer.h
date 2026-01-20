// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_RENEWABLE_SCENARIO_BUILDER_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_RENEWABLE_SCENARIO_BUILDER_H__

#include "scenario-builder-renderer-base.h"

namespace Antares::Component::Datagrid::Renderer
{
class renewableScBuilderRenderer: public ScBuilderRendererForAreaSelector
{
public:
    using ScBuilderRendererForAreaSelector::ScBuilderRendererForAreaSelector;
    ~renewableScBuilderRenderer() override = default;

    bool cellValue(int x, int y, const Yuni::String& value) override;
    int height() const override;
    wxString rowCaption(int rowIndx) const override;
    double cellNumericValue(int x, int y) const override;
}; // class renewableScBuilderRenderer

} // namespace Antares::Component::Datagrid::Renderer

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_RENEWABLE_SCENARIO_BUILDER_H__

#pragma once
