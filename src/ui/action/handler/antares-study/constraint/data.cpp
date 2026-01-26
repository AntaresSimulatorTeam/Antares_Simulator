// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "data.h"
#include <antares/utils/utils.h>

namespace Antares::Action::AntaresStudy::Constraint
{
bool Data::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
        pInfos.message << "The Bounds/RHS will be copied";
        break;
    default:
        pInfos.state = stNothingToDo;
        break;
    }

    return true;
}

bool Data::performWL(Context& ctx)
{
    if (ctx.constraint && ctx.extStudy)
    {
        Antares::Data::ConstraintName id;
        TransformNameIntoID(pOriginalConstraintName, id);

        auto source = ctx.extStudy->bindingConstraints.find(id);

        if (source && source != ctx.constraint)
        {
            // Deleted some code. UI is deprecated but not yet removed
            return true;
        }
    }
    return false;
}

} // namespace Antares::Action::AntaresStudy::Constraint
