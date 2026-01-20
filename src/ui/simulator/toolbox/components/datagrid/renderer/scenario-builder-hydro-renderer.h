// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_HYDRO_SCENARIO_BUILDER_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_HYDRO_SCENARIO_BUILDER_H__

#include "scenario-builder-renderer-base.h"

namespace Antares::Component::Datagrid::Renderer
{
class hydroScBuilderRenderer: public ScBuilderRendererAreasAsRows
{
public:
    hydroScBuilderRenderer() = default;

    bool cellValue(int x, int y, const Yuni::String& value);
    double cellNumericValue(int x, int y) const;
}; // class hydroScBuilderRenderer
} // namespace Antares::Component::Datagrid::Renderer

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_HYDRO_SCENARIO_BUILDER_H__
