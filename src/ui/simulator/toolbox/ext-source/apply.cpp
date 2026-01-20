// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "apply.h"
#include "window.h"
#include "performer.h"

namespace Antares::ExtSource::Handler
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

} // namespace Antares::ExtSource::Handler
