// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef ANTARES_APPLICATION_MAIN_BUILD_STANDARD_PAGE_HXX__
#define ANTARES_APPLICATION_MAIN_BUILD_STANDARD_PAGE_HXX__

#include "../../../toolbox/components/notebook/notebook.h"
#include "../../../toolbox/create.h"
#include <ui/common/component/panel.h>

namespace Antares::Forms
{
/*!
** \brief Create a standard page for the main notebook
**
** Nearly all pages for the main notebook have another notebook
** linked with an input selector (an area, an interconnection...).
**
** \tparam InSel The class of the Input Selector
** \param parent The parent control
** \param name Name of the page
** \param title Title of the page
** \return A pair, which contains a new notebook, and a new input selector
*/
template<class InSel>
static std::pair<Component::Notebook*, InSel*> createStdNotebookPage(Component::Notebook* parent,
                                                                     const wxString& name,
                                                                     const wxString& title)
{
    Component::Notebook* n = new Component::Notebook(parent, Component::Notebook::orTop);
    n->caption(title);
    InSel* selector = new InSel(n);
    n->addCommonControl(selector);
    parent->add(n, name, title);
    return std::pair<Component::Notebook*, InSel*>(n, selector);
}

} // namespace Antares::Forms

#endif // ANTARES_APPLICATION_MAIN_BUILD_STANDARD_PAGE_HXX__
