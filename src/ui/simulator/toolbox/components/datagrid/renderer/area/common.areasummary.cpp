// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "common.areasummary.h"
#include "toolbox/components/refresh.h"

namespace Antares::Component::Datagrid::Renderer
{
CommonClusterSummarySingleArea::CommonClusterSummarySingleArea(
  wxWindow* control,
  Toolbox::InputSelector::Area* notifier):
    pArea(nullptr),
    pControl(control),
    pAreaNotifier(notifier)
{
    if (notifier)
    {
        notifier->onAreaChanged.connect(this, &CommonClusterSummarySingleArea::onAreaChanged);
    }
}

CommonClusterSummarySingleArea::~CommonClusterSummarySingleArea()
{
    destroyBoundEvents();
}

void CommonClusterSummarySingleArea::onAreaChanged(Antares::Data::Area* area)
{
    if (pArea != area)
    {
        pArea = area;
        RefreshAllControls(pControl);
    }
}

IRenderer::CellStyle CommonClusterSummarySingleArea::cellStyle(int col, int row) const
{
    if (col > 0 and Math::Zero(cellNumericValue(col, row)))
    {
        return IRenderer::cellStyleDefaultDisabled;
    }
    else
    {
        if (col == 1 || col == 2)
        {
            return IRenderer::cellStyleConstraintWeight;
        }
        else
        {
            return IRenderer::cellStyleDefault;
        }
    }
}

void CommonClusterSummarySingleArea::onStudyClosed()
{
    pArea = nullptr;
    IRenderer::onStudyClosed();
}

void CommonClusterSummarySingleArea::onStudyAreaDelete(Antares::Data::Area* area)
{
    if (pArea == area)
    {
        onAreaChanged(nullptr);
    }
}

} // namespace Antares::Component::Datagrid::Renderer
