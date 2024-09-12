/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
#pragma once

#include "scenario-builder-renderer-base.h"
#include "../../../input/connection.h"

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
class ntcScBuilderRenderer : public ScBuilderRendererBase
{
public:
    ntcScBuilderRenderer();

    ntcScBuilderRenderer& operator=(const ntcScBuilderRenderer&) = delete;
    ntcScBuilderRenderer(const ntcScBuilderRenderer&) = delete;

    bool valid() const override;
    int height() const override;
    wxString rowCaption(int rowIndx) const override;

    bool cellValue(int x, int y, const Yuni::String& value) override;
    double cellNumericValue(int x, int y) const override;

private:
    void onStudyChanged(Data::Study& study);
    Data::AreaLink::Vector* pListOfLinks = nullptr;
}; // class ntcScBuilderRenderer

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
