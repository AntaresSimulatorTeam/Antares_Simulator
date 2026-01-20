// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "enabled.h"
#include <antares/utils/utils.h>

namespace Antares::Action::AntaresStudy::Constraint
{
bool Enabled::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
        pInfos.message << "The Enabled will be copied";
        break;
    default:
        pInfos.state = stNothingToDo;
        break;
    }

    return true;
}

bool Enabled::performWL(Context& ctx)
{
    if (ctx.constraint && ctx.extStudy)
    {
        Antares::Data::ConstraintName id;
        TransformNameIntoID(pOriginalConstraintName, id);

        auto source = ctx.extStudy->bindingConstraints.find(id);

        if (source && source != ctx.constraint)
        {
            ctx.constraint->enabled(source->enabled());
            return true;
        }
    }
    return false;
}

} // namespace Antares::Action::AntaresStudy::Constraint
