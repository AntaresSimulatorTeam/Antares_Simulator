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

#include "apply.h"
#include "window.h"
#include "performer.h"

namespace Antares
{
namespace ExtSource
{
namespace Handler
{
void Apply(Antares::Action::Context::Ptr context,
           Antares::Action::IAction::Ptr root,
           bool windowRequired)
{
    // prepare all actions
    if (!(!root))
    {
        root->prepare(*context);

        if (windowRequired)
        {
            auto* form = new Antares::Window::ApplyActionsDialog(nullptr, context, root);
            Dispatcher::GUI::ShowModal(form);
        }
        else
        {
            auto* form = new Antares::Window::PerformerDialog(nullptr, context, root);
            Dispatcher::GUI::ShowModal(form);
        }
    }
    else
    {
        // should never happend
        assert(false && "copy/paster: apply: invalid root pointer");
    }
}

} // namespace Handler
} // namespace ExtSource
} // namespace Antares
