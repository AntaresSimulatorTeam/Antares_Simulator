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
#ifndef ANTARES_APPLICATION_MAIN_BUILD_STANDARD_PAGE_HXX__
#define ANTARES_APPLICATION_MAIN_BUILD_STANDARD_PAGE_HXX__

#include <antares/wx-wrapper.h>
#include "../../../toolbox/components/notebook/notebook.h"
#include "../../../toolbox/create.h"
#include <ui/common/component/panel.h>

namespace Antares
{
namespace Forms
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

} // namespace Forms
} // namespace Antares

#endif // ANTARES_APPLICATION_MAIN_BUILD_STANDARD_PAGE_HXX__
