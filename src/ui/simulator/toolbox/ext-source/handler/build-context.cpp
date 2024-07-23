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
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "build-context.h"
#include "../../../windows/inspector.h"

namespace Antares
{
namespace ExtSource
{
namespace Handler
{
BuildContext::BuildContext() : shouldOverwriteArea(true), modifiedWhenCopied(false)
{
}

bool BuildContext::checkIdentity()
{
    if (Antares::Window::Inspector::SelectionAreaCount() == area.size()
        && Antares::Window::Inspector::SelectionLinksCount() == link.size()
        && Antares::Window::Inspector::SelectionThermalClusterCount() == cluster.size()
        && Antares::Window::Inspector::SelectionBindingConstraintCount() == constraint.size())
    {
        if (!Antares::Window::Inspector::AreasSelected(area, forceAreaName))
            return false;
        if (!Antares::Window::Inspector::ConstraintsSelected(constraint))
            return false;
        if (!Antares::Window::Inspector::LinksSelected(link))
            return false;
        return true;
    }
    return false;
}

bool BuildContext::checkIntegrity(bool forceDialog)
{
    // If the user-selection matches the imported items, there is
    if (checkIdentity())
    {
        shouldOverwriteArea = false;
        return true;
    }

    // Get if there are multiple inputs
    size_t inputCount = area.size() + link.size() + cluster.size();
    uint selectionCount = Antares::Window::Inspector::SelectionTotalCount();

    if (inputCount > 1)
    {
        if (selectionCount > 0)
        {
            bool error = false;
            // check that any selected item has a twin in the clipboard
            if (!Antares::Window::Inspector::AreasSelected(area, forceAreaName))
                error = true;
            if (!Antares::Window::Inspector::ConstraintsSelected(constraint))
                error = true;
            if (!Antares::Window::Inspector::LinksSelected(link))
                error = true;
            else
            {
                // forceAreaName[*(area.begin())] = name;
                shouldOverwriteArea = true;
            }
            // check that any selected link has a twin in the clipboard

            // check that any selected constraint has a twin in the clipboard
            if (forceDialog)
            {
                shouldOverwriteArea = true;
                // return true;
            }

            if (error)
            {
                logs.error() << "Impossible to paste multiple items into the current selection "
                             << "(ambigous signature resolution). "
                             << "Please unselect first."; // or use the 'Paste special' command
                return false;
            }
            else
                return true;
        }
    }
    else
    {
        switch (selectionCount)
        {
        case 1: // one to one paste
        {
            if (Antares::Window::Inspector::SelectionAreaCount())
            {
                if (!area.empty())
                {
                    Data::AreaName name;
                    Antares::Window::Inspector::FirstSelectedArea(name);
                    forceAreaName[*(area.begin())] = name;
                    shouldOverwriteArea = true;
                    return true;
                }
            }

            if (Antares::Window::Inspector::SelectionLinksCount())
            {
                if (!link.empty())
                {
                    Data::AreaLink* l;
                    Antares::Window::Inspector::FirstSelectedAreaLink(&l);
                    if (l)
                    {
                        LinkSet::const_iterator i = link.begin();
                        forceAreaName[i->first] = l->from->name;
                        forceAreaName[((i->second).begin())->first] = l->with->name;
                        shouldOverwriteArea = true;
                        return true;
                    }
                }
            }

            if (Antares::Window::Inspector::SelectionThermalClusterCount() == cluster.size())
            {
                if (!cluster.empty())
                {
                    shouldOverwriteArea = true;
                    return true;
                }
            }

            if (!forceDialog)
            {
                logs.error() << "The destination is incompatible with the item in the clipboard.";
                return false;
            }
            break;
        }
        }
    }

    return true;
}

} // namespace Handler
} // namespace ExtSource
} // namespace Antares
