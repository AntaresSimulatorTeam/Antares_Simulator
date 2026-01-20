// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "area.h"
#include "../../../../application/study.h"
#include "../../../../toolbox/components/refresh.h"

using namespace Yuni;

namespace Antares::Component::Datagrid::Renderer
{
ARendererArea::ARendererArea(wxWindow* control, Toolbox::InputSelector::Area* notifier):
    pControl(control),
    pArea(nullptr)
{
    // Event: The current selected area
    if (notifier)
    {
        notifier->onAreaChanged.connect(this, &ARendererArea::onAreaChanged);
    }

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
    {
        study = GetCurrentStudy();
    }

    pArea = !study ? nullptr : area;
    internalAreaChanged(area);
    onRefresh();
    if (pControl)
    {
        RefreshAllControls(pControl);
    }
}

void ARendererArea::onAreaDelete(Data::Area* area)
{
    if (area == pArea)
    {
        pArea = nullptr;
        internalAreaChanged(nullptr);
        onRefresh();
        if (pControl)
        {
            RefreshAllControls(pControl);
        }
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

} // namespace Antares::Component::Datagrid::Renderer
