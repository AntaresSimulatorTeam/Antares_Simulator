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
#ifndef __ANTARES_TOOLBOX_FWD_H__
#define __ANTARES_TOOLBOX_FWD_H__

// Wx stuff
class wxPanel;
class wxDialog;
class wxBoxSizer;
class wxSizer;
class wxWindow;
class wxStaticText;
class wxChoice;
class wxTextCtrl;
class wxButton;
class wxGrid;
class wxColour;
class wxSplitterWindow;
class wxTimer;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
// The custom wxGridTableBase used by Component
class VGridHelper;
class DBGrid;
class Component;

} // namespace Datagrid
} // namespace Component
} // namespace Antares

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
class Component;

} // namespace HTMLListbox
} // namespace Component
} // namespace Antares

namespace Antares
{
namespace Component
{
class Panel;
class Button;
class Notebook;
class Spotlight;

} // namespace Component
} // namespace Antares

namespace Antares
{
namespace Component
{
class MainPanel;

} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_FWD_H__
