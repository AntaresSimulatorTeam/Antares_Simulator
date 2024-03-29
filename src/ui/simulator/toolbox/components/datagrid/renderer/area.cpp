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

#include "area.h"
#include "../../../../application/study.h"
#include "../../../../toolbox/components/refresh.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
ARendererArea::ARendererArea(wxWindow* control, Toolbox::InputSelector::Area* notifier) :
 pControl(control), pArea(nullptr)
{
    // Event: The current selected area
    if (notifier)
        notifier->onAreaChanged.connect(this, &ARendererArea::onAreaChanged);

    // Event: An area has been deletde
    OnStudyAreaDelete.connect(this, &ARendererArea::onAreaDelete);
}

ARendererArea::~ARendererArea()
{
    onRefresh.clear();
    destroyBoundEvents();
}

void ARendererArea::onAreaChanged(Data::Area* area)
{
    // FIXME
    if (area && !study)
        study = GetCurrentStudy();

    pArea = !study ? nullptr : area;
    internalAreaChanged(area);
    onRefresh();
    if (pControl)
        RefreshAllControls(pControl);
}

void ARendererArea::onAreaDelete(Data::Area* area)
{
    if (area == pArea)
    {
        pArea = nullptr;
        internalAreaChanged(nullptr);
        onRefresh();
        if (pControl)
            RefreshAllControls(pControl);
    }
}

void ARendererArea::internalAreaChanged(Data::Area*)
{
    // Do nothing
}

void ARendererArea::onStudyClosed()
{
    onAreaChanged(nullptr);
    IRenderer::onStudyClosed();
}

void ARendererArea::onStudyLoaded()
{
    IRenderer::onStudyLoaded();
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
