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

#include "common.areasummary.h"
#include "toolbox/components/refresh.h"

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
CommonClusterSummarySingleArea::CommonClusterSummarySingleArea(
  wxWindow* control,
  Toolbox::InputSelector::Area* notifier) :
 pArea(nullptr), pControl(control), pAreaNotifier(notifier)
{
    if (notifier)
        notifier->onAreaChanged.connect(this, &CommonClusterSummarySingleArea::onAreaChanged);
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
        return IRenderer::cellStyleDefaultDisabled;
    else
    {
        if (col == 1 || col == 2)
            return IRenderer::cellStyleConstraintWeight;
        else
            return IRenderer::cellStyleDefault;
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
        onAreaChanged(nullptr);
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares