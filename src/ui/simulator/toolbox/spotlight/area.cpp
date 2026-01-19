// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "area.h"
#include <cassert>
#include "antares/resources/resources.h"

using namespace Yuni;

namespace Antares::Toolbox::Spotlight
{
ItemArea::ItemArea(Data::Area* a):
    area(a)
{
    assert(a);

    caption(a->name);
    group("Area");

    if (area->ui)
    {
        addTag(wxT("  "), area->ui->color[0], area->ui->color[1], area->ui->color[2]);
    }
}

ItemArea::~ItemArea()
{
}

} // namespace Antares::Toolbox::Spotlight
