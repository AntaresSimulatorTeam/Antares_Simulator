/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: MPL 2.0
*/

#include "drawingcontext.h"

namespace Antares
{
namespace Map
{
DrawingContext::DrawingContext(wxDC& dc,
                               const wxPoint& origin,
                               const wxPoint& offset,
                               const wxPoint& scroll,
                               const wxPoint& clientSize,
                               const wxPoint& btRight,
                               const size_t& layer,
                               bool forFileExport) :
 pDC(dc),
 pOrigin(origin),
 pOffsetForSelectedNodes(offset),
 pScroll(scroll),
 pClientSize(clientSize),
 pBottomRight(btRight),
 layerId(layer),
 pForFileExport(forFileExport)
{
}

} // namespace Map
} // namespace Antares
